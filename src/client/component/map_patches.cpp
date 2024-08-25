#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"
#include "game/dvars.hpp"

#include <utils/hook.hpp>

namespace map_patches
{
	struct GfxLightGridTree
	{
		unsigned char index;
		unsigned char maxDepth;
		char unused[2];
		int nodeCount;
		int leafCount;
		int coordMinGridSpace[3];
		int coordMaxGridSpace[3];
		int coordHalfSizeGridSpace[3];
		int defaultColorIndexBitCount;
		int defaultLightIndexBitCount;
		unsigned int* p_nodeTable;
		int leafTableSize;
		unsigned char* p_leafTable;
	};

	enum leaf_table_version : std::int8_t
	{
		h2 = 0i8,
		h1 = 0i8,
		s1 = 0i8,
		iw6 = 1i8,
	};

	utils::hook::detour r_decode_light_grid_block_hook;
	void r_decode_light_grid_block_stub(GfxLightGridTree* p_tree, int child_mask,
		char child_index, int encoded_node_address, char* p_node_raw, char* p_leaf_raw)
	{
		static const auto p_address = 0x6A032E_b + 1;
		if (p_tree->unused[0] == leaf_table_version::iw6 && *(uint8_t*)p_address != 6)
		{
			utils::hook::set<uint8_t>(p_address, 6); // iw6
		}
		else if (*(uint8_t*)p_address != 7)
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

		//WEAK game::symbol<bool(const game::GfxLightGrid* lightGrid, const game::GfxLightGridEntry* entry, int cornerIndex, const unsigned int* pos, const float* samplePos)> R_IsValidLightGridSample{ 0x0, 0x6D7E40 };
		WEAK game::symbol<void(float*)> Vec3Normalize{0x0, 0x68D20};
		WEAK game::symbol<int(int, float const* const, float const* const, struct game::Bounds const*, unsigned int, int)> SV_BrushModelSightTrace{0x0, 0x3370A0};

		game::symbol<game::ComWorld> comWorld{0x0, 0xA97C0E0};
		game::symbol<game::GfxWorld*> s_world{0x0, 0xE973AE0};

		bool R_IsBetterPrimaryLightEnv(unsigned short oldPrimaryLightEnvIndex, unsigned short newPrimaryLightEnvIndex, float oldWeight, float newWeight)
		{
			if (!oldPrimaryLightEnvIndex)
				return true;
			if (!newPrimaryLightEnvIndex)
				return false;
			if (comWorld->primaryLightEnvs[oldPrimaryLightEnvIndex].numIndices == 1
				&& comWorld->primaryLightEnvs[oldPrimaryLightEnvIndex].primaryLightIndices[0] >= 2048
				- (*s_world)->lastSunPrimaryLightIndex)
			{
				return true;
			}
			if (comWorld->primaryLightEnvs[newPrimaryLightEnvIndex].numIndices == 1
				&& comWorld->primaryLightEnvs[newPrimaryLightEnvIndex].primaryLightIndices[0] >= 2048
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
			unsigned int* defaultGridEntry)
		{
			const game::GfxLightGridEntry* v4;
			const game::GfxLightGridEntry* v5;
			int v6;
			const game::GfxLightGridEntry* v7;
			const game::GfxLightGridEntry* v8;
			const game::GfxLightGridEntry* v9;
			const game::GfxLightGridEntry* v10;
			int v11;
			const game::GfxLightGridEntry* v12;
			const game::GfxLightGridEntry* v13;
			unsigned __int16 v14;
			unsigned int lookup;
			unsigned int lookupa;
			unsigned int lookupb;
			unsigned int lookupc;
			unsigned int firstBlockEntry;
			unsigned int firstBlockEntrya;
			unsigned int z;
			unsigned int localZ;
			const unsigned __int8* rleData;
			const unsigned __int8* rleDataa;
			const GfxLightGridRow* remoteRow;
			unsigned int rleSizeFull;
			unsigned int colIndex;
			const unsigned __int8* remote_rleData;
			unsigned int rowIndex;
			unsigned int baseZ;
			unsigned int baseZa;
			unsigned int baseZb;
			unsigned int baseZc;

			rowIndex = pos[lightGrid->rowAxis] - lightGrid->mins[lightGrid->rowAxis];
			if (rowIndex >= lightGrid->maxs[lightGrid->rowAxis] + 1 - (unsigned int)lightGrid->mins[lightGrid->rowAxis])
				v14 = 0xFFFF;
			else
				v14 = lightGrid->rowDataStart[rowIndex];
			if (v14 == 0xFFFF)
			{
				*entries = 0;
				entries[1] = 0;
				entries[2] = 0;
				entries[3] = 0;
				return;
			}
			if (4 * (unsigned int)v14 >= lightGrid->rawRowDataSize)
			{
				__debugbreak();
			}
			remoteRow = (const GfxLightGridRow*)&lightGrid->rawRowData[4 * v14];
			if (remoteRow->firstEntry >= lightGrid->entryCount)
			{
				__debugbreak();
			}
			colIndex = pos[lightGrid->colAxis] - remoteRow->colStart;
			z = pos[2] - remoteRow->zStart;
			if (colIndex + 1 > remoteRow->colCount)
			{
				*entries = 0;
				entries[1] = 0;
				entries[2] = 0;
				entries[3] = 0;
				return;
			}
			if (z + 1 > remoteRow->zCount)
			{
				*entries = 0;
				entries[1] = 0;
				entries[2] = 0;
				entries[3] = 0;
				if (pos[2] < remoteRow->zStart)
					*defaultGridEntry = 0;
				return;
			}
			firstBlockEntry = remoteRow->firstEntry;
			remote_rleData = (const unsigned __int8*)&remoteRow[1];
			rleSizeFull = (remoteRow->zCount > 0xFFu) + 3;
			rleData = (const unsigned __int8*)&remoteRow[1];
			if (colIndex == -1)
			{
				*entries = 0;
				entries[1] = 0;
				baseZ = LOBYTE(remoteRow[1].colCount);
				if (remoteRow->zCount > 0xFFu)
					baseZ += HIBYTE(remoteRow[1].colCount) << 8;
				lookup = z - baseZ + firstBlockEntry;
				if (z - baseZ < HIBYTE(remoteRow[1].colStart))
					v13 = &lightGrid->entries[lookup];
				else
					v13 = 0;
				entries[2] = v13;
				if (z - baseZ + 1 < HIBYTE(remoteRow[1].colStart))
					v12 = &lightGrid->entries[lookup + 1];
				else
					v12 = 0;
				entries[3] = v12;
				if (z < baseZ)
					*defaultGridEntry = 0;
			}
			else
			{
				while (colIndex >= *rleData)
				{
					colIndex -= *rleData;
					firstBlockEntry += rleData[1] * *rleData;
					if (rleData[1])
						v11 = (remoteRow->zCount > 0xFFu) + 3;
					else
						v11 = 2;
					remote_rleData += v11;
					rleData = remote_rleData;
				}
				if (rleData[1])
				{
					baseZb = rleData[2];
					if (remoteRow->zCount > 0xFFu)
						baseZb += rleData[3] << 8;
					if (z < baseZb)
						*defaultGridEntry = 0;
					localZ = z - baseZb;
					lookupa = z - baseZb + firstBlockEntry + colIndex * rleData[1];
					if (z - baseZb < rleData[1])
						v10 = &lightGrid->entries[lookupa];
					else
						v10 = 0;
					*entries = v10;
					if (localZ + 1 < rleData[1])
						v9 = &lightGrid->entries[lookupa + 1];
					else
						v9 = 0;
					entries[1] = v9;
					if (colIndex + 1 < *rleData)
					{
						lookupb = lookupa + rleData[1];
						if (localZ < rleData[1])
							v8 = &lightGrid->entries[lookupb];
						else
							v8 = 0;
						entries[2] = v8;
						if (localZ + 1 < rleData[1])
							v7 = &lightGrid->entries[lookupb + 1];
						else
							v7 = 0;
						entries[3] = v7;
						return;
					}
				}
				else
				{
					*entries = 0;
					entries[1] = 0;
					if (rleData[3])
					{
						baseZa = rleData[4];
						if (remoteRow->zCount > 0xFFu)
							baseZa += rleData[5] << 8;
						if (z < baseZa + rleData[3])
							*defaultGridEntry = 0;
					}
					if (colIndex + 1 < *rleData)
					{
						entries[2] = 0;
						entries[3] = 0;
						return;
					}
				}
				if (pos[lightGrid->colAxis] + 1u == (unsigned int)remoteRow->colCount + remoteRow->colStart)
				{
					entries[2] = 0;
					entries[3] = 0;
				}
				else
				{
					firstBlockEntrya = firstBlockEntry + rleData[1] * *rleData;
					if (rleData[1])
						v6 = rleSizeFull;
					else
						v6 = 2;
					rleDataa = &rleData[v6];
					baseZc = rleDataa[2];
					if (remoteRow->zCount > 0xFFu)
						baseZc += rleDataa[3] << 8;
					lookupc = z - baseZc + firstBlockEntrya;
					if (z - baseZc < rleDataa[1])
						v5 = &lightGrid->entries[lookupc];
					else
						v5 = 0;
					entries[2] = v5;
					if (z - baseZc + 1 < rleDataa[1])
						v4 = &lightGrid->entries[lookupc + 1];
					else
						v4 = 0;
					entries[3] = v4;
				}
			}
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
			game::Bounds bounds;
			return SV_BrushModelSightTrace(0, samplePos, nudgedGridPos, &bounds, 0, 8193) == 0;
		}

		int R_LightGridLookup_NonCompressed(const game::GfxLightGrid* lightGrid, const float* samplePos,
			float* cornerWeight, game::GfxLightGridEntry* cornerEntryOut, unsigned int* defaultGridEntry) {

			game::GfxLightGridEntry* entries[8];

			unsigned int pos[3];
			float axisLerp[3];
			float quadWeight;

			// Compute grid position from sample position
			int rowIdx = static_cast<int>(floorf(samplePos[0]));
			int colIdx = static_cast<int>(floorf(samplePos[1]));
			int layerIdx = static_cast<int>(floorf(samplePos[2]));

			pos[0] = (rowIdx + 0x20000) >> 5;
			pos[1] = (colIdx + 0x20000) >> 5;
			pos[2] = (layerIdx + 0x20000) >> 6;

			axisLerp[0] = (samplePos[lightGrid->rowAxis] - -131072.0f) * 0.03125f - (float)pos[lightGrid->rowAxis];
			axisLerp[1] = (samplePos[lightGrid->colAxis] - -131072.0f) * 0.03125f - (float)pos[lightGrid->colAxis];
			axisLerp[2] = (samplePos[2] - -131072.0f) * 0.015625f - (float)pos[2];
			quadWeight = (1.0f - axisLerp[1]) * (1.0f - axisLerp[2]);
			cornerWeight[0] = (1.0f - axisLerp[0]) * quadWeight;
			cornerWeight[4] = quadWeight * axisLerp[0];
			quadWeight = (1.0f - axisLerp[1]) * axisLerp[2];
			cornerWeight[1] = (1.0f - axisLerp[0]) * quadWeight;
			cornerWeight[5] = quadWeight * axisLerp[0];
			quadWeight = (1.0f - axisLerp[2]) * axisLerp[1];
			cornerWeight[2] = (1.0f - axisLerp[0]) * quadWeight;
			cornerWeight[6] = quadWeight * axisLerp[0];
			quadWeight = axisLerp[1] * axisLerp[2];
			cornerWeight[3] = (1.0f - axisLerp[0]) * (axisLerp[1] * axisLerp[2]);
			cornerWeight[7] = quadWeight * axisLerp[0];

			*defaultGridEntry = 1;

			// Sample light grid entries
			R_GetLightGridSampleEntryQuad(lightGrid, pos, (const game::GfxLightGridEntry**)&entries[0], defaultGridEntry);
			++pos[lightGrid->rowAxis];
			R_GetLightGridSampleEntryQuad(lightGrid, pos, (const game::GfxLightGridEntry**)&entries[4], defaultGridEntry);
			--pos[lightGrid->rowAxis];

			fixup_entries(lightGrid, entries);

			char v24 = 1;
			game::GfxLightGridEntry* v25 = cornerEntryOut;
			__int64 v41 = 0i64;
			float v26 = 0.0;
			unsigned short v27 = 0;
			char defaultGridEntrya = 0;
			char* v28 = (char*)((char*)entries - (char*)cornerEntryOut);
			__int64* v40 = &v41;
			unsigned int v29 = 0;
			size_t v30 = 0i64;

			game::GfxLightGridEntry* v31;
			bool v32;
			unsigned short v33;
			float v34;

			do
			{
				v31 = *(game::GfxLightGridEntry**)((char*)v25 + (unsigned __int64)v28);
				if (!v31)
				{
					v25->colorsIndex = 0;
					*cornerWeight = 0.0;
					goto LABEL_19;
				}
				*v25 = *v31;
				if (*cornerWeight < 0.001)
				{
					v25->colorsIndex = 0;
					memset((game::GfxLightGridEntry*)((char*)v25 + (unsigned __int64)v28), 0, sizeof(game::GfxLightGridEntry));
					*cornerWeight = 0.0;
					goto LABEL_19;
				}
				v32 = ((unsigned __int8)v24 & v31->needsTrace) != 0 && !R_IsValidLightGridSample(lightGrid, v31, v29, pos, samplePos);
				*(char*)v40 = v32;
				if (v32)
				{
					if (defaultGridEntrya)
					{
						v25->colorsIndex = 0;
						*cornerWeight = 0.0;
						memset((game::GfxLightGridEntry*)((char*)v25 + (unsigned __int64)v28), 0, sizeof(game::GfxLightGridEntry));
						goto LABEL_19;
					}
				}
				else if (!defaultGridEntrya)
				{
					v26 = *cornerWeight;
					v27 = v31->primaryLightEnvIndex;
					defaultGridEntrya = 1;
					memset(cornerEntryOut, 0, v30);
					memset(entries, 0, v30);
					goto LABEL_19;
				}
				v33 = v31->primaryLightEnvIndex;
				v34 = *cornerWeight;
				if (R_IsBetterPrimaryLightEnv(v27, v33, v26, *cornerWeight))
				{
					v26 = v34;
					v27 = v33;
				}
			LABEL_19:
				v40 = (__int64*)((char*)v40 + 1);
				++v29;
				v30 += 8i64;
				++v25;
				++cornerWeight;
				v24 *= 2;
			} while (v29 < 8);
			/*v35 = 1;
			if (frontEndDataOut && r_showLightGrid->current.integer == 1)
			{
				v36 = &v41;
				v37 = entries;
				v38 = 8i64;
				do
				{
					v35 = __ROL4__(v35, 1);
					v36 = (__int64*)((char*)v36 + 1);
					++v37;
					--v38;
				} while (v38);
			}*/
			return v27;
		}

		game::dvar_t* r_lightGridNonCompressed;

		utils::hook::detour r_lightgrid_lookup_hook;
		int r_lightgrid_lookup_stub(game::GfxLightGrid* lightGrid, float* samplePos, GfxLightGridRaw* lightGridRaw, float* cornerWeight,
			game::GfxLightGridEntry* cornerEntry, unsigned int* defaultGridEntry, int a7, unsigned int a8)
		{
			auto primaryLightIndex = r_lightgrid_lookup_hook.invoke<int>(lightGrid, samplePos, lightGridRaw, cornerWeight, cornerEntry, defaultGridEntry, a7, a8);

			auto address = _ReturnAddress();
			if (address == (void*)0x6D6EE5_b)
			{
				return primaryLightIndex;
			}

			if (r_lightGridNonCompressed && r_lightGridNonCompressed->current.enabled)
			{
				if (lightGrid->entryCount)
				{
					primaryLightIndex = R_LightGridLookup_NonCompressed(lightGrid, samplePos, cornerWeight, cornerEntry, defaultGridEntry);

					if (primaryLightIndex == 0xFF || primaryLightIndex == 0xFFFF) // iw3 max is 0xFF, iw4/iw5 max is 0xFFFF
					{
						primaryLightIndex = lightGrid->lastSunPrimaryLightIndex;
					}
				}
			}

			return primaryLightIndex;
		}

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

		struct GfxLightGridColors_IW4
		{
			unsigned char rgb[56][3];
		};

		utils::hook::detour sub_6D66A0_hook;
		void sub_6D66A0_stub(game::GfxLightGrid* lightGrid, unsigned int* colorsIndex, float* colorsWeight, int colorsCount, float a5, int a6, unsigned int* a7, game::GfxLightGridColors* colors, __int16* viewModelAmbient)
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
						for (auto i = 0; i < colorsCount; i++)
						{
							if (i == 0)
							{
								memcpy(colors, &lightGrid->colors[colorsIndex[i]], sizeof(game::GfxLightGridColors));
							}
							else
							{
								blend_colors(colors, &lightGrid->colors[colorsIndex[i]]);
							}
						}
					}

					if (viewModelAmbient)
					{
						__debugbreak();//
					}
					return;
				}
			}

			sub_6D66A0_hook.invoke<void>(lightGrid, colorsIndex, colorsWeight, colorsCount, a5, a6, a7, colors, viewModelAmbient);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			if (game::environment::is_sp())
			{
				return;
			}

			// skip fx name prefix checks
			utils::hook::set<uint8_t>(0x2F377D_b, 0xEB); // createfx parse
			utils::hook::set<uint8_t>(0x4444E0_b, 0xEB); // scr_loadfx

			// patch iw6 leafTable decoding
			r_decode_light_grid_block_hook.create(0x69E7D0_b, r_decode_light_grid_block_stub);

			r_lightGridNonCompressed = dvars::register_bool("r_lightGridNonCompressed", false, game::DVAR_FLAG_REPLICATED, "Use old lightgrid data, if available.");

			r_lightgrid_lookup_hook.create(0x6D8120_b, r_lightgrid_lookup_stub);

			sub_6D66A0_hook.create(0x6D66A0_b, sub_6D66A0_stub);
		}
	};
}

REGISTER_COMPONENT(map_patches::component)
