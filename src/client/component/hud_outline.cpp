#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "console.hpp"

#include "gsc/script_extension.hpp"
#include "gsc/script_error.hpp"

#include "game/dvars.hpp"

#include "scheduler.hpp"

#include <utils/hook.hpp>

namespace hud_outline
{
    void r_update_code_constant_from_vec4(game::GfxCmdBufSourceState* source, unsigned int constant, const float* value)
    {
        if (source->input.consts[constant][0] != value[0] ||
            source->input.consts[constant][1] != value[1] ||
            source->input.consts[constant][2] != value[2] ||
            source->input.consts[constant][3] != value[3])
        {
            source->input.consts[constant][0] = value[0];
            source->input.consts[constant][1] = value[1];
            source->input.consts[constant][2] = value[2];
            source->input.consts[constant][3] = value[3];
            ++source->constVersions[constant];
        }
    }

#define psoutlineDataBits \
    ((unsigned int*)(&ps->objective[35]))

    constexpr unsigned int max_outlines = 32;
    constexpr unsigned int max_clients = 18;

    // reimplemented structs
    namespace level
    {
        game::outline_data_t outlineData[max_outlines];
    }
    namespace cg_s
    {
        bool scopeForceEnemyOutlines;
        int scopeForceEnemyOutlineColorIndex;
    }
    namespace scene
    {
        game::GfxDrawSurf drawSurfsHudOutline[256];
    }
    namespace view_info
    {
        game::GfxHudOutlineState hudOutline;
    }

    game::GfxDrawList hudOutlineDrawList{};

#define HUD_OUTLINE_FLAG 0x4000000

    game::dvar_t* r_hudOutlineEnable = nullptr;
    game::dvar_t* r_hudOutlinePostMode = nullptr;
    game::dvar_t* r_hudOutlineWhen = nullptr;
    game::dvar_t* r_hudOutlineWidth = nullptr;
    game::dvar_t* r_hudOutlineAlpha0 = nullptr;
    game::dvar_t* r_hudOutlineAlpha1 = nullptr;
    game::dvar_t* r_hudOutlineHaloWhen = nullptr;
    game::dvar_t* r_hudOutlineHaloBlurRadius = nullptr;
    game::dvar_t* r_hudOutlineHaloLumScale = nullptr;
    game::dvar_t* r_hudOutlineHaloDarkenScale = nullptr;
    game::dvar_t* r_hudOutlineCurvyWhen = nullptr;
    game::dvar_t* r_hudOutlineCurvyBlurRadius = nullptr;
    game::dvar_t* r_hudOutlineCurvyWidth = nullptr;
    game::dvar_t* r_hudOutlineCurvyDepth = nullptr;
    game::dvar_t* r_hudOutlineCurvyLumScale = nullptr;
    game::dvar_t* r_hudOutlineCurvyDarkenScale = nullptr;
    game::dvar_t* r_hudOutlineCloakWhen = nullptr;
    game::dvar_t* r_hudOutlineCloakBlurRadius = nullptr;
    game::dvar_t* r_hudOutlineCloakLumScale = nullptr;
    game::dvar_t* r_hudOutlineCloakDarkenScale = nullptr;

    const char* s_hudOutlinePostModeNames[] = {
        "pixel",
        "(deprecated)",
        "halo",
        "curvy",
        "cloak",
        nullptr
    };

    const char* s_hudOutlineWhenNames[] = {
        "after_colorize",
        "after_fxaa",
        "before_fxaa",
        "before_hudfx",
        nullptr
    };

    void R_RegisterHudOutlineDvars()
    {
        r_hudOutlineEnable = dvars::register_bool(
            "r_hudOutlineEnable",
            true,
            0x44u,
            "Enables wireframe outlines to be drawn around DObjs (as a post process).");
        r_hudOutlinePostMode = dvars::register_enum(
            "r_hudOutlinePostMode",
            s_hudOutlinePostModeNames,
            game::R_HUD_OUTLINE_POST_MODE_PIXEL,
            0x44u,
            "hud outline apply mode");
        r_hudOutlineWhen = dvars::register_enum(
            "r_hudOutlineWhen",
            s_hudOutlineWhenNames,
            game::R_HUD_OUTLINE_WHEN_AFTER_FXAA,
            0x44u,
            "Controls when the pixel-mode HUD outline post effect is applied.");
        r_hudOutlineWidth = dvars::register_float("r_hudOutlineWidth", 1.0f, 0.0f, 20.0f, 0x44u, "Set the width of the Hud Outline");
        r_hudOutlineAlpha0 = dvars::register_float("r_hudOutlineAlpha0", 0.85000002f, 0.0f, 1.0f, 0x44u, "Set the alpha of the first outline");
        r_hudOutlineAlpha1 = dvars::register_float("r_hudOutlineAlpha1", 0.64999998f, 0.0f, 1.0f, 0x44u, "Set the alpha of the second outline");
        r_hudOutlineHaloWhen = dvars::register_enum(
            "r_hudOutlineHaloWhen",
            s_hudOutlineWhenNames,
            game::R_HUD_OUTLINE_WHEN_BEFORE_FXAA,
            0x44u,
            "Controls when the halo-mode HUD outline post effect is applied.");
        r_hudOutlineHaloBlurRadius = dvars::register_float("r_hudOutlineHaloBlurRadius", 1.0f, 0.0f, 10.0f, 0x44u, "Radius of the gaussian blur applied to the outline color buffer.");
        r_hudOutlineHaloLumScale = dvars::register_float("r_hudOutlineHaloLumScale", 1.0f, 0.0f, 10.0f, 0x44u, "Scale applied to the output luminance.");
        r_hudOutlineHaloDarkenScale = dvars::register_float("r_hudOutlineHaloDarkenScale", 1.0f, 0.0f, 10.0f, 0x44u, "Amount to darken the background around the outline.  A value of 0.0 is pure-additive; a value of 1.0 is a normal blend; values > 1.0 overdarken.");
        r_hudOutlineCurvyWhen = dvars::register_enum(
            "r_hudOutlineCurvyWhen",
            s_hudOutlineWhenNames,
            game::R_HUD_OUTLINE_WHEN_AFTER_FXAA,
            0x44u,
            "Controls when the curvy-mode HUD outline post effect is applied.");
        r_hudOutlineCurvyBlurRadius = dvars::register_float("r_hudOutlineCurvyBlurRadius", 1.5f, 0.0f, 10.0f, 0x44u, "Radius of the gaussian blur applied to the outline color buffer.");
        r_hudOutlineCurvyWidth = dvars::register_float("r_hudOutlineCurvyWidth", 0.25f, 0.0f, 1.0f, 0x44u, "Width of the outline, proportional to r_hudOutlineCurvyBlurRadius");
        r_hudOutlineCurvyDepth = dvars::register_float("r_hudOutlineCurvyDepth", 0.44999999f, 0.0f, 1.0f, 0x44u, "Overlap depth of the outline around the model.  This dictates the inside/outside tolerance.  Larger values move the outline inward.");
        r_hudOutlineCurvyLumScale = dvars::register_float("r_hudOutlineCurvyLumScale", 1.0f, 0.0f, 10.0f, 0x44u, "Scale applied to the output luminance.");
        r_hudOutlineCurvyDarkenScale = dvars::register_float("r_hudOutlineCurvyDarkenScale", 1.0f, 0.0f, 10.0f, 0x44u, "Amount to darken the background around the outline.  A value of 0.0 is pure-additive; a value of 1.0 is a normal blend; values > 1.0 overdarken which may be useful for a "
            "double-outline effect.");
        r_hudOutlineCloakWhen = dvars::register_enum(
            "r_hudOutlineCloakWhen",
            s_hudOutlineWhenNames,
            game::R_HUD_OUTLINE_WHEN_BEFORE_FXAA,
            0x44u,
            "Controls when the cloak-mode HUD outline post effect is applied.");
        r_hudOutlineCloakBlurRadius = dvars::register_float("r_hudOutlineCloakBlurRadius", 0.34999999f, 0.0f, 10.0f, 0x44u, "");
        r_hudOutlineCloakLumScale = dvars::register_float("r_hudOutlineCloakLumScale", 0.75f, 0.0f, 10.0f, 0x44u, "");
        r_hudOutlineCloakDarkenScale = dvars::register_float("r_hudOutlineCloakDarkenScale", 1.0f, 0.0f, 10.0f, 0x44u, "");
    }

    game::dvar_t* hudOutlineDuringADS = nullptr;
    game::dvar_t* hudOutlineDuringADSEnemyColor = nullptr;
    game::dvar_t* hudOutlineDuringADSFriendlyColor = nullptr;

    const char* s_hudOutlineColors[] = {
        "none",
        "red",
        "green",
        "blue",
        "orange",
        "yellow",
        "dark blue",
        nullptr
    };

    void BG_RegisterHudOutlineDvars()
    {
        hudOutlineDuringADS = dvars::register_bool("hudOutlineDuringADS", false, 0, "Turn on the HUD outline when you are pointing at a player while in ADS.");
        hudOutlineDuringADSEnemyColor = dvars::register_enum(
            "hudOutlineDuringADSEnemyColor",
            s_hudOutlineColors,
            1,
            0,
            "HUD outline color for enemies when you are pointing at a player while in ADS.");
        hudOutlineDuringADSFriendlyColor = dvars::register_enum(
            "hudOutlineDuringADSFriendlyColor",
            s_hudOutlineColors,
            2,
            0,
            "HUD outline color for enemies when you are pointing at a player while in ADS.");
    }

    void G_HudOutline_SetClientBits(
        game::playerState_s* ps,
        unsigned int outlineIndex,
        bool enabled,
        unsigned char color_index,
        bool depth_enable)
    {
        const unsigned char value = enabled ? (color_index + 1) : 0;
        const int base = static_cast<int>(outlineIndex) * 4;
        auto* bits = psoutlineDataBits;

        // bit 0
        const int idx0 = base;
        if (value & 1)
            bits[idx0 >> 5] |= (1u << (idx0 & 0x1F));
        else
            bits[idx0 >> 5] &= ~(1u << (idx0 & 0x1F));

        // bit 1
        const int idx1 = base + 1;
        if (value & 2)
            bits[idx1 >> 5] |= (1u << (idx1 & 0x1F));
        else
            bits[idx1 >> 5] &= ~(1u << (idx1 & 0x1F));

        // bit 2
        const int idx2 = base + 2;
        if (value & 4)
            bits[idx2 >> 5] |= (1u << (idx2 & 0x1F));
        else
            bits[idx2 >> 5] &= ~(1u << (idx2 & 0x1F));

        // bit 3 (depth_enable)
        const int idx3 = base + 3;
        if (depth_enable)
            bits[idx3 >> 5] |= (1u << (idx3 & 0x1F));
        else
            bits[idx3 >> 5] &= ~(1u << (idx3 & 0x1F));
    }

    void G_HudOutline_UpdateClientBits()
    {
        for (unsigned int client_index = 0; client_index < max_clients; ++client_index)
        {
            if ((*game::mp::clients)[client_index].sess.connected != game::CON_CONNECTED)
            {
                continue;
            }

            auto* ps = &(*game::mp::clients)[client_index].ps;

            psoutlineDataBits[0] = 0;
            psoutlineDataBits[1] = 0;
            psoutlineDataBits[2] = 0;
            psoutlineDataBits[3] = 0;

            for (unsigned int outline_index = 0; outline_index < max_outlines; ++outline_index)
            {
                const auto& outline = level::outlineData[outline_index];

                if ((outline.enabledForClientMask & (1u << client_index)) == 0)
                {
                    continue;
                }

                G_HudOutline_SetClientBits(
                    ps,
                    outline_index,
                    true,
                    outline.colorIndexForClient[client_index],
                    outline.depthEnableForClient[client_index]);
            }
        }
    }

    void G_HudOutline_UpdateSingleClientBits(int clientIndex, game::gclient_s* client)
    {
        auto* ps = &client->ps;
        auto* data = psoutlineDataBits;

        data[0] = 0;
        data[1] = 0;
        data[2] = 0;
        data[3] = 0;

        for (unsigned int outline_index = 0; outline_index < max_outlines; ++outline_index)
        {
            const auto& outline = level::outlineData[outline_index];
            if ((outline.enabledForClientMask & (1u << clientIndex)) == 0)
            {
                continue;
            }

            G_HudOutline_SetClientBits(
                &client->ps,
                outline_index,
                true,
                outline.colorIndexForClient[clientIndex],
                outline.depthEnableForClient[clientIndex]);
        }
    }

    void G_EntSetHudOutline(game::gentity_s* ent, int enable)
    {
        int* e_flags_ptr = nullptr;

        if (auto* client = ent->client)
            e_flags_ptr = &client->ps.eFlags;
        else if (auto* agent = ent->agent)
            e_flags_ptr = &agent->ps.eFlags;
        else
            e_flags_ptr = &ent->s.lerp.eFlags;

        if (enable)
            *e_flags_ptr |= HUD_OUTLINE_FLAG;
        else
            *e_flags_ptr &= ~HUD_OUTLINE_FLAG;
    }

    void G_EntSetHudOutlineInfo(game::gentity_s* ent, unsigned int outline_info)
    {
        game::playerState_s* ps = nullptr;

        if (auto* client = ent->client)
            ps = &client->ps;
        else if (auto* agent = ent->agent)
            ps = &agent->ps;
        else
        {
            ent->s.hudData.data &= 0xFFFFF83F;
            ent->s.hudData.data |= (outline_info & 0x1F) << 6;
            return;
        }

        if (ps != nullptr)
        {
            ps->hudData.data &= 0xFFFFF83F;
            ps->hudData.data |= (outline_info & 0x1F) << 6;
        }
    }

    unsigned int G_EntGetHudOutlineInfo(const game::gentity_s* ent)
    {
        if (const auto* client = ent->client)
        {
            return (client->ps.hudData.data >> 6) & 0x1F;
        }
        else if (const auto* agent = ent->agent)
        {
            return (agent->ps.hudData.data >> 6) & 0x1F;
        }
        return (ent->s.hudData.data >> 6) & 0x1F;
    }

    bool G_EntHasHudOutline(const game::gentity_s* ent)
    {
        if (const auto* client = ent->client)
        {
            return client->ps.eFlags & HUD_OUTLINE_FLAG;
        }
        else if (const auto* agent = ent->agent)
        {
            return agent->ps.eFlags & HUD_OUTLINE_FLAG;
        }
        return ent->s.lerp.eFlags & HUD_OUTLINE_FLAG;
    }

    void G_HudOutline_InitEmptyData(game::outline_data_t* data)
    {
        data->enabledForClientMask = 0;
        for (int i = 0; i < max_clients; ++i)
        {
            data->colorIndexForClient[i] = 0;
            data->depthEnableForClient[i] = false;
        }
    }

    void G_HudOutline_CopyDataFromIndex(unsigned int index, game::outline_data_t* out)
    {
        const auto& src = level::outlineData[index];
        out->enabledForClientMask = src.enabledForClientMask;
        for (int i = 0; i < max_clients; ++i)
        {
            out->colorIndexForClient[i] = src.colorIndexForClient[i];
            out->depthEnableForClient[i] = src.depthEnableForClient[i];
        }
    }

    bool G_HudOutline_FindMatchingData(const game::outline_data_t* data, unsigned int* out_index)
    {
        for (unsigned int i = 0; i < max_outlines; ++i)
        {
            const auto& outline = level::outlineData[i];

            if (!outline.refCount || outline.enabledForClientMask != data->enabledForClientMask)
                continue;

            bool matches = true;
            for (int j = 0; j < max_clients; ++j)
            {
                if (outline.colorIndexForClient[j] != data->colorIndexForClient[j] ||
                    outline.depthEnableForClient[j] != data->depthEnableForClient[j])
                {
                    matches = false;
                    break;
                }
            }

            if (matches)
            {
                *out_index = i;
                return true;
            }
        }
        return false;
    }

    bool G_HudOutline_InitFirstEmptyData(const game::outline_data_t* data, unsigned int* out_index)
    {
        for (unsigned int i = 0; i < max_outlines; ++i)
        {
            if (level::outlineData[i].refCount != 0)
                continue;

            auto& outline = level::outlineData[i];
            outline.enabledForClientMask = data->enabledForClientMask;
            for (int j = 0; j < max_clients; ++j)
            {
                outline.colorIndexForClient[j] = data->colorIndexForClient[j];
                outline.depthEnableForClient[j] = data->depthEnableForClient[j];
            }

            assert(outline.refCount == 0);
            *out_index = i;
            return true;
        }
        return false;
    }

    void G_HudOutline_DecrementRefCount(unsigned int index)
    {
        auto& outline = level::outlineData[index];
        if (--outline.refCount == 0)
        {
            outline.enabledForClientMask = 0;
            for (int i = 0; i < max_clients; ++i)
            {
                outline.colorIndexForClient[i] = 0;
                outline.depthEnableForClient[i] = false;
            }
        }
    }

    void G_HudOutline_IncrementRefCount(unsigned int index)
    {
        ++level::outlineData[index].refCount;
    }

    void G_HudOutline_EnableForClientMask(
        game::gentity_s* subjectEnt,
        unsigned int addClientMask,
        int color_index,
        int depth_enable)
    {
        assert(subjectEnt);

        game::outline_data_t data{};

        const bool had_outline = G_EntHasHudOutline(subjectEnt) != 0;
        if (had_outline)
        {
            const unsigned int index = G_EntGetHudOutlineInfo(subjectEnt);
            G_HudOutline_CopyDataFromIndex(index, &data);
        }
        else
        {
            G_HudOutline_InitEmptyData(&data);
        }

        assert(color_index == static_cast<unsigned char>(color_index));
        const unsigned char color_byte = static_cast<unsigned char>(color_index);

        const unsigned int old_mask = data.enabledForClientMask;
        data.enabledForClientMask |= addClientMask;
        bool changed = (data.enabledForClientMask != old_mask);

        for (int i = 0; i < max_clients; ++i)
        {
            if (((1u << i) & addClientMask) == 0)
                continue;

            const unsigned char old_color = data.colorIndexForClient[i];
            const bool old_depth = data.depthEnableForClient[i];

            data.colorIndexForClient[i] = color_byte;
            data.depthEnableForClient[i] = (depth_enable != 0);

            changed |= (data.colorIndexForClient[i] != old_color) ||
                (data.depthEnableForClient[i] != old_depth);
        }

        if (!changed)
            return;

        if (had_outline)
        {
            const unsigned int old_index = G_EntGetHudOutlineInfo(subjectEnt);
            G_HudOutline_DecrementRefCount(old_index);
        }

        unsigned int outline_index = 0;

        if (G_HudOutline_FindMatchingData(&data, &outline_index))
        {
            // found existing matching slot, outline_index is set
        }
        else if (G_HudOutline_InitFirstEmptyData(&data, &outline_index))
        {
            // allocated a new slot, outline_index is set
        }
        else
        {
            console::warn("Warning: HudOutlineEnableForClient() could not be applied because there are currently too many hud outline entities\n");
            G_EntSetHudOutline(subjectEnt, 0);
            G_EntSetHudOutlineInfo(subjectEnt, 0);
            G_HudOutline_UpdateClientBits();
            return;
        }

        assert(outline_index < max_outlines);

        G_HudOutline_IncrementRefCount(outline_index);
        G_EntSetHudOutline(subjectEnt, 1);
        G_EntSetHudOutlineInfo(subjectEnt, outline_index);
        G_HudOutline_UpdateClientBits();
    }

    void G_HudOutline_DisableForClientMask(game::gentity_s* ent, unsigned int removeClientMask)
    {
        if (!G_EntHasHudOutline(ent))
            return;

        const unsigned int old_index = G_EntGetHudOutlineInfo(ent);
        game::outline_data_t data{};
        G_HudOutline_CopyDataFromIndex(old_index, &data);

        const unsigned int old_mask = data.enabledForClientMask;
        data.enabledForClientMask &= ~removeClientMask;
        bool changed = (data.enabledForClientMask != old_mask);

        for (int i = 0; i < max_clients; ++i)
        {
            if (((1u << i) & removeClientMask) == 0)
                continue;

            if (data.colorIndexForClient[i] != 0)
            {
                data.colorIndexForClient[i] = 0;
                changed = true;
            }

            if (data.depthEnableForClient[i])
            {
                data.depthEnableForClient[i] = false;
                changed = true;
            }
        }

        if (!changed)
            return;

        G_HudOutline_DecrementRefCount(old_index);

        unsigned int new_index = 0;

        if (data.enabledForClientMask == 0)
        {
            G_EntSetHudOutline(ent, 0);
            G_EntSetHudOutlineInfo(ent, 0);
            G_HudOutline_UpdateClientBits();
            return;
        }

        if (G_HudOutline_FindMatchingData(&data, &new_index))
        {
            // found existing matching slot
        }
        else if (G_HudOutline_InitFirstEmptyData(&data, &new_index))
        {
            // allocated a new slot
        }
        else
        {
            console::warn("Warning: HudOutlineDisableForClient() could not be applied because there are currently too many hud outline entities\n");
            G_EntSetHudOutline(ent, 0);
            G_EntSetHudOutlineInfo(ent, 0);
            G_HudOutline_UpdateClientBits();
            return;
        }

        assert(new_index < max_outlines);

        G_HudOutline_IncrementRefCount(new_index);
        G_EntSetHudOutline(ent, 1);
        G_EntSetHudOutlineInfo(ent, new_index);
        G_HudOutline_UpdateClientBits();
    }

    void G_HudOutline_FreeForClient(game::gentity_s* clientEnt)
    {
        assert(clientEnt);

        const unsigned int remove_mask = 1u << clientEnt->s.number;

        for (int i = 0; i < *game::mp::num_entities; ++i)
        {
            game::gentity_s* ent = &game::mp::g_entities[i];

            if (!ent->r.isInUse)
                continue;

            G_HudOutline_DisableForClientMask(ent, remove_mask);
        }

        for (int i = 0; i < max_outlines; ++i)
        {
            const auto& outline = level::outlineData[i];
            if (outline.enabledForClientMask)
                assert(outline.refCount > 0);
            else
                assert(outline.refCount == 0);
        }
    }

    void G_HudOutline_FreeForEnt(game::gentity_s* ent)
    {
        if (!G_EntHasHudOutline(ent))
        {
            return;
        }

        auto info = G_EntGetHudOutlineInfo(ent);
        G_HudOutline_DecrementRefCount(info);
        G_EntSetHudOutline(ent, 0);
        G_EntSetHudOutlineInfo(ent, 0);
        G_HudOutline_UpdateClientBits();
    }

    void G_HudOutline_AddForNewClient(game::gentity_s* clientEnt)
    {
        G_HudOutline_UpdateSingleClientBits(clientEnt->s.number, clientEnt->client);
    }

    int G_HudOutline_GetDepthEnableByIndex(game::playerState_s* ps, int outlineIndex)
    {
        const int idx = outlineIndex * 4 + 3;
        auto* bits = psoutlineDataBits;

        return (bits[idx >> 5] >> (idx & 0x1F)) & 1;
    }

    int G_HudOutline_GetColorIndex(game::playerState_s* ps, int outlineIndex)
    {
        return (2
            * ((2 * (((1 << ((4 * outlineIndex + 2) & 0x1F)) & psoutlineDataBits[(4 * outlineIndex + 2) >> 5]) != 0)) | (((1 << ((4 * outlineIndex + 1) & 0x1F)) & psoutlineDataBits[(4 * outlineIndex + 1) >> 5]) != 0))) | (((1 << ((4 * outlineIndex) & 0x1F)) & psoutlineDataBits[(4 * outlineIndex) >> 5]) != 0);
    }

    void CG_ApplyHudOutlineRenderFlags(
        game::cg_s* cgame,
        short entityNum,
        int eFlags,
        int outlineIndex,
        unsigned int* renderFlags)
    {
        int colorIndex = 0;
        bool forceDepthFlag = false;
        unsigned int clientType = 0;

        if ((eFlags & HUD_OUTLINE_FLAG) != 0)
            colorIndex = G_HudOutline_GetColorIndex(&cgame->predictedPlayerState, outlineIndex);

        const bool adsOutlineEnabled = hudOutlineDuringADS->current.enabled;

        bool applyFlags = false;

        if (!adsOutlineEnabled)
        {
            if (colorIndex > 0)
                applyFlags = true;
        }
        else
        {
            if (colorIndex > 0)
            {
                applyFlags = true;
            }
            else if ((cgame->predictedPlayerState.pm_flags & 0x10) != 0 &&
                cgame->crosshairClientNum == entityNum)
            {
                forceDepthFlag = true;

                clientType = cgame->crosshairClientType;
                if (clientType & 0x10)
                {
                    colorIndex = hudOutlineDuringADSEnemyColor->current.integer;
                    applyFlags = true;
                }
                else if (clientType & 8)
                {
                    colorIndex = hudOutlineDuringADSFriendlyColor->current.integer;
                    applyFlags = true;
                }
            }
        }

        if (!applyFlags)
            return;

        *renderFlags |= (2 * (colorIndex + 1)) & 0xE;

        if (forceDepthFlag ||
            G_HudOutline_GetDepthEnableByIndex(&cgame->predictedPlayerState, outlineIndex))
        {
            *renderFlags |= 0x4000;
        }
    }

    int ScrCmd_BuildHudOutlineClientMaskFromEntArray(const gsc::function_args& args)
    {
        const auto clients = args[0].as<scripting::array>();
        const auto clients_size = clients.size();
        if (!clients_size)
        {
            throw std::runtime_error("An empty array is not valid");
        }

        auto result = 0;

        for (auto i = 0; i < clients_size; ++i)
        {
            auto key = clients[i];
            auto entity = key.as<scripting::entity>();
            const auto id_ref = entity.get_entity_reference();
            if (id_ref.entnum < max_clients)
                result |= 1 << (id_ref.entnum & 0xFF);
            else
                throw std::runtime_error("All array elements need to be client entities.");
        }

        return result;
    }

    namespace
    {
        void objective_memset_stub(void* dst, int val, size_t size)
        {
            memset(dst, val, size); // level.objectives
            memset(level::outlineData, 0, sizeof(level::outlineData)); // level.outlineData
        }
    }

    void R_SetupViewUseHudOutline(game::GfxViewInfo* viewInfo/*, const game::GfxSceneParms* sceneParms*/)
    {
        const auto postMode = static_cast<game::R_HudOutlinePostMode>(r_hudOutlinePostMode->current.integer);
        const game::dvar_t* whenDvar = r_hudOutlineWhen;

        switch (postMode)
        {
        case game::R_HUD_OUTLINE_POST_MODE_HALO:
            whenDvar = r_hudOutlineHaloWhen;
            break;

        case game::R_HUD_OUTLINE_POST_MODE_CURVY:
            whenDvar = r_hudOutlineCurvyWhen;
            break;

        case game::R_HUD_OUTLINE_POST_MODE_CLOAK:
            whenDvar = r_hudOutlineCloakWhen;
            break;

        default:
            break;
        }

        auto& hudOutline = view_info::hudOutline;

        hudOutline.enable = r_hudOutlineEnable->current.enabled;
        hudOutline.postMode = static_cast<unsigned char>(postMode);

        hudOutline.when = static_cast<unsigned char>(whenDvar->current.unsignedInt);

        hudOutline.width = r_hudOutlineWidth->current.value;
        hudOutline.alpha0 = r_hudOutlineAlpha0->current.value;
        hudOutline.alpha1 = r_hudOutlineAlpha1->current.value;

        hudOutline.haloBlurRadius = r_hudOutlineHaloBlurRadius->current.value;
        hudOutline.haloLumScale = r_hudOutlineHaloLumScale->current.value;
        hudOutline.haloDarkenScale = r_hudOutlineHaloDarkenScale->current.value;

        hudOutline.curvyBlurRadius = r_hudOutlineCurvyBlurRadius->current.value;
        hudOutline.curvyWidth = r_hudOutlineCurvyWidth->current.value;
        hudOutline.curvyDepth = r_hudOutlineCurvyDepth->current.value;
        hudOutline.curvyLumScale = r_hudOutlineCurvyLumScale->current.value;
        hudOutline.curvyDarkenScale = r_hudOutlineCurvyDarkenScale->current.value;

        hudOutline.cloakBlurRadius = r_hudOutlineCloakBlurRadius->current.value;
        hudOutline.cloakLumScale = r_hudOutlineCloakLumScale->current.value;
        hudOutline.cloakDarkenScale = r_hudOutlineCloakDarkenScale->current.value;
    }

    int R_UsingHUDOutline()
    {
        return view_info::hudOutline.enable;
    }

    int R_IsHudOutline2Pass(game::R_HudOutlinePostMode postMode)
    {
        return postMode >= game::R_HUD_OUTLINE_POST_MODE_HALO;
    }

    int R_GetHudOutlineWhen()
    {
        return view_info::hudOutline.when;
    }

    game::Material* RB_HudOutlineGetGenerateMaterial(game::R_HudOutlinePostMode postMode/*, GfxMSAAState msaaState*/)
    {
        if (postMode == game::R_HUD_OUTLINE_POST_MODE_CURVY) 
            return game::Material_RegisterHandle("hud_outline_stencil_fill_curvy");

        return game::Material_RegisterHandle("hud_outline_stencil_fill");
    }

    game::Material* RB_HudOutlineGetApplyMaterial(game::R_HudOutlinePostMode postMode)
    {
        static game::Material* const materials[] =
        {
            game::Material_RegisterHandle("hud_outline_stencil_apply"),
            game::Material_RegisterHandle("hud_outline_stencil_apply"),
            game::Material_RegisterHandle("hud_outline_stencil_fill_apply"),
            game::Material_RegisterHandle("hud_outline_stencil_fill_apply_curvy"),
            game::Material_RegisterHandle("hud_outline_stencil_fill_apply_cloak")
        };

        const auto index = static_cast<std::size_t>(postMode);
        assert(index < std::size(materials));

        return materials[index];
    }

    game::R_HudOutlinePostMode RB_GetHudOutlineParms(float* outHudOutlineParms, game::GfxHudOutlineState* hudOutlineState)
    {
        const auto mode = static_cast<game::R_HudOutlinePostMode>(hudOutlineState->postMode);

        if (mode == game::R_HUD_OUTLINE_POST_MODE_PIXEL
            || mode == game::R_HUD_OUTLINE_POST_MODE_DEPRECATED)
        {
            static const auto r_aaSamples = game::Dvar_FindVar("r_aaSamples");
            outHudOutlineParms[0] = r_aaSamples ? r_aaSamples->current.integer : 1.0f;
            outHudOutlineParms[1] = hudOutlineState->alpha1;
            outHudOutlineParms[2] = hudOutlineState->alpha0;
            outHudOutlineParms[3] = hudOutlineState->width;
        }
        else if (mode == game::R_HUD_OUTLINE_POST_MODE_CURVY)
        {
            const float width = std::max(hudOutlineState->curvyWidth, 0.01f);
            const float depth = std::max(hudOutlineState->curvyDepth, 0.01f);
            const float scale = 2.0f / width;

            outHudOutlineParms[0] = scale;
            outHudOutlineParms[1] = -depth * scale;
            outHudOutlineParms[2] = hudOutlineState->curvyLumScale;
            outHudOutlineParms[3] = hudOutlineState->curvyDarkenScale;
        }
        else
        {
            outHudOutlineParms[0] = 0.0f;
            outHudOutlineParms[1] = 0.0f;

            if (mode == game::R_HUD_OUTLINE_POST_MODE_HALO)
            {
                outHudOutlineParms[2] = hudOutlineState->haloLumScale;
                outHudOutlineParms[3] = hudOutlineState->haloDarkenScale;
            }
            else if (mode == game::R_HUD_OUTLINE_POST_MODE_CLOAK)
            {
                outHudOutlineParms[2] = hudOutlineState->cloakLumScale;
                outHudOutlineParms[3] = hudOutlineState->cloakDarkenScale;
            }
        }

        return mode;
    }

    void RB_HudOutlineApplyLDR(game::GfxViewInfo* viewInfo, bool isFullScreen, game::GfxRenderTargetId renderTarget, game::GfxViewport* dstViewport)
    {
        game::GfxCmdBufContext context = *reinterpret_cast<game::GfxCmdBufContext*>(0x1408973F0);

        auto renderTargetFixed = renderTarget;
        if (game::gfxRenderTargets[renderTarget].imageAndSurface.surface.depthStencil)
        {
            if (renderTarget == game::R_RENDERTARGET_FRAME_BUFFER)
            {
                renderTargetFixed = game::R_RENDERTARGET_FRAME_BUFFER_NODEPTH;
            }
            else if (renderTarget == game::R_RENDERTARGET_SCENE)
            {
                renderTargetFixed = game::R_RENDERTARGET_SCENE_NODEPTH;
            }
            else
            {
                renderTargetFixed = game::R_RENDERTARGET_SCENE_PINGPONG_NODEPTH;
                assert(renderTarget == game::R_RENDERTARGET_SCENE_PINGPONG);
            }
        }

        game::R_SetRenderTargetSize(context.source, renderTargetFixed);
        game::R_SetRenderTarget(&context, renderTargetFixed);
        float outParms[4]{};
        auto postMode = RB_GetHudOutlineParms(outParms, &view_info::hudOutline);
        r_update_code_constant_from_vec4(context.source, 81, outParms);
        auto* material = RB_HudOutlineGetApplyMaterial(postMode);

        auto* stencil_image = *reinterpret_cast<game::GfxImage**>(0x141220518);
		context.source->input.codeImages[game::TEXTURE_SRC_CODE_RAW_STENCIL] = stencil_image;
        context.source->input.codeImageSamplerStates[game::TEXTURE_SRC_CODE_RAW_STENCIL] = 97;

        game::RB_ViewportFilter(material, dstViewport, isFullScreen);
    }

    void RB_HudOutlineStencilFillGenerate(game::GfxViewInfo* viewInfo, bool isFullScreen)
    {
        game::GfxViewport scaledViewport{};

        auto* renderTarget = &game::gfxRenderTargets[game::R_RENDERTARGET_POST_EFFECT_0];

        scaledViewport.x = renderTarget->width * viewInfo->sceneViewport.x / (short)game::vidConfig->renderWidth;
        scaledViewport.y = renderTarget->height * viewInfo->sceneViewport.y / (short)game::vidConfig->renderHeight;
        scaledViewport.width = renderTarget->width * viewInfo->sceneViewport.width / (short)game::vidConfig->renderWidth;
        scaledViewport.height = renderTarget->height * viewInfo->sceneViewport.height / (short)game::vidConfig->renderHeight;

        float blurRadius = 0.0f;
        const auto mode = static_cast<game::R_HudOutlinePostMode>(view_info::hudOutline.postMode);
        if (mode == game::R_HUD_OUTLINE_POST_MODE_HALO) blurRadius = view_info::hudOutline.haloBlurRadius;
        else if (mode == game::R_HUD_OUTLINE_POST_MODE_CURVY) blurRadius = view_info::hudOutline.curvyBlurRadius;
        else blurRadius = view_info::hudOutline.cloakBlurRadius;
        blurRadius = fmaxf(blurRadius, 0.1f);

        game::R_SetRenderTargetSize(game::gfxCmdBufContext->source, game::R_RENDERTARGET_POST_EFFECT_0);
        game::R_SetRenderTarget(game::gfxCmdBufContext, game::R_RENDERTARGET_POST_EFFECT_0);

        if (scaledViewport.x || scaledViewport.y || 
            scaledViewport.width != game::gfxCmdBufContext->source->renderTargetWidth)
        {
            float clearColor[4]{0.0f,0.0f,0.0f,0.0f};
            game::R_ClearScreen(game::gfxCmdBufState, 4u, clearColor, 0.0f, 0, nullptr);
        }

        auto savedViewportBehaviour = game::gfxCmdBufSourceState->viewportBehavior;
        auto savedViewport = game::gfxCmdBufSourceState->sceneViewport;
        game::gfxCmdBufSourceState->viewportBehavior = game::GFX_USE_VIEWPORT_FOR_VIEW;
        game::R_SetViewportStruct(game::gfxCmdBufContext->source, &scaledViewport);

        auto* stencilImage = *reinterpret_cast<game::GfxImage**>(0x141220518);
        game::gfxCmdBufSourceState->input.codeImages[game::TEXTURE_SRC_CODE_RAW_STENCIL] = stencilImage;
        game::gfxCmdBufSourceState->input.codeImageSamplerStates[game::TEXTURE_SRC_CODE_RAW_STENCIL] = 97;

        int imageWidth = stencilImage->width;
        int imageHeight = stencilImage->height;
        float invWidth = 1.0f / imageWidth;
        float invHeight = 1.0f / imageHeight;
        float parms[4]{ invWidth * -0.5f, invHeight * -0.5f, invWidth * 1.5f, invHeight * 1.5f };
        r_update_code_constant_from_vec4(game::gfxCmdBufContext->source, 81, parms);

        auto* material = RB_HudOutlineGetGenerateMaterial(mode);
        
        game::GfxViewport* activeViewport = isFullScreen ? &scaledViewport : &viewInfo->sceneViewport;
        game::RB_ViewportFilter(material, activeViewport, isFullScreen);

        game::R_SetViewportStruct(game::gfxCmdBufContext->source, &savedViewport);
        game::gfxCmdBufSourceState->viewportBehavior = savedViewportBehaviour;

        game::RB_GaussianFilterImageWithOptions(blurRadius, 
            game::R_RENDERTARGET_POST_EFFECT_0, game::R_RENDERTARGET_POST_EFFECT_1, viewInfo, 0x18);
    }

    void RB_HudOutlineStencilFillApply(game::GfxViewInfo* viewInfo, game::GfxRenderTargetId dstTarget, bool isFullScreen, game::GfxViewport* dstViewport)
    {
        game::gfxCmdBufSourceState->input.codeImages[game::TEXTURE_SRC_CODE_FEEDBACK] = game::gfxRenderTargets[game::R_RENDERTARGET_POST_EFFECT_1].imageAndSurface.image;

        auto renderTargetFixed = dstTarget;
        if (game::gfxRenderTargets[dstTarget].imageAndSurface.surface.depthStencil)
        {
            if (dstTarget == game::R_RENDERTARGET_FRAME_BUFFER)
            {
                renderTargetFixed = game::R_RENDERTARGET_FRAME_BUFFER_NODEPTH;
            }
            else if (dstTarget == game::R_RENDERTARGET_SCENE)
            {
                renderTargetFixed = game::R_RENDERTARGET_SCENE_NODEPTH;
            }
            else
            {
                renderTargetFixed = game::R_RENDERTARGET_SCENE_PINGPONG_NODEPTH;
                assert(dstTarget == game::R_RENDERTARGET_SCENE_PINGPONG);
            }
        }

        game::R_SetRenderTargetSize(game::gfxCmdBufContext->source, renderTargetFixed);
        game::R_SetRenderTarget(game::gfxCmdBufContext, renderTargetFixed);
        float outParms[4]{};
        auto postMode = RB_GetHudOutlineParms(outParms, &view_info::hudOutline);
        r_update_code_constant_from_vec4(game::gfxCmdBufContext->source, 81, outParms);
        auto* material = RB_HudOutlineGetApplyMaterial(postMode);

        game::RB_ViewportFilter(material, dstViewport, isFullScreen);
    }

    int RB_ApplyHudOutline(game::GfxViewInfo* viewInfo, bool isFullScreen, game::GfxRenderTargetId renderTarget, game::GfxViewport* dstViewport)
    {
        if (!R_UsingHUDOutline())
        {
            return 0;
        }

        if (R_IsHudOutline2Pass(static_cast<game::R_HudOutlinePostMode>(view_info::hudOutline.postMode)))
        {
            RB_HudOutlineStencilFillGenerate(viewInfo, isFullScreen);
            RB_HudOutlineStencilFillApply(viewInfo, renderTarget, isFullScreen, dstViewport);
        }
        else
        {
            RB_HudOutlineApplyLDR(viewInfo, isFullScreen, renderTarget, dstViewport);
        }

        return 1;
    }

    void RB_ApplyHudOutlineBeforeHudFX(game::GfxViewInfo* viewInfo)
    {
        if (R_GetHudOutlineWhen() == game::R_HUD_OUTLINE_WHEN_BEFORE_HUDFX)
        {
            RB_ApplyHudOutline(viewInfo, game::RB_IsSceneViewportFull(viewInfo), game::R_RENDERTARGET_SCENE, &viewInfo->sceneViewport);
        }
    }

    void R_ShutdownCmdBufState_stub(game::GfxCmdBufSourceState* source, game::GfxCmdBufState* state)
    {
        {
            if (R_GetHudOutlineWhen() == game::R_HUD_OUTLINE_WHEN_AFTER_FXAA || R_GetHudOutlineWhen() == game::R_HUD_OUTLINE_WHEN_BEFORE_FXAA)
            {
                auto* viewInfo = &(*game::backEndData)->viewInfo[(*game::backEndData)->viewInfoIndex];
                RB_ApplyHudOutline(viewInfo, game::RB_IsSceneViewportFull(viewInfo), game::R_RENDERTARGET_SCENE_NODEPTH, &viewInfo->sceneViewport);
            }
        }

        game::R_ShutdownCmdBufState(source, state);
    }

    void R_SetRenderTargetSize_stub(game::GfxCmdBufSourceState* source, game::GfxRenderTargetId newTargetId)
    {
        {
            auto* r_postfx_enable = game::Dvar_FindVar("r_postfx_enable");
            if (r_postfx_enable && r_postfx_enable->current.enabled)
            {
                if (R_GetHudOutlineWhen() == game::R_HUD_OUTLINE_WHEN_AFTER_COLORIZE)
                {
                    auto* viewInfo = &(*game::backEndData)->viewInfo[(*game::backEndData)->viewInfoIndex];
                    RB_ApplyHudOutline(viewInfo, game::RB_IsSceneViewportFull(viewInfo), newTargetId, &viewInfo->sceneViewport);
                }
            }
        }

        game::R_SetRenderTargetSize(source, newTargetId);
    }

    void sub_140628EE0_stub(game::GfxViewInfo* viewInfo)
    {
        utils::hook::invoke<void>(0x140628EE0, viewInfo);

        RB_ApplyHudOutlineBeforeHudFX(viewInfo);
    }

    void R_SetUI3DSamplerAndConstants_stub(game::GfxCmdBufInput* input, game::GfxUI3DBackend* rbUI3D)
    {
        auto* viewInfo = &(*game::frontEndDataOut)->viewInfo[(*game::frontEndDataOut)->viewInfoIndex];
        R_SetupViewUseHudOutline(viewInfo);

        utils::hook::invoke<void>(0x14009B240, input, rbUI3D);
    }

    void R_SubmitSurfaces_stub(game::GfxViewInfo* viewInfo,
        game::GfxViewParms* viewParmsDraw,
        game::GfxDrawList** drawList,
        game::GfxDrawListInfo** listInfo)
    {
        utils::hook::invoke<void>(0x1400F1B30, viewInfo, viewParmsDraw, drawList, listInfo);

        memset(&hudOutlineDrawList, 0, sizeof(hudOutlineDrawList));
        memset(scene::drawSurfsHudOutline, 0, sizeof(scene::drawSurfsHudOutline));

        hudOutlineDrawList.info.baseTechType = game::TECHNIQUE_UNLIT;
        hudOutlineDrawList.info.viewInfo = viewInfo;
        hudOutlineDrawList.info.eyeOffset[0] = viewParmsDraw->camera.origin[0];
        hudOutlineDrawList.info.eyeOffset[1] = viewParmsDraw->camera.origin[1];
        hudOutlineDrawList.info.eyeOffset[2] = viewParmsDraw->camera.origin[2];
        hudOutlineDrawList.info.cameraView = 1;
        hudOutlineDrawList.info.isNoSunShadow = 0;
        hudOutlineDrawList.info.codeSurfListType = game::GFX_CODE_SURF_LIST_INVALID;
        /// R_EmitDrawSurfListAndAddDrawCommands
        utils::hook::invoke<void>(0x1405F8A50,
            &scene::drawSurfsHudOutline[0],
            256,
            &hudOutlineDrawList);
    }

    utils::hook::detour CG_GetRenderFlagForRefEntity_hook;
    unsigned int CG_GetRenderFlagForRefEntity_stub(game::cg_s* cgameGlob, game::centity_s* cent, const game::DObj* obj, int eFlags)
	{
        unsigned int flags = CG_GetRenderFlagForRefEntity_hook.invoke<unsigned int>(cgameGlob, cent, obj, eFlags);

		if ((((eFlags & HUD_OUTLINE_FLAG) != 0) || cgameGlob->crosshairClientNum == static_cast<char>(cent->nextState.number))
            //&& !cgameGlob->scopeForceEnemyOutlines
            && cgameGlob->predictedPlayerState.pm_type != game::PM_INTERMISSION)
		{
			auto hudOutlineInfo = (cent->nextState.hudData.data >> 6) & 0x1F;
			CG_ApplyHudOutlineRenderFlags(cgameGlob, cent->nextState.number, eFlags, hudOutlineInfo, &flags);
		}

        return flags;
	}

    void EntHandleDissociate_stub(game::gentity_s* ent)
    {
        if (G_EntHasHudOutline(ent))
            G_HudOutline_FreeForEnt(ent);

        utils::hook::invoke<void>(0x140319360, ent);
    }

    void G_ClearDialogQueue_stub(game::gentity_s* ent)
    {
        G_HudOutline_FreeForClient(ent);

        utils::hook::invoke<void>(0x140078A10, ent);
    }

    void ClientSpawn_stub(utils::hook::assembler& a)
    {
        // original code
        a.mov(ptr(r14, 0x4D0C), ecx);
        a.mov(ptr(r14, 0x4C44), eax);

        a.pushad64();
        a.push(rcx);
        a.mov(rcx, rsi); // gentity
        a.call_aligned(G_HudOutline_AddForNewClient);
        a.pop(rcx);
        a.popad64();

        // continue
        a.jmp(0x140328368);
    }

    namespace
    {
        unsigned char byte_94F000[16] =
        {
            0x00, 0x10, 0x04, 0x02,
            0x12, 0x00, 0x10, 0xFF,
            0xFF, 0xFF, 0x00, 0x00,
            0x00, 0x00, 0x00, 0x00
        };

        void R_StencilOverrideUpdateStateBits(__m128i* context, unsigned int renderFlags)
        {
            auto base = context->m128i_i64[0];
            auto rend = context->m128i_i64[1];

            auto v5 = *reinterpret_cast<uint64_t*>(rend + 6376);
            auto v6 = *reinterpret_cast<uint64_t*>(v5 + 320);

            uint32_t index =
                *reinterpret_cast<uint32_t*>(rend + 6408) +
                (uint32_t)(*reinterpret_cast<uint8_t*>(*reinterpret_cast<int*>(rend + 6384) + v5 + 48));

            __m128i v23 = *reinterpret_cast<__m128i*>(v6 + index * 40);

            auto& state0 = reinterpret_cast<uint32_t*>(&v23)[1];
            auto& state1 = reinterpret_cast<uint32_t*>(&v23)[2];

            uint32_t v9, v11;
            uint32_t v3 = 0;

            if ((renderFlags & 0xE0000) && *reinterpret_cast<uint8_t*>(base + 12860) < 0x80)
            {
                uint32_t flags = 0xE02E8240;

                if (renderFlags & (1 << 18)) v3 = 9;
                else
                {
                    if (renderFlags & (1 << 19)) flags = 0xE02E0240;
                    v3 = 8;
                }

                v9 = flags | ((uint8_t*)&v23)[4];
                v11 = (state1 & 0xFF000000) | v3 | (v3 << 8) | (v3 << 16);

                state0 = v9;
                state1 = v11;
            }
            else if (!(renderFlags & 0x401E))
            {
                v9 = state0;
                v11 = state1;
            }
            else
            {
                int mode = *reinterpret_cast<int*>(base + 12832);
                unsigned char bits = byte_94F000[mode];

                uint32_t flags = 0xE02E8240;
                if (mode == 3 || (renderFlags & (1 << 14)))
                    flags = 0xE02E0240;

                bits = (unsigned char)(((renderFlags >> 1) << 5) | bits);

                v9 = flags | ((uint8_t*)&v23)[4];
                v11 = (state1 & 0xFF000000) | bits | (bits << 8) | (bits << 16);

                state0 = v9;
                state1 = v11;
            }

            if (*reinterpret_cast<int*>(base + 12832) >= 3 ||
                ((*reinterpret_cast<uint8_t*>(base + 12860) & 0x40) &&
                    (reinterpret_cast<uint32_t*>(&v23)[3] & 0x10000000)))
            {
                v9 |= 1u;
                state0 = v9;
            }

            auto rs = rend;

            if (((*reinterpret_cast<uint32_t*>(rs + 8200) ^ v9) & 0xFFFFFFCF) ||
                ((*reinterpret_cast<uint32_t*>(rs + 8204) ^ v11) & 0xFFFFFF))
            {
                uint32_t matIdx = (v11 >> 16) & 0xFF;

                auto material = utils::hook::invoke<uint64_t>(
                    0x1405DE2A0,
                    &v23,
                    ((uint8_t*)&v23)[8],
                    (v11 >> 8) & 0xFF
                );

                if (matIdx != *reinterpret_cast<uint32_t*>(rs + 8304) ||
                    material != *reinterpret_cast<uint64_t*>(rs + 8344))
                {
                    auto device = *reinterpret_cast<uint64_t*>(rs);
                    auto fn = *reinterpret_cast<void(__fastcall**)(__int64, __int64, uint64_t)>(
                        *reinterpret_cast<uint64_t*>(device) + 288
                        );

                    fn(device, material, matIdx);

                    *reinterpret_cast<uint64_t*>(rs + 8344) = material;
                    *reinterpret_cast<uint32_t*>(rs + 8304) = matIdx;
                }

                *reinterpret_cast<uint32_t*>(rs + 8200) = v9;
                *reinterpret_cast<uint32_t*>(rs + 8204) = v11;
            }
        }
    }

    namespace
    {
        utils::hook::detour MSG_WriteDeltaPlayerstate_hook;
        void MSG_WriteDeltaPlayerstate_stub(void* snapInfo,
            game::msg_t* msg,
            int time,
            game::playerState_s* from,
            game::playerState_s* to)
        {
            MSG_WriteDeltaPlayerstate_hook.invoke<void>(snapInfo, msg, time, from, to);
            
            if (!from || !memcmp(&from->objective[35], &to->objective[35], sizeof(to->objective[35])))
            {
                game::MSG_WriteBit0(msg);
            }
            else
            {
                auto* values = reinterpret_cast<int*>(&to->objective[35]);
                game::MSG_WriteBit1(msg);
                game::MSG_WriteLong(msg, values[0]);
                game::MSG_WriteLong(msg, values[1]);
                game::MSG_WriteLong(msg, values[2]);
                game::MSG_WriteLong(msg, values[3]);
            }
        }

        utils::hook::detour MSG_ReadDeltaPlayerstate_hook;
        void MSG_ReadDeltaPlayerstate_stub(const int localClientNum,
            game::msg_t* msg,
            const int time,
            game::playerState_s* from,
            game::playerState_s* to,
            bool predictedFieldsIgnoreXor)
        {
            MSG_ReadDeltaPlayerstate_hook.invoke<void>(localClientNum, msg, time, from, to, predictedFieldsIgnoreXor);

            if (game::MSG_ReadBit(msg))
            {
                auto* values = reinterpret_cast<int*>(&to->objective[35]);
                values[0] = game::MSG_ReadLong(msg);
                values[1] = game::MSG_ReadLong(msg);
                values[2] = game::MSG_ReadLong(msg);
                values[3] = game::MSG_ReadLong(msg);
            }
        }
    }

    class component final : public component_interface
    {
    public:
        void post_unpack() override
        {
            // todo:
            // - implement cg_s logic for scopes

            scheduler::once([]()
            {
                R_RegisterHudOutlineDvars();
            }, scheduler::renderer);
            scheduler::once([]()
            {
                BG_RegisterHudOutlineDvars();
            }, scheduler::main);

            // add outline data memset from G_InitGame
            utils::hook::call(0x140353FEA, objective_memset_stub);

            // reimplement GSC functions
            script_extension::add_method("hudoutlineenableforclients", [](const game::scr_entref_t ent, const gsc::function_args& args)
            {
                if (args.size() < 3)
                {
                    throw std::runtime_error("usage: HudOutlineEnableForClients( <client array>, <color_index>, <depth_enable> );");
                }

                auto* entity = &game::mp::g_entities[ent.entnum];

                auto client_array = ScrCmd_BuildHudOutlineClientMaskFromEntArray(args);
                const auto color_index = args[1].as<int>();
                const auto depth_enable = args[2].as<int>();

                G_HudOutline_EnableForClientMask(entity, client_array, color_index, depth_enable != 0);
                return scripting::script_value{};
            });

            script_extension::add_method("hudoutlineenableforclient", [](const game::scr_entref_t ent, const gsc::function_args& args)
            {
                if (args.size() < 3)
                {
                    throw std::runtime_error("usage: HudOutlineEnableForClient( <client>, <color_index>, <depth_enable> );");
                }

                auto* entity = &game::mp::g_entities[ent.entnum];

                auto client = args[0].as<scripting::entity>();
                auto* client_entity = &game::mp::g_entities[client.get_entity_reference().entnum];

                if (!client_entity || !client_entity->client)
                {
                    throw std::runtime_error("Invalid client entity.");
                }

                const auto client_num = client_entity->s.number;
                if (client_num < 0 || client_num >= max_clients)
                {
                    throw std::runtime_error("client entity number out of range.");
                }

                const auto color_index = args[1].as<int>();
                if (color_index < 0 || color_index > 6)
                {
                    throw std::runtime_error("color_index must be >= 0 and <= 6.");
                }

                const auto depth_enable = args[2].as<int>();

                G_HudOutline_EnableForClientMask(entity, 1u << client_num, color_index, depth_enable != 0);
                return scripting::script_value{};
            });

            script_extension::add_method("hudoutlinedisableforclient", [](const game::scr_entref_t ent, const gsc::function_args& args)
            {
                if (args.size() < 1)
                {
                    throw std::runtime_error("usage: HudOutlineDisableForClient( <client> );");
                }

                auto* entity = &game::mp::g_entities[ent.entnum];

                auto client = args[0].as<scripting::entity>();
                auto* client_entity = &game::mp::g_entities[client.get_entity_reference().entnum];

                if (!client_entity || !client_entity->client)
                {
                    throw std::runtime_error("Invalid client entity.");
                }

                const auto client_num = client_entity->s.number;
                if (client_num < 0 || client_num >= max_clients)
                {
                    throw std::runtime_error("client entity number out of range.");
                }

                G_HudOutline_DisableForClientMask(entity, 1u << client_num);
                return scripting::script_value{};
            });

            script_extension::add_method("hudoutlinedisableforclients", [](const game::scr_entref_t ent, const gsc::function_args& args)
            {
                if (args.size() < 1)
                {
                    throw std::runtime_error("usage: HudOutlineDisableForClients( <client array> );");
                }

                auto* entity = &game::mp::g_entities[ent.entnum];

                auto client_array = ScrCmd_BuildHudOutlineClientMaskFromEntArray(args);

                G_HudOutline_DisableForClientMask(entity, client_array);
                return scripting::script_value{};
            });

            script_extension::add_method("hudoutlineenable", [](const game::scr_entref_t ent, const gsc::function_args& args)
            {
                if (args.size() < 1)
                {
                    throw std::runtime_error("usage: HudOutlineEnable( <color_index> [, <depth_enable>] );");
                }

                auto* entity = &game::mp::g_entities[ent.entnum];

                const auto color_index = args[0].as<int>();
                if (color_index < 0 || color_index > 6)
                {
                    throw std::runtime_error("color_index must be >= 0 and <= 6.");
                }

                const auto depth_enable = args.size() > 1 ? args[1].as<int>() : 0;

                G_HudOutline_EnableForClientMask(entity, ~0u, color_index, depth_enable != 0);
                return scripting::script_value{};
            });

            script_extension::add_method("hudoutlinedisable", [](const game::scr_entref_t ent, const gsc::function_args& args)
            {
                auto* entity = &game::mp::g_entities[ent.entnum];

                G_HudOutline_DisableForClientMask(entity, ~0u);
                return scripting::script_value{};
            });

            utils::hook::call(0x14062872D, R_ShutdownCmdBufState_stub); // RB_ApplyHudOutline in RB_EndSceneResolve
            utils::hook::call(0x140630A00, R_SetRenderTargetSize_stub); // RB_ApplyHudOutline in RB_ApplyPostEffects
            utils::hook::call(0x1406284BC, sub_140628EE0_stub);         // // RB_ApplyHudOutlineBeforeHudFX in RB_StandardDrawCommands, this doesn't work even in 1.15

            // R_SetupViewUseHudOutline in R_SetupViewInfo
            utils::hook::call(0x1400F0FA8, R_SetUI3DSamplerAndConstants_stub);

            // emit in R_SubmitSurfaces
            utils::hook::call(0x1400EB6EE, R_SubmitSurfaces_stub);

            // CG_ApplyHudOutlineRenderFlags in CG_GetRenderFlagForRefEntity
            CG_GetRenderFlagForRefEntity_hook.create(0x1400B4D60, CG_GetRenderFlagForRefEntity_stub);

            // G_HudOutline_FreeForEnt in G_FreeEntity
            utils::hook::call(0x1403891EB, EntHandleDissociate_stub);
            
            // G_HudOutline_FreeForClient in ClientDisconnect
            utils::hook::call(0x140327E9E, G_ClearDialogQueue_stub);
            
            // G_HudOutline_AddForNewClient in ClientSpawn
            utils::hook::nop(0x14032835A, 14);
            utils::hook::jump(0x14032835A, utils::hook::assemble(ClientSpawn_stub), true);

            // Stencil hooks
            utils::hook::jump(0x140606220, R_StencilOverrideUpdateStateBits);
            utils::hook::set<uint32_t>(0x14063B59A + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063B5F9 + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063B934 + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063B993 + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063BCDF + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063BD3F + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063C147 + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063C1B1 + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063C9FA + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063CA63 + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063D24A + 2, 0xE401E);
            utils::hook::set<uint32_t>(0x14063D2B7 + 2, 0xE401E);

            utils::hook::set<uint32_t>(0x1405C3D88 + 1, 0xE401E);
            utils::hook::set<uint32_t>(0x1405C3D7E + 6, 0xFFF1BFE1);

            // R_AddDObjToScene
            utils::hook::set<uint32_t>(0x1400EC596 + 2, 0x1E401F);
            utils::hook::set<uint32_t>(0x1400EC5E8 + 2, 0x1E401F);

            // R_AddViewmodelDObjToScene
            utils::hook::set<uint32_t>(0x1400E99F8 + 3, 0x1E401F);
            utils::hook::set<uint32_t>(0x1400E9A4B + 1, 0x1E401F);

            // objective patches since we use playerstate objective[35] as client side hud outline data
            utils::hook::set<uint32_t>(0x1403462FD + 3, 0x05ED5AC4 - sizeof(game::objective_s) * 1); // G_UpdateObjectiveToClients (compare objectives[35] instead of objectives[36])
            utils::hook::set<uint8_t>(0x14041FCBA + 2, 35); // ClientSpawn ( adds default entNum )
            utils::hook::set<uint32_t>(0x140210D3B + 2, 35); // CG_CompassDrawTickertape
            utils::hook::set<uint32_t>(0x1402102FF + 4, 35); // CG_CompassDrawPlayerPointers_MP

            // write and read hud outline data properly
            MSG_WriteDeltaPlayerstate_hook.create(0x1404289E0, MSG_WriteDeltaPlayerstate_stub);
            MSG_ReadDeltaPlayerstate_hook.create(0x140421C10, MSG_ReadDeltaPlayerstate_stub);
        }
    };
}

REGISTER_COMPONENT(hud_outline::component)
