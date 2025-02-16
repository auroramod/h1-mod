#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace map_patches
{
	enum leaf_table_version : std::int8_t
	{
		h2 = 0i8,
		h1 = 0i8,
		s1 = 0i8,
		iw6 = 1i8,
	};

	utils::hook::detour r_decode_light_grid_block_hook;
	void r_decode_light_grid_block_stub(game::GfxLightGridTree* p_tree, int child_mask,
		char child_index, int encoded_node_address, char* p_node_raw, char* p_leaf_raw)
	{
		static const auto p_address = SELECT_VALUE(0x57BCCE_b + 1, 0x6A032E_b + 1);
		auto value = *(uint8_t*)p_address;
		if (p_tree->unused[0] == leaf_table_version::iw6)
		{
			if (value != 6)
			{
				utils::hook::set<uint8_t>(p_address, 6); // iw6
			}
		}
		else if (value != 7)
		{
			utils::hook::set<uint8_t>(p_address, 7); // s1,h1,h2
		}

		r_decode_light_grid_block_hook.invoke<void>(p_tree, child_mask,
			child_index, encoded_node_address, p_node_raw, p_leaf_raw);
	}

	namespace
	{
		struct GfxLightGridRow
		{
			unsigned short colStart;
			unsigned short colCount;
			unsigned short zStart;
			unsigned short zCount;
			unsigned int firstEntry;
		};

		struct GfxLightGridRaw
		{
			unsigned int colorIndex;
			unsigned short lightIndex;
			unsigned char voxel;
		};

		struct GfxHeroLightParams
		{
			const float* heroLightSamplePos;
			const float(*viewModelAmbient)[3];
			const float(*heroAmbient)[3];
		};

		struct GfxLight
		{
			unsigned __int8 type;
			unsigned __int8 physicallyBased;
			unsigned __int8 canUseShadowMap;
			unsigned __int8 shadowState;
			char opl;
			char lightingState;
			char __pad[2];
			float color[3];
			float dir[3];
			float up[3];
			float origin[3];
			float radius;
			float fadeOffset[2];
			float bulbRadius;
			float bulbLength[3];
			float cosHalfFovOuter;
			float cosHalfFovInner;
			int exponent;
			unsigned int spotShadowIndex;
			float dynamicSpotLightNearPlaneOffset;
			float dynamicSpotLightLength;
			float cucRotationOffsetRad;
			float cucRotationSpeedRad;
			float cucScrollVector[2];
			float cucScaleVector[2];
			float cucTransVector[2];
			game::GfxLightDef* def;
		};

		struct GfxPrimaryLightsAndWeights
		{
			unsigned int primaryLightCount;
			const GfxLight* primaryLights[8];
			float primaryLightWeights[8];
		};

		struct GfxLightGridColors_IW5
		{
			unsigned char rgb[56][3];
		};

		//WEAK game::symbol<bool(const game::GfxLightGrid* lightGrid, const game::GfxLightGridEntry* entry, int cornerIndex, const unsigned int* pos, const float* samplePos)> R_IsValidLightGridSample{ 0x0, 0x6D7E40 };
		WEAK game::symbol<void(float*)> Vec3Normalize{ 0x5E090, 0x68D20 };
		WEAK game::symbol<int(int, float const* const, float const* const, struct game::Bounds const*, unsigned int, int)> SV_BrushModelSightTrace{ 0x19F830, 0x3370A0 };

		game::symbol<game::ComWorld*> comWorld{ 0xECA078, 0x10B4528 };
		game::symbol<game::GfxWorld*> s_world{ 0xF7A2BE0, 0xE973AE0 };

		game::dvar_t* r_lightGridNonCompressed;

		namespace
		{
			typedef unsigned short ushort;
			typedef unsigned int uint;

			uint as_uint(const float x) {
				return *(uint*)&x;
			}
			float as_float(const uint x) {
				return *(float*)&x;
			}

			float half_to_float(const ushort x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
				const uint e = (x & 0x7C00) >> 10; // exponent
				const uint m = (x & 0x03FF) << 13; // mantissa
				const uint v = as_uint((float)m) >> 23; // evil log2 bit hack to count leading zeros in denormalized format
				return as_float((x & 0x8000) << 16 | (e != 0) * ((e + 112) << 23 | m) | ((e == 0) & (m != 0)) * ((v - 37) << 23 | ((m << (150 - v)) & 0x007FE000))); // sign : normalized : denormalized
			}
			ushort float_to_half(const float x) { // IEEE-754 16-bit floating-point format (without infinity): 1-5-10, exp-15, +-131008.0, +-6.1035156E-5, +-5.9604645E-8, 3.311 digits
				const uint b = as_uint(x) + 0x00001000; // round-to-nearest-even: add last bit after truncated mantissa
				const uint e = (b & 0x7F800000) >> 23; // exponent
				const uint m = b & 0x007FFFFF; // mantissa; in line below: 0x007FF000 = 0x00800000-0x00001000 = decimal indicator flag - initial rounding
				return (ushort)((b & 0x80000000) >> 16 | (e > 112) * ((((e - 112) << 10) & 0x7C00) | m >> 13) | ((e < 113) & (e > 101)) * ((((0x007FF000 + m) >> (125 - e)) + 1) >> 1) | (e > 143) * 0x7FFF); // sign : normalized : denormalized : saturate
			}
		}

		void GetColors(game::GfxLightGridColors* from, GfxLightGridColors_IW5* to)
		{
			for (auto i = 0; i < 56; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					to->rgb[i][j] = static_cast<unsigned char>(half_to_float(from->rgb[i][j]) * 255.0f);
				}
			}
		}

		void GetColors(GfxLightGridColors_IW5* from, game::GfxLightGridColors* to)
		{
			for (auto i = 0; i < 56; i++)
			{
				for (auto j = 0; j < 3; j++)
				{
					to->rgb[i][j] = static_cast<unsigned short>(float_to_half(from->rgb[i][j] / 255.0f));
				}
			}
		}

		void blend_colors(game::GfxLightGridColors* colorsa, game::GfxLightGridColors* colorsb, float blendFactor = 0.5f)
		{
			for (auto i = 0; i < 56; i++)
			{
				for (auto channel = 0; channel < 3; channel++)
				{
					auto a = half_to_float(colorsa->rgb[i][channel]) * 255.0f;
					auto b = half_to_float(colorsb->rgb[i][channel]) * 255.0f;

					auto val = a * (1.0f - blendFactor) + b * blendFactor;

					colorsa->rgb[i][channel] = float_to_half(val / 255.0f);
				}
			}
		}

		bool R_IsBetterPrimaryLightEnv(unsigned short oldPrimaryLightEnvIndex, unsigned short newPrimaryLightEnvIndex, float oldWeight, float newWeight)
		{
			if (!oldPrimaryLightEnvIndex)
				return true;
			if (!newPrimaryLightEnvIndex)
				return false;

			auto comWorld_ptr = *comWorld;
			if (comWorld_ptr->primaryLightEnvs[oldPrimaryLightEnvIndex].numIndices == 1
				&& comWorld_ptr->primaryLightEnvs[oldPrimaryLightEnvIndex].primaryLightIndices[0] >= 2048
				- (*s_world)->lastSunPrimaryLightIndex)
			{
				return true;
			}
			if (comWorld_ptr->primaryLightEnvs[newPrimaryLightEnvIndex].numIndices == 1
				&& comWorld_ptr->primaryLightEnvs[newPrimaryLightEnvIndex].primaryLightIndices[0] >= 2048
				- (*s_world)->lastSunPrimaryLightIndex)
			{
				return false;
			}
			return newWeight > oldWeight;
		}

		void R_GetLightGridSampleEntryQuad(
			const game::GfxLightGrid* lightGrid,
			const unsigned int* pos,
			const game::GfxLightGridEntry** entries,
			unsigned short* defaultGridEntry)
		{
			__int64 v4; // rax
			int v7; // er10
			__int64 v8; // rcx
			unsigned short v10; // cx
			const game::GfxLightGridEntry* v11; // r8
			unsigned __int8* v12; // rdi
			unsigned int v13; // esi
			unsigned int v14; // er10
			unsigned int v15; // er15
			int v16; // ebp
			unsigned __int8* v17; // r9
			unsigned int v18; // esi
			unsigned int v19; // eax
			__int64 v20; // rdi
			const game::GfxLightGridEntry* v21; // rax
			const game::GfxLightGridEntry* v22; // rax
			unsigned int v23; // er12
			int v24; // ecx
			__int64 v25; // rax
			unsigned __int8 v26; // dl
			int v27; // ecx
			unsigned int v28; // er14
			unsigned int v29; // ecx
			unsigned int v30; // er15
			__int64 v31; // rdx
			const game::GfxLightGridEntry* v32; // rax
			const game::GfxLightGridEntry* v33; // rax
			unsigned int v34; // eax
			__int64 v35; // rdx
			bool v36; // cf
			int v37; // ebp
			unsigned __int8* v38; // r9
			int v39; // ecx
			unsigned int v40; // er10
			const game::GfxLightGridEntry* v41; // rax

			v4 = lightGrid->rowAxis;
			v7 = lightGrid->mins[v4];
			v8 = pos[v4] - v7;
			if ((unsigned int)v8 >= (unsigned int)lightGrid->maxs[v4] - v7 + 1
				|| (v10 = lightGrid->rowDataStart[v8], v10 == 0xFFFF))
			{
				*entries = 0i64;
				entries[1] = 0i64;
				entries[2] = 0i64;
				entries[3] = 0i64;
				return;
			}
			v11 = 0i64;
			v12 = &lightGrid->rawRowData[4 * v10];
			v13 = pos[lightGrid->colAxis] - *(unsigned short*)v12;
			v14 = pos[2] - *((unsigned short*)v12 + 2);
			if (v13 + 1 > *((unsigned short*)v12 + 1))
			{
				*entries = 0i64;
				entries[1] = 0i64;
				entries[2] = 0i64;
				entries[3] = 0i64;
				return;
			}
			v15 = *((unsigned short*)v12 + 3);
			if (v14 + 1 > v15)
			{
				*entries = 0i64;
				entries[1] = 0i64;
				entries[2] = 0i64;
				entries[3] = 0i64;
				if (pos[2] < *((unsigned short*)v12 + 2) && !lightGrid->useSkyForLowZ)
					*defaultGridEntry = 0;
				return;
			}
			v16 = *((int*)v12 + 2);
			v17 = v12 + 12;
			if (v13 != -1)
			{
				v23 = ((unsigned short)v15 > 0xFFu) + 3;
				if (v13 >= *v17)
				{
					do
					{
						v24 = *v17;
						v13 -= v24;
						v16 += v24 * v17[1];
						v25 = v23;
						if (!v17[1])
							v25 = 2i64;
						v17 += v25;
					} while (v13 >= *v17);
					v15 = *((short*)v12 + 3);
				}
				if (v17[1])
				{
					v29 = v17[2];
					if ((unsigned short)v15 > 0xFFu)
						v29 += v17[3] << 8;
					if (v14 < v29)
						*defaultGridEntry = 0;
					v30 = v14 - v29;
					v31 = v16 + v14 - v29 + v13 * v17[1];
					if (v14 - v29 < v17[1])
						v32 = &lightGrid->entries[v31];
					else
						v32 = 0i64;
					*entries = v32;
					if (v30 + 1 < v17[1])
						v33 = &lightGrid->entries[(unsigned int)(v31 + 1)];
					else
						v33 = 0i64;
					entries[1] = v33;
					v28 = *v17;
					if (v13 + 1 < v28)
					{
						v34 = v17[1];
						v35 = v34 + (unsigned int)v31;
						if (v30 < v34)
							entries[2] = &lightGrid->entries[v35];
						else
							entries[2] = 0i64;
						v36 = v30 + 1 < v17[1];
						goto LABEL_59;
					}
				}
				else
				{
					*entries = 0i64;
					entries[1] = 0i64;
					v26 = v17[3];
					if (v26)
					{
						v27 = v17[4];
						if (*((unsigned short*)v12 + 3) > 0xFFu)
							v27 += v17[5] << 8;
						if (v14 < v27 + (unsigned int)v26)
							*defaultGridEntry = 0;
					}
					v28 = *v17;
					if (v13 + 1 < v28)
						goto LABEL_32;
				}
				if (pos[lightGrid->colAxis] + 1u == (unsigned int)(*((unsigned short*)v12 + 1u) + *(unsigned short*)v12))
				{
				LABEL_32:
					entries[2] = 0i64;
				LABEL_33:
					entries[3] = v11;
					return;
				}
				v37 = v28 * v17[1] + v16;
				if (!v17[1])
					v23 = 2;
				v38 = &v17[v23];
				v39 = v38[2];
				if (*((unsigned short*)v12 + 3) > 0xFFu)
					v39 += v38[3] << 8;
				v40 = v14 - v39;
				v35 = v40 + v37;
				if (v40 < v38[1])
					v41 = &lightGrid->entries[v35];
				else
					v41 = 0i64;
				entries[2] = v41;
				v36 = v40 + 1 < v38[1];
			LABEL_59:
				if (v36)
					v11 = &lightGrid->entries[(unsigned int)(v35 + 1)];
				goto LABEL_33;
			}
			*entries = 0i64;
			entries[1] = 0i64;
			v18 = v12[14];
			if (*((unsigned short*)v12 + 3) > 0xFFu)
				v18 += v12[15] << 8;
			v19 = v12[13];
			v20 = v14 - v18 + v16;
			if (v14 - v18 < v19)
				v21 = &lightGrid->entries[v20];
			else
				v21 = 0i64;
			entries[2] = v21;
			if (v14 - v18 + 1 < v17[1])
				v22 = &lightGrid->entries[(unsigned int)(v20 + 1)];
			else
				v22 = 0i64;
			entries[3] = v22;
			if (v14 < v18)
				*defaultGridEntry = 0;
		}

		void fixup_entries(const game::GfxLightGrid* lightGrid, game::GfxLightGridEntry** entries)
		{
			for (auto i = 0; i < 8; i++)
			{
				auto* entry = entries[i];
				if (entry)
				{
					auto* world = (*s_world);
					if (entry->primaryLightEnvIndex > world->primaryLightEnvCount &&
						(entry->primaryLightEnvIndex != 0xFF &&
							entry->primaryLightEnvIndex != 0xFFFF))
					{
						entry->primaryLightEnvIndex = 0;
						entry->needsTrace = 0;
					}
				}
			}
		}

		BOOL R_IsValidLightGridSample(const game::GfxLightGrid* lightGrid, const game::GfxLightGridEntry* entry, int cornerIndex, const unsigned int* pos, const float* samplePos)
		{
			float traceDir[3];
			float gridPos[3];
			float nudgedGridPos[3];

			gridPos[0] = pos[0] * 32.0f + -131072.0f;
			gridPos[1] = pos[1] * 32.0f + -131072.0f;
			gridPos[2] = pos[2] * 64.0f + -131072.0f;
			gridPos[lightGrid->rowAxis] = (float)((cornerIndex & 4) != 0 ? 0x20 : 0) + gridPos[lightGrid->rowAxis];
			gridPos[lightGrid->colAxis] = (float)((cornerIndex & 2) != 0 ? 0x20 : 0) + gridPos[lightGrid->colAxis];
			gridPos[2] = (float)((cornerIndex & 1) != 0 ? 0x40 : 0) + gridPos[2];
			traceDir[0] = *samplePos - gridPos[0];
			traceDir[1] = samplePos[1] - gridPos[1];
			traceDir[2] = samplePos[2] - gridPos[2];
			Vec3Normalize(traceDir);
			nudgedGridPos[0] = (0.0099999998f * traceDir[0]) + gridPos[0];
			nudgedGridPos[1] = (0.0099999998f * traceDir[1]) + gridPos[1];
			nudgedGridPos[2] = (0.0099999998f * traceDir[2]) + gridPos[2];
			game::Bounds bounds{};
			return SV_BrushModelSightTrace(0, samplePos, nudgedGridPos, &bounds, 0, 8193) == 0;
		}

		int R_LightGridLookup_IW(const game::GfxLightGrid* lightGrid, const float* samplePos, float* cornerWeight,
			const game::GfxLightGridEntry** cornerEntryOut, unsigned short* defaultGridEntry)
		{
			// Early null checks for pointers
			if (!lightGrid || !samplePos || !cornerWeight || !cornerEntryOut || !defaultGridEntry) {
				return 0xFF;  // Invalid input data
			}

			unsigned int pos[3];
			float axisLerp[3];
			float quadWeight;
			bool isDefaultEntryUsed = false;
			float primaryLightEnvWeight = 0.0f;
			unsigned short primaryLightEnvIndex = 0;

			// Calculate position and interpolation along grid axes
			int rowIdx = static_cast<int>(floorf(samplePos[0]));
			int colIdx = static_cast<int>(floorf(samplePos[1]));
			int layerIdx = static_cast<int>(floorf(samplePos[2]));

			pos[0] = (rowIdx + 0x20000) >> 5;
			pos[1] = (colIdx + 0x20000) >> 5;
			pos[2] = (layerIdx + 0x20000) >> 6;

			axisLerp[lightGrid->rowAxis] = (samplePos[lightGrid->rowAxis] - -131072.0f) * 0.03125f - (float)pos[lightGrid->rowAxis];
			axisLerp[lightGrid->colAxis] = (samplePos[lightGrid->colAxis] - -131072.0f) * 0.03125f - (float)pos[lightGrid->colAxis];
			axisLerp[2] = (samplePos[2] - -131072.0f) * 0.015625f - (float)pos[2];

			// Check that axisLerp values are within a valid range
			for (int i = 0; i < 3; ++i) {
				if (axisLerp[i] < 0.0f || axisLerp[i] > 1.0f) {
					axisLerp[i] = std::clamp(axisLerp[i], 0.0f, 1.0f); // Clamp values between 0 and 1
				}
			}

			// Calculate corner weights based on the axis interpolation
			quadWeight = (1.0f - axisLerp[lightGrid->colAxis]) * (1.0f - axisLerp[2]);
			cornerWeight[0] = (1.0f - axisLerp[lightGrid->rowAxis]) * quadWeight;
			cornerWeight[4] = quadWeight * axisLerp[lightGrid->rowAxis];

			quadWeight = (1.0f - axisLerp[lightGrid->colAxis]) * axisLerp[2];
			cornerWeight[1] = (1.0f - axisLerp[lightGrid->rowAxis]) * quadWeight;
			cornerWeight[5] = quadWeight * axisLerp[lightGrid->rowAxis];

			quadWeight = axisLerp[lightGrid->colAxis] * (1.0f - axisLerp[2]);
			cornerWeight[2] = (1.0f - axisLerp[lightGrid->rowAxis]) * quadWeight;
			cornerWeight[6] = quadWeight * axisLerp[lightGrid->rowAxis];

			quadWeight = axisLerp[lightGrid->colAxis] * axisLerp[2];
			cornerWeight[3] = (1.0f - axisLerp[lightGrid->rowAxis]) * quadWeight;
			cornerWeight[7] = quadWeight * axisLerp[lightGrid->rowAxis];

			*defaultGridEntry = 1;

			// Sample light grid entries at the corners
			R_GetLightGridSampleEntryQuad(lightGrid, pos, cornerEntryOut, defaultGridEntry);
			++pos[lightGrid->rowAxis];
			R_GetLightGridSampleEntryQuad(lightGrid, pos, cornerEntryOut + 4, defaultGridEntry);
			--pos[lightGrid->rowAxis];

			fixup_entries(lightGrid, const_cast<game::GfxLightGridEntry**>(cornerEntryOut));

			// Evaluate light grid entries at each corner
			for (int i = 0; i < 8; ++i) {
				const game::GfxLightGridEntry* entry = *(cornerEntryOut + i);

				// Null entry check
				if (!entry || cornerWeight[i] < 0.001f) {
					cornerWeight[i] = 0.0f;
					continue;
				}

				if (entry->needsTrace && !R_IsValidLightGridSample(lightGrid, entry, i, pos, samplePos)) {
					if (isDefaultEntryUsed) {
						cornerWeight[i] = 0.0f;
						continue;
					}
				}
				else if (!isDefaultEntryUsed) {
					primaryLightEnvWeight = cornerWeight[i];
					primaryLightEnvIndex = entry->primaryLightEnvIndex;
					isDefaultEntryUsed = true;
					memset(cornerEntryOut, 0, i * sizeof(game::GfxLightGridEntry*)); // Properly reset the array
				}

				// Check if this entry is a better primary light environment
				if (R_IsBetterPrimaryLightEnv(primaryLightEnvIndex, entry->primaryLightEnvIndex, primaryLightEnvWeight, cornerWeight[i])) {
					primaryLightEnvWeight = cornerWeight[i];
					primaryLightEnvIndex = entry->primaryLightEnvIndex;
				}
			}

			return primaryLightEnvIndex;
		}

		utils::hook::detour r_lightgrid_lookup_hook;
		int r_lightgrid_lookup_stub(game::GfxLightGrid* lightGrid, float* samplePos, GfxLightGridRaw* lightGridRaw, float* cornerWeight,
			game::GfxLightGridEntry* cornerEntry, unsigned short* defaultGridEntry, float* referencePos, unsigned int smoothFetch)
		{
			auto primaryLightIndex = r_lightgrid_lookup_hook.invoke<int>(lightGrid, samplePos, lightGridRaw, cornerWeight, cornerEntry, defaultGridEntry, referencePos, smoothFetch);

			if (r_lightGridNonCompressed && r_lightGridNonCompressed->current.enabled)
			{
				if (lightGrid->entryCount)
				{
					bool effect_callback = false;
					auto address = _ReturnAddress();
					if (address == (void*)SELECT_VALUE(0x5BF0B5_b, 0x6D6EE5_b))
					{
						effect_callback = true;
						//return primaryLightIndex;
					}

					game::GfxLightGridEntry* entries[8] = { nullptr };
					primaryLightIndex = R_LightGridLookup_IW(lightGrid, samplePos, cornerWeight, (const game::GfxLightGridEntry**)entries, defaultGridEntry);
					for (auto i = 0; i < 8; i++)
					{
						if (entries[i])
						{
							memcpy(&cornerEntry[i], entries[i], sizeof(game::GfxLightGridEntry));
						}
					}

					if (primaryLightIndex == 0xFF)
					{
						primaryLightIndex = lightGrid->lastSunPrimaryLightIndex;
					}

					if (primaryLightIndex == 0xFFFF)
					{
						__debugbreak();
					}

					for (auto i = 0; i < 8; i++)
					{
						if (cornerEntry[i].primaryLightEnvIndex == 0xFF)
						{
							cornerEntry[i].primaryLightEnvIndex = static_cast<unsigned short>(lightGrid->lastSunPrimaryLightIndex);
						}

						// this is fucked idk
						if (!effect_callback)
						{
							cornerEntry[i].unused = 0xFF;
						}
					}
				}
			}

			return primaryLightIndex;
		}

		void R_AverageLightGridColors(const GfxLightGridColors_IW5* colors, float sunWeight, unsigned __int8* outColor)
		{
			int total[3] = { 0 };  // Array to store sum of RGB values
			const int numSamples = 56;

			// Sum up RGB values across all samples
			for (int i = 0; i < numSamples; ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					total[j] += colors->rgb[i][j];
				}
			}

			// Calculate the average for each color and store it in outColor
			for (int i = 0; i < 3; ++i)
			{
				outColor[i] = (unsigned __int8)(total[i] / numSamples);
			}

			// Apply sun weight to the alpha channel
			outColor[3] = (unsigned __int8)((sunWeight * 255.0f) + 0.5f);
		}

		void R_GetLightGridColorsFixedPointBlendWeights(
			const float* colorsWeight,
			unsigned int colorsCount,
			float weightNormalizeScale,
			uint16_t* fixedPointWeight,
			uint16_t* a5)
		{
			unsigned short v6; // r10
			unsigned int v7; // r3
			unsigned int v8; // r8
			int v9; // r5
			int v10; // r11
			__int64 v11; // [sp+0h] [-10h]

			v6 = 0;
			v7 = 0;
			v8 = 0;
			v9 = 0;
			v10 = 0;
			do
			{
				v11 = (__int64)((*colorsWeight * (weightNormalizeScale * 256.0f)) + 0.5f);
				a5[v10] = (unsigned short)v11;
				v6 += (unsigned short)v11;
				if (*(unsigned short*)((char*)a5 + v9) < (unsigned int)(unsigned short)v11)
				{
					v7 = v8;
					v9 = v10 * 2;
				}
				++v8;
				++colorsWeight;
				++v10;
			} while (v8 < colorsCount);
			a5[v7] = a5[v7] - v6 + 256;
		}

		void R_ScaleLightGridColors(const GfxLightGridColors_IW5* colors, unsigned short fixedPointWeight, unsigned short* scaled)
		{
			assert(colors);

			int v3; // r11
			int v4; // ctr
			unsigned short* v6; // r10
			unsigned short v7; // r4

			v3 = 0;
			v4 = 21;
			v6 = scaled - 1;
			do
			{
				v6[1] = colors->rgb[0][v3] * fixedPointWeight;
				v6[2] = colors->rgb[0][v3 + 1] * fixedPointWeight;
				v6[3] = colors->rgb[0][v3 + 2] * fixedPointWeight;
				v6[4] = colors->rgb[1][v3] * fixedPointWeight;
				v6[5] = colors->rgb[1][v3 + 1] * fixedPointWeight;
				v6[6] = colors->rgb[1][v3 + 2] * fixedPointWeight;
				v6[7] = colors->rgb[2][v3] * fixedPointWeight;
				v7 = colors->rgb[2][v3 + 1] * fixedPointWeight;
				v3 += 8;
				v6 += 8;
				*v6 = v7;
				--v4;
			} while (v4);
		}

		void R_WeightedAccumulateLightGridColors(const GfxLightGridColors_IW5* colors, unsigned short fixedPointWeight, unsigned short* accumulated)
		{
			int v3; // r10
			int v4; // ctr
			unsigned short* v5; // r11
			unsigned short v6; // r31
			unsigned short v7; // r30
			unsigned short v8; // r29
			unsigned short v9; // r28
			unsigned short v10; // r20
			unsigned short v11; // r6
			unsigned short v12; // r5
			short v13; // r8

			v3 = 0;
			v4 = 21;
			v5 = accumulated - 1;
			do
			{
				v6 = v5[4];
				v7 = v5[5];
				v8 = v5[6];
				v9 = v5[7];
				v10 = v5[8];
				v11 = v5[2];
				v12 = v5[3];
				v5[1] += colors->rgb[0][v3] * fixedPointWeight;
				v5[2] = colors->rgb[0][v3 + 1] * fixedPointWeight + v11;
				v5[3] = colors->rgb[0][v3 + 2] * fixedPointWeight + v12;
				v5[4] = colors->rgb[1][v3] * fixedPointWeight + v6;
				v5[5] = colors->rgb[1][v3 + 1] * fixedPointWeight + v7;
				v5[6] = colors->rgb[1][v3 + 2] * fixedPointWeight + v8;
				v5[7] = colors->rgb[2][v3] * fixedPointWeight + v9;
				v13 = colors->rgb[2][v3 + 1] * fixedPointWeight;
				v3 += 8;
				v5 += 8;
				*v5 = v13 + v10;
				--v4;
			} while (v4);
		}

		void R_PackAccumulatedLightGridColors(const unsigned short* accumulated, GfxLightGridColors_IW5* packed)
		{
			int v2; // r11
			int v3; // ctr
			const unsigned short* v4; // r10

			v2 = 0;
			v3 = 21;
			v4 = accumulated - 1;
			do
			{
				packed->rgb[0][v2] = (unsigned short)(v4[1] + 127) >> 8;
				packed->rgb[0][v2 + 1] = (unsigned short)(v4[2] + 127) >> 8;
				packed->rgb[0][v2 + 2] = (unsigned short)(v4[3] + 127) >> 8;
				packed->rgb[1][v2] = (unsigned short)(v4[4] + 127) >> 8;
				packed->rgb[1][v2 + 1] = (unsigned short)(v4[5] + 127) >> 8;
				packed->rgb[1][v2 + 2] = (unsigned short)(v4[6] + 127) >> 8;
				packed->rgb[2][v2] = (unsigned short)(v4[7] + 127) >> 8;
				v4 += 8;
				packed->rgb[2][v2 + 1] = (unsigned short)(*v4 + 127) >> 8;
				v2 += 8;
				--v3;
			} while (v3);
		}

		void __fastcall R_FixedPointBlendLightGridColors(const game::GfxLightGrid* lightGrid, const unsigned short* colorsIndex, const unsigned short* fixedPointWeight,
			unsigned int colorsCount, GfxLightGridColors_IW5* outPacked, const float* heroLightSamplePos, const float(*heroAmbient)[3], const float(*viewModelAmbient)[3])
		{
			const unsigned short* v16; // r31
			unsigned int v17; // r30
			__int64 v18; // r29
			unsigned short v19[216]; // [sp+50h] [-1B0h] BYREF

			GfxLightGridColors_IW5 colors{};

			GetColors(&lightGrid->colors[*colorsIndex], &colors);
			R_ScaleLightGridColors(&colors, *fixedPointWeight, v19);
			v16 = colorsIndex + 1;
			v17 = 1;
			v18 = fixedPointWeight - colorsIndex;
			do
			{
				GetColors(&lightGrid->colors[*v16], &colors);
				R_WeightedAccumulateLightGridColors(&colors, *(v16 + v18), v19);
				++v17;
				++v16;
			} while (v17 < colorsCount);
			/*if (viewModelAmbient || heroAmbient || heroLightSamplePos)
			{
				R_AddHeroOnlyLightsToGridColors(v19, heroLightSamplePos, heroAmbient, viewModelAmbient);
				if (r_lightGridEnableTweaks->current.enabled
					&& r_lightGridUseTweakedValues->current.enabled
					&& r_heroLighting->current.enabled)
				{
					R_AdjustLightColorSamples(v19);
				}
			}*/
			R_PackAccumulatedLightGridColors(v19, outPacked);
		}

		void R_BlendAndAverageLightGridColors(const game::GfxLightGrid* lightGrid, const unsigned short* colorsIndex, const float* colorsWeight, unsigned int colorsCount,
			float primaryLightWeight, float weightNormalizeScale, unsigned __int8* outAverage)
		{
			const unsigned short* v13; // r31
			unsigned int v14; // r30
			__int64 v15; // r29
			float v17; // fp9
			unsigned short v18[8]; // [sp+50h] [-250h] BYREF
			GfxLightGridColors_IW5 v19; // [sp+60h] [-240h] BYREF
			unsigned short v20[200]; // [sp+110h] [-190h] BYREF

			v17 = 1.0f;

			GfxLightGridColors_IW5 colors{};

			R_GetLightGridColorsFixedPointBlendWeights(
				colorsWeight,
				colorsCount,
				weightNormalizeScale,
				(unsigned short*)colorsWeight,
				v18);

			GetColors(&lightGrid->colors[*colorsIndex], &colors);
			R_ScaleLightGridColors(&colors, v18[0], v20);
			v13 = colorsIndex + 1;
			v14 = 1;
			v15 = (char*)v18 - (char*)colorsIndex;
			do
			{
				GetColors(&lightGrid->colors[*v13], &colors);
				R_WeightedAccumulateLightGridColors(&colors, *(const unsigned short*)((char*)v13 + v15), v20);
				++v14;
				++v13;
			} while (v14 < colorsCount);
			R_PackAccumulatedLightGridColors(v20, &v19);
			R_AverageLightGridColors(&v19, v17, outAverage);
		}

		utils::hook::detour r_get_lightgrid_average_color_hook;
		void r_get_lightgrid_average_color_stub(game::GfxLightGrid* lightGrid, unsigned int colorIndex, float* rgb)
		{
			if (r_lightGridNonCompressed && r_lightGridNonCompressed->current.enabled)
			{
				if (lightGrid->colors)
				{
					GfxLightGridColors_IW5 colors_iw5{};
					unsigned char rgba[4]{};
					GetColors(&lightGrid->colors[colorIndex], &colors_iw5);
					R_AverageLightGridColors(&colors_iw5, 1.0f, rgba);

					rgb[0] = rgba[0] / 255.0f;
					rgb[1] = rgba[1] / 255.0f;
					rgb[2] = rgba[2] / 255.0f;
					return;
				}
			}

			r_get_lightgrid_average_color_hook.invoke<void>(lightGrid, colorIndex, rgb);
		}

		utils::hook::detour r_get_lightgrid_colors_from_indices_hook;
		void r_get_lightgrid_colors_from_indices_stub(game::GfxLightGrid* lightGrid, unsigned int* colorsIndex, float* colorsWeight, int colorsCount, float primaryLightWeight,
			float weightNormalizeScale, GfxHeroLightParams* heroLightParams, game::GfxLightGridColors* colors, short* viewModelAmbient)
		{
			if (r_lightGridNonCompressed && r_lightGridNonCompressed->current.enabled)
			{
				if (lightGrid->colors)
				{
					memset(colors, 0, sizeof(game::GfxLightGridColors));
					if (colorsCount == 1)
					{
						memcpy(colors, &lightGrid->colors[*colorsIndex], sizeof(game::GfxLightGridColors));
					}
					else
					{
						memset(colors, 0, sizeof(game::GfxLightGridColors));

						unsigned short v19[8]{};
						*(unsigned __int64*)v19 = (__int64)(float)((float)((float)primaryLightWeight * 255.0f) + 0.5f);

						R_GetLightGridColorsFixedPointBlendWeights(
							colorsWeight,
							colorsCount,
							weightNormalizeScale,
							(unsigned short*)colorsWeight,
							v19);

						unsigned short colorsIndex_2[8] = {};
						for (auto i = 0; i < colorsCount; i++)
						{
							colorsIndex_2[i] = static_cast<unsigned short>(colorsIndex[i]);
						}

						GfxLightGridColors_IW5 iw5_colors{};
						R_FixedPointBlendLightGridColors(lightGrid, colorsIndex_2, v19, colorsCount, &iw5_colors, nullptr, nullptr, nullptr);

						GetColors(&iw5_colors, colors);
					}

					if (viewModelAmbient)
					{
						__debugbreak();
					}
					return;
				}
			}

			r_get_lightgrid_colors_from_indices_hook.invoke<void>(lightGrid, colorsIndex, colorsWeight, colorsCount, primaryLightWeight, weightNormalizeScale, heroLightParams, colors, viewModelAmbient);
		}

		utils::hook::detour r_get_lighting_info_for_effects_hook;
		void r_get_lighting_info_for_effects_stub(float* samplePos, char tryUseCache, float radiometricUnit, float* outColor, GfxPrimaryLightsAndWeights* outPrimaryLightsAndWeights)
		{
			if (r_lightGridNonCompressed && r_lightGridNonCompressed->current.enabled)
			{
				auto* world = (*s_world);
				if (world->lightGrid.colors)
				{
					r_get_lighting_info_for_effects_hook.invoke<void>(samplePos, 0, radiometricUnit, outColor, outPrimaryLightsAndWeights);
					return;
				}
			}

			r_get_lighting_info_for_effects_hook.invoke<void>(samplePos, tryUseCache, radiometricUnit, outColor, outPrimaryLightsAndWeights);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// skip fx name prefix checks
			if (game::environment::is_mp())
			{
				utils::hook::set<uint8_t>(0x2F377D_b, 0xEB); // createfx parse
				utils::hook::set<uint8_t>(0x4444E0_b, 0xEB); // scr_loadfx

				// patch iw6 leafTable decoding
				r_decode_light_grid_block_hook.create(0x69E7D0_b, r_decode_light_grid_block_stub);
			}

			r_lightGridNonCompressed = dvars::register_bool("r_lightGridNonCompressed", false, game::DVAR_FLAG_REPLICATED, "Use old lightgrid data, if available.");

			r_lightgrid_lookup_hook.create(SELECT_VALUE(0x5C02F0_b, 0x6D8120_b), r_lightgrid_lookup_stub);

			r_get_lightgrid_colors_from_indices_hook.create(SELECT_VALUE(0x5BE870_b, 0x6D66A0_b), r_get_lightgrid_colors_from_indices_stub);

			r_get_lightgrid_average_color_hook.create(SELECT_VALUE(0x5BE7B0_b, 0x6D65E0_b), r_get_lightgrid_average_color_stub);

			r_get_lighting_info_for_effects_hook.create(SELECT_VALUE(0x5BEF20_b, 0x6D6D50_b), r_get_lighting_info_for_effects_stub);
		}
	};
}

REGISTER_COMPONENT(map_patches::component)
