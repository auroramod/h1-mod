#pragma once

#include "fastfiles_image_sites.hpp"

namespace sorted_material_sites
{
	using image_pool_sites::reloc;

	struct constant
	{
		std::uint64_t address;
		std::uint8_t offset;
		std::uint8_t size;
		std::uint64_t old_value;
		std::uint64_t new_value;
	};

	inline constexpr reloc relocs[] =
	{
		{0x1400EA82C, 3, 0, 0x1400EA833}, // lea rsi, word_14FD6AC00
		{0x1400EAAC5, 4, 2, 0x1524CC280}, // movzx eax, byte ptr [rax+rdx+100h]
		{0x1400EAB23, 4, 2, 0x1524CC280}, // movzx eax, byte ptr [rax+rdx+100h]
		{0x1402C72EB, 5, 1, 0x140000000}, // movzx r9d, rva word_14E077D00[r14+rax*4]
		{0x1402C7342, 5, 1, 0x140000000}, // movzx edx, rva word_14E077D00[r13+rcx*2]
		{0x1402C7353, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r13+rdx*4]
		{0x1402C7365, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r13+rdx*4], xmm0
		{0x1402C736F, 5, 1, 0x140000000}, // movzx ecx, rva word_14E077D00[r13+rax*2]
		{0x1402C737C, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r13+rcx*4]
		{0x1402C7395, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r13+rcx*4], xmm0
		{0x1402C73A9, 5, 1, 0x140000000}, // movzx edx, rva word_14E077D00[r13+rcx*2]
		{0x1402C73C2, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r13+rdx*4]
		{0x1402C73D4, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r13+rdx*4], xmm0
		{0x1402C73E2, 5, 1, 0x140000000}, // movzx edx, rva word_14E077D00[r13+rcx*2]
		{0x1402C73EF, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r13+rdx*4]
		{0x1402C7404, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r13+rdx*4], xmm0
		{0x1402C7442, 5, 1, 0x140000000}, // movzx ecx, rva word_14E077D00[r14+rax*2]
		{0x1402C744F, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r14+rcx*4]
		{0x1402C7468, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r14+rcx*4], xmm0
		{0x1402C7563, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r14+rax*4]
		{0x1402C7571, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r14+rax*4], xmm0
		{0x1402C7598, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r14+rax*4]
		{0x1402C75A6, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r14+rax*4], xmm0
		{0x1402C76BA, 3, 0, 0x1402C76C1}, // lea rax, dword_14E06D700
		{0x1402C7D09, 4, 1, 0x140000000}, // mov rva dword_14E06D700[r11+r10*4], edi
		{0x1402C8FC0, 4, 1, 0x140000000}, // mov rva dword_14E06D700[r12+r9*4], 43FE0000h
		{0x1402C94D6, 6, 1, 0x140000000}, // movss xmm0, rva dword_14E06D700[r11+r9*4]
		{0x1402C94E0, 6, 1, 0x140000000}, // minss xmm0, rva dword_14E06D700[r11+r8*4]
		{0x1402C94EA, 6, 1, 0x140000000}, // movss rva dword_14E06D700[r11+r9*4], xmm0
		{0x1402CB546, 3, 0, 0x1402CB54D}, // lea r9, word_14E077D00
		{0x1402CB550, 3, 0, 0x1402CB557}, // lea rdx, dword_14E06D700
		{0x1402CB561, 3, 0, 0x1402CB568}, // lea r9, word_14E077D00
		{0x1402CB56B, 3, 0, 0x1402CB572}, // lea r8, dword_14E06D700
		{0x1402CB583, 3, 0, 0x1402CB58A}, // lea rdx, dword_14E06D700
		{0x1402CB592, 3, 0, 0x1402CB599}, // lea r9, dword_14E06D700
		{0x1402CB619, 4, 1, 0x140000000}, // mov rva dword_14E06D700[r10+r9*4], eax
		{0x14059EB5A, 3, 0, 0x14059EB61}, // lea rcx, word_14FD6AC00
		{0x14059F3C9, 4, 1, 0x140000000}, // mov eax, rva dword_14FE70FF0[r10+rdx*4]
		{0x14059F3E1, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r10+r8*2]
		{0x14059FC49, 4, 1, 0x140000000}, // mov eax, rva dword_14FE70FF0[r10+rdx*4]
		{0x14059FC61, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r10+r8*2]
		{0x1405A04B8, 3, 0, 0x1405A04BF}, // lea rcx, word_14FD6AC00
		{0x1405A0CE7, 4, 1, 0x140000000}, // mov eax, rva dword_14FE70FF0[r9+rdx*4]
		{0x1405A0CFF, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r9+r8*2]
		{0x1405A1527, 4, 1, 0x140000000}, // mov eax, rva dword_14FE70FF0[r9+rdx*4]
		{0x1405A153F, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r9+r8*2]
		{0x1405A1D42, 3, 0, 0x1405A1D49}, // lea rcx, word_14FD6AC00
		{0x1405A2588, 4, 1, 0x140000000}, // mov eax, rva dword_14FE70FF0[r9+rdx*4]
		{0x1405A25A0, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r9+r8*2]
		{0x1405A2DC8, 4, 1, 0x140000000}, // mov eax, rva dword_14FE70FF0[r9+rdx*4]
		{0x1405A2DE0, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r9+r8*2]
		{0x1405A4CB6, 3, 0, 0x1405A4CBD}, // lea r14, dword_14E06D700
		{0x1405A4CDD, 5, 2, 0x14E06D700}, // movzx r10d, word ptr [r14+rax*4+0A600h]
		{0x1405A4D32, 5, 2, 0x14E06D700}, // movzx edx, word ptr [r14+rcx*2+0A600h]
		{0x1405A4D57, 5, 2, 0x14E06D700}, // movzx ecx, word ptr [r14+rax*2+0A600h]
		{0x1405A4D89, 5, 2, 0x14E06D700}, // movzx edx, word ptr [r14+rcx*2+0A600h]
		{0x1405A4DBA, 5, 2, 0x14E06D700}, // movzx edx, word ptr [r14+rcx*2+0A600h]
		{0x1405A4E1A, 5, 2, 0x14E06D700}, // movzx ecx, word ptr [r14+rax*2+0A600h]
		{0x1405B1705, 3, 0, 0x1405B170C}, // lea rcx, word_14FD6AC00
		{0x1405B1714, 5, 2, 0x14FD6AC00}, // movzx r11d, word ptr [rcx+rax*2+5300h]
		{0x1405B1721, 4, 2, 0x14FD6AC00}, // movzx esi, word ptr [rcx+rax*2+5300h]
		{0x1405B1B2D, 3, 0, 0x1405B1B34}, // lea rcx, word_14FD6AC00
		{0x1405B1B3B, 5, 2, 0x14FD6AC00}, // movzx r11d, word ptr [rcx+rax*2+5300h]
		{0x1405B1B48, 5, 2, 0x14FD6AC00}, // movzx r10d, word ptr [rcx+rax*2+5300h]
		{0x1405C387E, 5, 1, 0x140000000}, // movzx ecx, rva word_14FD6AC00[r14+rax*2]
		{0x1405E8EED, 3, 0, 0x1405E8EF4}, // lea rcx, word_14FD6AC00
		{0x1405E8EFB, 5, 2, 0x14FD6AC00}, // movzx r11d, word ptr [rcx+rax*2+5300h]
		{0x1405E8F08, 5, 2, 0x14FD6AC00}, // movzx r10d, word ptr [rcx+rax*2+5300h]
		{0x1405EA076, 3, 0, 0x1405EA07D}, // lea rsi, dword_14FE70FF0
		{0x1405EA0AE, 3, 0, 0x1405EA0B5}, // lea rbp, word_14FD6AC00
		{0x1405EC57F, 3, 0, 0x1405EC586}, // lea rbp, word_14FD6AC00
		{0x1405EC5E7, 4, 2, 0x14FD6AC00}, // mov [rbp+rax*2+5300h], cx
		{0x1405EC63E, 3, 0, 0x1405EC645}, // lea rsi, word_14FD6AC00
		{0x1405EC648, 3, 0, 0x1405EC64F}, // lea rdx, unk_1524CC380
		{0x1405EC8BC, 3, 0, 0x1405EC8C3}, // lea rsi, dword_14E06D700
		{0x1405EC8C7, 4, 2, 0x14E06D700}, // mov [rsi+rax*4+0A600h], cx
		{0x1405EC8D7, 3, 2, 0x14E06D700}, // add rsi, 0A602h
		{0x1405EC913, 4, 2, 0x14E06D700}, // mov [rdi+rsi+0A600h], ax
		{0x1405EC920, 4, 2, 0x14E06D700}, // mov [rdi+rsi+0A602h], ax
		{0x1405EC96F, 3, 0, 0x1405EC976}, // lea rdx, word_14FD6AC00
		{0x140606F4E, 3, 0, 0x140606F55}, // lea rcx, word_14FD6AC00
		{0x140606F5C, 5, 2, 0x14FD6AC00}, // movzx r10d, word ptr [rcx+rax*2+5300h]
		{0x140606F69, 5, 2, 0x14FD6AC00}, // movzx r11d, word ptr [rcx+rax*2+5300h]
		{0x140615DC7, 3, 0, 0x140615DCE}, // lea rdx, unk_1524CC380
		{0x140615FD7, 3, 0, 0x140615FDE}, // lea rdx, unk_1524CC380
		{0x1406160F7, 3, 0, 0x1406160FE}, // lea rdx, unk_1524CC380
		{0x140616230, 3, 0, 0x140616237}, // lea rdx, unk_1524CC380
		{0x140616360, 3, 0, 0x140616367}, // lea rdx, unk_1524CC380
		{0x140617242, 3, 0, 0x140617249}, // lea rax, unk_1524CC380
		{0x1406172D6, 3, 0, 0x1406172DD}, // lea rax, unk_1524CC380
		{0x140617416, 3, 0, 0x14061741D}, // lea rax, unk_1524CC380
		{0x1406174B7, 3, 0, 0x1406174BE}, // lea rax, unk_1524CC380
		{0x140617556, 3, 0, 0x14061755D}, // lea rax, unk_1524CC380
		{0x1406175E9, 3, 0, 0x1406175F0}, // lea rax, unk_1524CC380
		{0x140617686, 3, 0, 0x14061768D}, // lea rax, unk_1524CC380
		{0x140617726, 3, 0, 0x14061772D}, // lea rax, unk_1524CC380
		{0x1406177C2, 3, 0, 0x1406177C9}, // lea rax, unk_1524CC380
		{0x140617C46, 3, 0, 0x140617C4D}, // lea rax, unk_1524CC380
		{0x140617CD9, 3, 0, 0x140617CE0}, // lea rax, unk_1524CC380
		{0x140617D76, 3, 0, 0x140617D7D}, // lea rax, unk_1524CC380
		{0x140617E16, 3, 0, 0x140617E1D}, // lea rax, unk_1524CC380
		{0x140617EFC, 3, 0, 0x140617F03}, // lea rdx, unk_1524CC380
		{0x140618009, 3, 0, 0x140618010}, // lea rdx, unk_1524CC380
		{0x1406180CE, 3, 0, 0x1406180D5}, // lea rdx, unk_1524CC380
		{0x140618167, 3, 0, 0x14061816E}, // lea rdx, unk_1524CC380
		{0x140618277, 3, 0, 0x14061827E}, // lea rdx, unk_1524CC380
		{0x140618317, 3, 0, 0x14061831E}, // lea rdx, unk_1524CC380
		{0x1406183D2, 3, 0, 0x1406183D9}, // lea rdx, unk_1524CC380
		{0x140618482, 3, 0, 0x140618489}, // lea rdx, unk_1524CC380
		{0x140618535, 3, 0, 0x14061853C}, // lea rdx, unk_1524CC380
		{0x1406185E5, 3, 0, 0x1406185EC}, // lea rdx, unk_1524CC380
		{0x140618677, 3, 0, 0x14061867E}, // lea rdx, unk_1524CC380
		{0x140618DEA, 3, 0, 0x140618DF1}, // lea rax, unk_1524CC380
		{0x140618E0A, 3, 0, 0x140618E11}, // lea rax, unk_1524CC380
		{0x140618E4A, 3, 0, 0x140618E51}, // lea rax, unk_1524CC380
		{0x140618E6B, 3, 0, 0x140618E72}, // lea rax, unk_1524CC380
		{0x140618E97, 3, 0, 0x140618E9E}, // lea rax, unk_1524CC380
		{0x140618EB7, 3, 0, 0x140618EBE}, // lea rax, unk_1524CC380
		{0x140618ED7, 3, 0, 0x140618EDE}, // lea rax, unk_1524CC380
		{0x140618EF7, 3, 0, 0x140618EFE}, // lea rax, unk_1524CC380
		{0x140618F1A, 3, 0, 0x140618F21}, // lea rax, unk_1524CC380
		{0x140618F3A, 3, 0, 0x140618F41}, // lea rax, unk_1524CC380
		{0x140618F5A, 3, 0, 0x140618F61}, // lea rax, unk_1524CC380
		{0x1406259EE, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625A74, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625AFA, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625B80, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625D2B, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625DB2, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625E39, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
		{0x140625EC4, 5, 2, 0x1524CC280}, // movzx eax, byte ptr [rcx+r13+100h]
	};

	inline constexpr constant constants[] =
	{
		{0x140094CE3, 1, 4, 0x2980ull, 0x4480ull}, // mov ebp, 2980h
		{0x140094D1B, 3, 4, 0x1FFFull, 0x3FFFull}, // and r9d, 1FFFh
		{0x140094DE4, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1400E88F6, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1400EA476, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1400EA868, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1400EA956, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rcx, 7FFC00000000h
		{0x1400EA9CA, 2, 8, 0x7FFC000000ull, 0xFFFC000000ull}, // mov rcx, 7FFC000000h
		{0x1400EAB1E, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1400EADE9, 2, 8, 0x8003FFFFFFFFFFFFull, 0x3FFFFFFFFFFFFull}, // mov rax, 8003FFFFFFFFFFFFh
		{0x1400EAE12, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rcx, 7FFC000000000000h
		{0x1402C72DE, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1402C99C0, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1402C9C3D, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x14059DB66, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059DCED, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059E287, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059E437, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059E9F7, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059EB61, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x14059EC2D, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059F23B, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059F3B9, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x14059F3DA, 3, 4, 0x1FFFull, 0x3FFFull}, // and r8d, 1FFFh
		{0x14059F4AF, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059FABB, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x14059FC39, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x14059FC5A, 3, 4, 0x1FFFull, 0x3FFFull}, // and r8d, 1FFFh
		{0x14059FD2F, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A034E, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A04BF, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A055E, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A0B5E, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A0CD7, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A0CF8, 3, 4, 0x1FFFull, 0x3FFFull}, // and r8d, 1FFFh
		{0x1405A0D9E, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A139E, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A1517, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A1538, 3, 4, 0x1FFFull, 0x3FFFull}, // and r8d, 1FFFh
		{0x1405A15DE, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A1BFF, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A1D55, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A1DFD, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A2405, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A2578, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A2599, 3, 4, 0x1FFFull, 0x3FFFull}, // and r8d, 1FFFh
		{0x1405A2649, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A2C45, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A2DB8, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A2DD9, 3, 4, 0x1FFFull, 0x3FFFull}, // and r8d, 1FFFh
		{0x1405A2E89, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405A4CCE, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405A580A, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rsi, 7FFC000000000000h
		{0x1405A5C5C, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rsi, 7FFC000000000000h
		{0x1405A6AA2, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r10, 78000000000h
		{0x1405A6ABD, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A6B20, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A6B7D, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A6BF5, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A6C5D, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A6CC5, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A6E7B, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A7367, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r10, 78000000000h
		{0x1405A73B4, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A742D, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A75A8, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A766F, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A7737, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A7C1D, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r9, 78000000000h
		{0x1405A7C3D, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A7D7C, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A8156, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r9, 78000000000h
		{0x1405A8170, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A82D6, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A83CA, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A87F3, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r9, 78000000000h
		{0x1405A881D, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A88FB, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r9, 78000000000h
		{0x1405A8912, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A8B69, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A8C39, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A8D02, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A90C6, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov r9, 78000000000h
		{0x1405A90FD, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405A9564, 2, 8, 0x7FFC000000ull, 0xFFFC000000ull}, // mov r8, 7FFC000000h
		{0x1405A9588, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A9593, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A95C3, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A95DE, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A9683, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A96A2, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A96B5, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405A9AC2, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405A9C1D, 3, 1, 0x2Bull, 0x2Cull}, // shl rsi, 2Bh
		{0x1405A9DE9, 3, 4, 0x1FFFull, 0x3FFFull}, // and r15d, 1FFFh
		{0x1405A9DF4, 2, 4, 0x1FFFull, 0x3FFFull}, // and ebx, 1FFFh
		{0x1405A9E1B, 2, 8, 0x7FFF80000000000ull, 0xFFFF00000000000ull}, // mov rcx, 7FFF80000000000h
		{0x1405A9E2A, 2, 8, 0xF80007FFFFFFFFFFull, 0xF0000FFFFFFFFFFFull}, // mov rcx, 0F80007FFFFFFFFFFh
		{0x1405A9ED0, 2, 8, 0x80000000000ull, 0x100000000000ull}, // mov r8, 80000000000h
		{0x1405A9F7D, 2, 8, 0x80000000000ull, 0x100000000000ull}, // mov rax, 80000000000h
		{0x1405AA0C6, 3, 1, 0x11ull, 0x12ull}, // shl rsi, 11h
		{0x1405AA0D6, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405AA0E2, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405AA298, 2, 8, 0xFFFFFF8003FFFFFFull, 0xFFFFFF0003FFFFFFull}, // mov rcx, 0FFFFFF8003FFFFFFh
		{0x1405AA2C4, 2, 4, 0x1FFFull, 0x3FFFull}, // and ebp, 1FFFh
		{0x1405AA2CD, 2, 4, 0x1FFFull, 0x3FFFull}, // and ebx, 1FFFh
		{0x1405AA5A4, 2, 4, 0x1FFFull, 0x3FFFull}, // and edi, 1FFFh
		{0x1405AA748, 2, 4, 0x1FFFull, 0x3FFFull}, // and ebp, 1FFFh
		{0x1405AA7B9, 2, 8, 0x7FFC000000ull, 0xFFFC000000ull}, // mov r9, 7FFC000000h
		{0x1405AA857, 2, 8, 0x7FFC000000ull, 0xFFFC000000ull}, // mov r9, 7FFC000000h
		{0x1405AA959, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov rax, 78000000000h
		{0x1405AA963, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405AA9A7, 2, 8, 0x78000000000ull, 0xF0000000000ull}, // mov rax, 78000000000h
		{0x1405AA9B1, 3, 1, 0x27ull, 0x28ull}, // shl rcx, 27h
		{0x1405ABD70, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405ABD7B, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405ABDD6, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405ABDE1, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405ABE55, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405ABE6D, 3, 1, 0x2Bull, 0x2Cull}, // shr rax, 2Bh
		{0x1405ABEA9, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x1405ACD93, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rbp, 7FFC00000000h
		{0x1405B1864, 1, 4, 0x1FFFull, 0x3FFFull}, // mov edx, 1FFFh
		{0x1405B21AD, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rax, 7FFC000000000000h
		{0x1405C8E36, 3, 1, 0x27ull, 0x28ull}, // shr rsi, 27h
		{0x1405C8F66, 3, 1, 0x27ull, 0x28ull}, // shr rsi, 27h
		{0x1405C9BD7, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405C9E65, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405CA015, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405CA170, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405CA335, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405CA490, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405CA699, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405CA8EB, 3, 1, 0x27ull, 0x28ull}, // shr rbx, 27h
		{0x1405D2EFA, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D2EFF, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D2F6F, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D2F75, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D2FD9, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D2FDF, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D303D, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D3043, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D30AC, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D30B2, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D31FB, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D3200, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D327F, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D3285, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D32EB, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D32F0, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D335A, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405D3360, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D33CC, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405D33D1, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1405DA3B6, 2, 4, 0x1FFFull, 0x3FFFull}, // and edx, 1FFFh
		{0x1405E90EF, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rdx, 7FFC00000000h
		{0x1405EA082, 2, 4, 0x3FCull, 0x800ull}, // mov r8d, 3FCh
		{0x1405EA0D4, 2, 4, 0x1FFFull, 0x3FFFull}, // and edi, 1FFFh
		{0x1405EC5DB, 1, 4, 0x2980ull, 0x4480ull}, // mov ecx, 2980h
		{0x1405EC65B, 1, 4, 0x1FFFull, 0x3FFFull}, // mov ebp, 1FFFh
		{0x1405EC660, 2, 8, 0x8003FFFFFFFFFFFFull, 0x3FFFFFFFFFFFFull}, // mov r14, 8003FFFFFFFFFFFFh
		{0x1405EC863, 2, 4, 0x1FFFull, 0x3FFFull}, // cmp esi, 1FFFh
		{0x1405EC86B, 1, 4, 0x1FFFull, 0x3FFFull}, // mov edx, 1FFFh
		{0x1405EC87C, 2, 8, 0x8003FFFFFFFFFFFFull, 0x3FFFFFFFFFFFFull}, // mov rax, 8003FFFFFFFFFFFFh
		{0x1405EC88D, 1, 4, 0x1FFFull, 0x3FFFull}, // and eax, 1FFFh
		{0x1405EC89C, 2, 8, 0x7FFC000000000000ull, 0xFFFC000000000000ull}, // mov rcx, 7FFC000000000000h
		{0x140606EBE, 3, 1, 0x27ull, 0x28ull}, // shr rax, 27h
		{0x140606EF6, 3, 1, 0x27ull, 0x28ull}, // shr rax, 27h
		{0x140607132, 1, 4, 0x1FFFull, 0x3FFFull}, // mov ecx, 1FFFh
		{0x140615DFD, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x140615E0A, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x140615E4F, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140615F45, 3, 1, 0x28ull, 0x29ull}, // shr rcx, 28h
		{0x140615F49, 2, 4, 0x7FFC00ull, 0x7FFE00ull}, // and ecx, 7FFC00h
		{0x14061600D, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x14061601A, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x14061605F, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x14061612E, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x14061613B, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x140616180, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140616257, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x140616264, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x1406162A9, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140616387, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x140616394, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x1406163D9, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140616809, 3, 1, 0x18ull, 0x19ull}, // shr rdx, 18h
		{0x140616811, 2, 4, 0x7FFC00ull, 0x7FFE00ull}, // and edx, 7FFC00h
		{0x1406168BD, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x1406168C8, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x1406169EE, 3, 1, 0x28ull, 0x29ull}, // shr rax, 28h
		{0x1406169F2, 1, 4, 0x7FFC00ull, 0x7FFE00ull}, // and eax, 7FFC00h
		{0x140616A9D, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140616AA8, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140616B9D, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140616BA9, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140616CA0, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140616CA5, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140616DA0, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140616DA5, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140616EA3, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140616EA8, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140616FB3, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140616FB8, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x1406170D8, 3, 1, 0x18ull, 0x19ull}, // shr rdx, 18h
		{0x1406170E0, 2, 4, 0x7FFC00ull, 0x7FFE00ull}, // and edx, 7FFC00h
		{0x140617561, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x1406175F4, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140617691, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140617731, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x14061795F, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x1406179E6, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x140617A6F, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x140617AFF, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x140617C51, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140617CE4, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140617D81, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140617E21, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140617EE8, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x140617F16, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x140617F23, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x140617F69, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140617FF5, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x140618023, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x140618030, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x140618076, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x1406180F0, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x1406180FD, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x140618143, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140618194, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x1406181A1, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x1406181E7, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140618254, 3, 1, 0x28ull, 0x29ull}, // shr rax, 28h
		{0x140618258, 1, 4, 0x7FFC00ull, 0x7FFE00ull}, // and eax, 7FFC00h
		{0x1406182A4, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x1406182B1, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x1406182F7, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140618345, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x140618352, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x140618398, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x1406183ED, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x1406183FA, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x14061843F, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x14061849D, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x1406184AA, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x1406184EF, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140618550, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x14061855D, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x1406185A2, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140618600, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x14061860D, 3, 1, 0xFull, 0x10ull}, // shr rcx, 0Fh
		{0x140618652, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x1406186C5, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov edx, 0FFF80000h
		{0x1406186D2, 3, 1, 0xFull, 0x10ull}, // shr rax, 0Fh
		{0x140618718, 2, 8, 0xFFFF0000000003FFull, 0xFFFF0000000001FFull}, // mov rdx, 0FFFF0000000003FFh
		{0x140618795, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x1406187CE, 3, 1, 0x11ull, 0x12ull}, // shr r9, 11h
		{0x1406187D5, 3, 4, 0x3FFE0000ull, 0x3FFF0000ull}, // and r9d, 3FFE0000h
		{0x140618864, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x140618881, 3, 1, 0x11ull, 0x12ull}, // shr rax, 11h
		{0x14061888B, 1, 4, 0x3FFE0000ull, 0x3FFF0000ull}, // and eax, 3FFE0000h
		{0x1406188FC, 3, 1, 0x18ull, 0x19ull}, // shr rax, 18h
		{0x140618906, 1, 4, 0x7FFC00ull, 0x7FFE00ull}, // and eax, 7FFC00h
		{0x140618937, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov eax, 0FFF80000h
		{0x14061894F, 3, 1, 0xFull, 0x10ull}, // shr r9, 0Fh
		{0x1406189C3, 3, 1, 0x28ull, 0x29ull}, // shr rcx, 28h
		{0x1406189C7, 2, 4, 0x7FFC00ull, 0x7FFE00ull}, // and ecx, 7FFC00h
		{0x140618A27, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov eax, 0FFF80000h
		{0x140618A3F, 3, 1, 0xFull, 0x10ull}, // shr r9, 0Fh
		{0x140618AA7, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov eax, 0FFF80000h
		{0x140618AC0, 3, 1, 0xFull, 0x10ull}, // shr r9, 0Fh
		{0x140618B35, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140618B3A, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140618BB5, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140618BBA, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140618C38, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140618C3D, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140618CB8, 1, 4, 0xFFF80000ull, 0xFFFC0000ull}, // mov ecx, 0FFF80000h
		{0x140618CBD, 3, 1, 0xFull, 0x10ull}, // shr r8, 0Fh
		{0x140618D51, 3, 1, 0x18ull, 0x19ull}, // shr rax, 18h
		{0x140618D5B, 1, 4, 0x7FFC00ull, 0x7FFE00ull}, // and eax, 7FFC00h
		{0x140618EA2, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140618EC2, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140618EE2, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140618F02, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140618F25, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140618F45, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140618FA7, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x140618FB7, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x140618FCA, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x140618FEA, 3, 1, 0x2Bull, 0x2Cull}, // shr rcx, 2Bh
		{0x1406192FB, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x140619411, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x140619526, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rax, 7FFC00000000h
		{0x140619782, 2, 8, 0x7FFC00000000ull, 0xFFFC00000000ull}, // mov rcx, 7FFC00000000h
		{0x1406259D3, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625A59, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625ADF, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625B65, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625D20, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625DA7, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625E2E, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
		{0x140625EB9, 2, 4, 0x1FFFull, 0x3FFFull}, // and ecx, 1FFFh
	};
}
