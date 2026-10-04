#pragma once

namespace image_pool_sites
{
	enum reloc_kind : std::uint8_t
	{
		rip,
		rva,
		base,
	};

	struct reloc
	{
		std::uint64_t address;
		std::uint8_t offset;
		std::uint8_t kind;
		std::uint64_t base;
	};

	struct constant
	{
		std::uint64_t address;
		std::uint8_t offset;
		std::uint8_t size;
		std::int32_t old_value;
		std::int32_t new_value;
	};

	inline constexpr reloc relocs[] =
	{
	{0x14008AD1C, 3, 0, 0x14008AD23}, // lea rcx, dword_141C8B900
	{0x14008AD40, 4, 2, 0x141C8B900}, // movzx edi, word ptr [rcx+rbx*2+30580h]
	{0x14008AD5E, 3, 0, 0x14008AD65}, // lea rcx, dword_141C8B900
	{0x14008AD6A, 4, 2, 0x141C8B900}, // cmp [rcx+rdi*4+1780h], r9d
	{0x14008ADDB, 3, 0, 0x14008ADE2}, // lea rcx, dword_141C8B900
	{0x14008AE60, 5, 2, 0x141C8B900}, // movzx esi, word ptr [rcx+r14*2+30580h]
	{0x14008AE7B, 3, 0, 0x14008AE82}, // lea rcx, dword_141C8B900
	{0x14008AE87, 4, 2, 0x141C8B900}, // cmp [rcx+rsi*4+1780h], r9d
	{0x14008AEE4, 3, 0, 0x14008AEEB}, // lea rcx, dword_141C8B900
	{0x14008AF7C, 3, 0, 0x14008AF83}, // lea r13, unk_141CD3688
	{0x14008B1EC, 3, 0, 0x14008B1F3}, // lea r9, unk_141CD3688
	{0x14008B249, 3, 0, 0x14008B250}, // lea r9, unk_141CD3688
	{0x14008B40D, 3, 0, 0x14008B414}, // lea rax, unk_141CD3688
	{0x14008B4BA, 3, 0, 0x14008B4C1}, // lea r13, unk_141CD3688
	{0x14008B816, 4, 2, 0x141CD3680}, // mov r10, [rax+rbx+3A9820h]
	{0x14008B9DD, 3, 0, 0x14008B9E4}, // lea rax, unk_141CD3688
	{0x14008BC48, 4, 1, 0x140000000}, // test rva dword_145216E80[rbx+rax*4], r8d
	{0x14008BC84, 3, 1, 0x140000000}, // mov rax, [rbx+207CEA0h]
	{0x14008BC8B, 3, 1, 0x140000000}, // lea rbx, [rbx+207CEA0h]
	{0x14008BCE2, 3, 0, 0x14008BCE9}, // lea rax, unk_14207CEA0
	{0x14008BE71, 3, 0, 0x14008BE78}, // lea r15, dword_141C8B900
	{0x14008BE80, 5, 2, 0x141C8B900}, // movzx ebp, word ptr [r15+rsi*2+30580h]
	{0x14008BF88, 3, 0, 0x14008BF8F}, // lea rax, unk_14207CEA0
	{0x14008C056, 3, 0, 0x14008C05D}, // lea rdi, unk_141CD3688
	{0x14008C08E, 3, 0, 0x14008C095}, // lea rbx, unk_14207CEA0
	{0x14008C132, 3, 0, 0x14008C139}, // lea r14, dword_141C8B900
	{0x14008C20A, 3, 0, 0x14008C211}, // lea rcx, unk_14207CEA0
	{0x14008C2CB, 3, 0, 0x14008C2D2}, // lea rcx, unk_14207CEA0
	{0x14008C386, 3, 0, 0x14008C38D}, // lea rcx, dword_141C8B900
	{0x14008C3EC, 3, 0, 0x14008C3F3}, // lea rcx, unk_14207CEA0
	{0x14008C591, 3, 0, 0x14008C598}, // lea rdi, unk_141CD3688
	{0x14008C662, 3, 0, 0x14008C669}, // lea rdi, unk_141CD3688
	{0x14008C8D9, 3, 0, 0x14008C8E0}, // lea rdi, unk_141CD3688
	{0x14028978B, 4, 1, 0x140000000}, // test rva dword_145216E80[r14+rax*4], r8d
	{0x1402897C8, 4, 1, 0x140000000}, // test rva dword_145216E80[r14+rax*4], r8d
	{0x140289909, 4, 2, 0x145124100}, // test [rbp+rcx*4+0F2D80h], r8d
	{0x140289CA3, 3, 0, 0x140289CAA}, // lea rcx, dword_145216E80
	{0x140289CAF, 3, 0, 0x140289CB6}, // lea rax, unk_14320E3D0
	{0x140289E7C, 4, 1, 0x140000000}, // lea r8, ds:5216E80h[rdx*4]
	{0x14028A2B6, 3, 0, 0x14028A2BD}, // lea rbx, dword_145216E80
	{0x14028A483, 3, 0, 0x14028A48A}, // lea rax, unk_14320E3D0
	{0x14028A5E6, 3, 0, 0x14028A5ED}, // lea rax, unk_14320E3D0
	{0x14028A5F6, 6, 2, 0x145124100}, // movdqu xmm1, xmmword ptr [r14+r9+0F2D80h]
	{0x14028A616, 6, 2, 0x145124100}, // movdqu xmm1, xmmword ptr [r14+r9+0F2D70h]
	{0x14028A640, 4, 2, 0x145124100}, // mov ecx, [rdx+r9+0F2D80h]
	{0x14028A6E0, 4, 1, 0x140000000}, // test rva dword_145216E80[rsi+rax*4], r9d
	{0x14028A70E, 4, 1, 0x140000000}, // and r8d, rva dword_145216E80[rsi+rax*4]
	{0x14028A743, 4, 1, 0x140000000}, // test rva dword_145216E80[rsi+rax*4], r8d
	{0x14028A764, 4, 1, 0x140000000}, // and r10d, rva dword_145216E80[rsi+rax*4]
	{0x14028A7F0, 4, 1, 0x140000000}, // test rva dword_145216E80[rsi+rax*4], r8d
	{0x14028A810, 4, 1, 0x140000000}, // and r8d, rva dword_145216E80[rsi+rax*4]
	{0x14028A83C, 3, 1, 0x140000000}, // test rva dword_145216E80[rsi+rax*4], edx
	{0x14028A861, 4, 1, 0x140000000}, // and r10d, rva dword_145216E80[rsi+rax*4]
	{0x14028B033, 3, 2, 0x145124100}, // or [rdx+rbp+0F2D80h], eax
	{0x14028B149, 4, 1, 0x140000000}, // or rva dword_141C8B900[r12+rax*4], edx
	{0x14028B151, 4, 1, 0x140000000}, // or rva dword_145216E80[r12+rax*4], edx
	{0x14028B179, 4, 2, 0x140000000}, // cmp [r9+r12+5218610h], r13
	{0x14028B196, 4, 1, 0x140000000}, // lea r8, ds:5216E80h[rax*4]
	{0x14028B1A2, 4, 2, 0x140000000}, // cmp [r9+r12+5218628h], r13
	{0x14028B1BB, 4, 1, 0x140000000}, // lea rdx, ds:5216E80h[rax*4]
	{0x14028B1CE, 4, 2, 0x140000000}, // cmp [r9+r12+5218640h], r13
	{0x14028B1E7, 4, 1, 0x140000000}, // lea rdx, ds:5216E80h[rax*4]
	{0x14028B1FA, 4, 2, 0x140000000}, // cmp [r9+r12+5218658h], r13
	{0x14028B21D, 4, 1, 0x140000000}, // lea r8, ds:5216E80h[rax*4]
	{0x14028B238, 4, 1, 0x140000000}, // lea rdx, ds:5216E80h[rax*4]
	{0x14028B25A, 4, 1, 0x140000000}, // lea rdx, ds:5216E80h[rax*4]
	{0x14028B27F, 4, 1, 0x140000000}, // lea rdx, ds:5216E80h[rax*4]
	{0x14028B69A, 4, 2, 0x145124100}, // test [rdi+rcx*4+0F2D80h], r8d
	{0x14028B6EA, 4, 2, 0x145124100}, // test [r12+rcx*4+0F2D80h], r8d
	{0x1402C54A1, 3, 0, 0x1402C54A8}, // lea r11, dword_141C8B900
	{0x1402C54AB, 3, 0, 0x1402C54B2}, // lea rdi, dword_145216E80
	{0x1402C54B2, 3, 0, 0x1402C54B9}, // lea rsi, qword_145218600
	{0x1402C55B4, 3, 0, 0x1402C55BB}, // lea r8, qword_145218600
	{0x1402C563D, 4, 2, 0x145124100}, // test [r8+rax*4+20DD00h], edx
	{0x1402C565F, 4, 2, 0x145124100}, // or [r8+rax*4+20DD00h], r9d
	{0x1402C56A6, 4, 2, 0x145124100}, // mov r10d, [r8+r11*4+20DD00h]
	{0x1402C56B6, 4, 2, 0x145124100}, // test [r8+rax*4+20DD00h], r9d
	{0x1402C56C3, 4, 2, 0x145124100}, // mov [r8+r11*4+20DD00h], r10d
	{0x1402C56D2, 4, 2, 0x145124100}, // mov [r8+r11*4+20DD00h], edx
	{0x1402C56DE, 4, 2, 0x145124100}, // or [r8+rax*4+20DD00h], r9d
	{0x1402C56EB, 4, 2, 0x145124100}, // and [r8+rax*4+20DD00h], r9d
	{0x1402C570A, 5, 2, 0x145124100}, // movups xmm6, xmmword ptr [rcx+r8+0F4540h]
	{0x1402C5713, 5, 2, 0x145124100}, // movups xmm0, xmmword ptr [rax+r8+0F4500h]
	{0x1402C571C, 5, 2, 0x145124100}, // movups xmm7, xmmword ptr [rcx+r8+0F4550h]
	{0x1402C5725, 5, 2, 0x145124100}, // movups xmm2, xmmword ptr [rcx+r8+0F4500h]
	{0x1402C572E, 5, 2, 0x145124100}, // movups xmm3, xmmword ptr [rcx+r8+0F4510h]
	{0x1402C5737, 5, 2, 0x145124100}, // movups xmm4, xmmword ptr [rcx+r8+0F4520h]
	{0x1402C5740, 5, 2, 0x145124100}, // movups xmm5, xmmword ptr [rcx+r8+0F4530h]
	{0x1402C5749, 5, 2, 0x145124100}, // movups xmmword ptr [rcx+r8+0F4500h], xmm0
	{0x1402C5752, 5, 2, 0x145124100}, // movups xmm1, xmmword ptr [rax+r8+0F4510h]
	{0x1402C575B, 5, 2, 0x145124100}, // movups xmmword ptr [rcx+r8+0F4510h], xmm1
	{0x1402C5764, 5, 2, 0x145124100}, // movups xmm0, xmmword ptr [rax+r8+0F4520h]
	{0x1402C576D, 5, 2, 0x145124100}, // movups xmmword ptr [rcx+r8+0F4520h], xmm0
	{0x1402C5776, 5, 2, 0x145124100}, // movups xmm1, xmmword ptr [rax+r8+0F4530h]
	{0x1402C577F, 5, 2, 0x145124100}, // movups xmmword ptr [rcx+r8+0F4530h], xmm1
	{0x1402C5788, 5, 2, 0x145124100}, // movups xmm0, xmmword ptr [rax+r8+0F4540h]
	{0x1402C5791, 5, 2, 0x145124100}, // movups xmmword ptr [rcx+r8+0F4540h], xmm0
	{0x1402C579A, 5, 2, 0x145124100}, // movups xmm1, xmmword ptr [rax+r8+0F4550h]
	{0x1402C57A3, 5, 2, 0x145124100}, // movups xmmword ptr [rcx+r8+0F4550h], xmm1
	{0x1402C57AC, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4500h], xmm2
	{0x1402C57B5, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4510h], xmm3
	{0x1402C57BE, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4520h], xmm4
	{0x1402C57C7, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4530h], xmm5
	{0x1402C57D0, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4540h], xmm6
	{0x1402C57DE, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4550h], xmm7
	{0x1402C57E7, 4, 2, 0x145124100}, // mov eax, [r8+rbp*4+0C7800h]
	{0x1402C57EF, 6, 2, 0x145124100}, // movss xmm0, dword ptr [r8+rsi*4+0C7800h]
	{0x1402C57FE, 4, 2, 0x145124100}, // mov [r8+rsi*4+0C7800h], eax
	{0x1402C5806, 6, 2, 0x145124100}, // movss dword ptr [r8+rbp*4+0C7800h], xmm0
	{0x1402C5826, 4, 2, 0x145124100}, // and [r8+rax*4+20DD00h], r9d
	{0x1402C583E, 5, 2, 0x145124100}, // movups xmm0, xmmword ptr [rcx+r8+0F4500h]
	{0x1402C5847, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4500h], xmm0
	{0x1402C5850, 5, 2, 0x145124100}, // movups xmm1, xmmword ptr [rcx+r8+0F4510h]
	{0x1402C5859, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4510h], xmm1
	{0x1402C5862, 5, 2, 0x145124100}, // movups xmm0, xmmword ptr [rcx+r8+0F4520h]
	{0x1402C586B, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4520h], xmm0
	{0x1402C5874, 5, 2, 0x145124100}, // movups xmm1, xmmword ptr [rcx+r8+0F4530h]
	{0x1402C587D, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4530h], xmm1
	{0x1402C5886, 5, 2, 0x145124100}, // movups xmm0, xmmword ptr [rcx+r8+0F4540h]
	{0x1402C588F, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4540h], xmm0
	{0x1402C5898, 5, 2, 0x145124100}, // movups xmm1, xmmword ptr [rcx+r8+0F4550h]
	{0x1402C58A1, 5, 2, 0x145124100}, // movups xmmword ptr [rax+r8+0F4550h], xmm1
	{0x1402C58AA, 4, 2, 0x145124100}, // mov eax, [r8+r10*4+0C7800h]
	{0x1402C58B2, 4, 2, 0x145124100}, // mov [r8+rdx*4+0C7800h], eax
	{0x1402C593D, 3, 0, 0x1402C5944}, // lea rax, qword_145218600
	{0x1402C5993, 4, 2, 0x145124100}, // or [r15+rax*4+20DD00h], edx
	{0x1402C59A1, 4, 2, 0x145124100}, // and [r15+rax*4+20DD00h], edx
	{0x1402C5ADF, 6, 2, 0x145124100}, // movss dword ptr [r15+r12*4+0C7800h], xmm0
	{0x1402C5B47, 5, 1, 0x140000000}, // movzx r11d, rva word_141CBBE80[rbx+rcx*2]
	{0x1402C5B57, 4, 1, 0x140000000}, // mov r8d, rva dword_141C8B900[rbx+rcx*4]
	{0x1402C5B62, 4, 1, 0x140000000}, // or r8d, rva dword_145216E80[rbx+rcx*4]
	{0x1402C5B77, 5, 1, 0x140000000}, // mov rva word_141CBBE80[rbx+r9*2], r11w
	{0x1402C5D02, 3, 0, 0x1402C5D09}, // lea rax, qword_145218600
	{0x1402C5E7D, 4, 1, 0x140000000}, // lea r11, ds:5339A80h[rcx*4]
	{0x1402C5E8E, 5, 1, 0x140000000}, // mov rva word_141CBBE80[r13+r10*2], r8w
	{0x1402C5F9F, 3, 0, 0x1402C5FA6}, // lea rcx, unk_145339A80
	{0x1402C62EC, 4, 1, 0x140000000}, // mov rdx, rva qword_145218608[rdi+rcx*8]
	{0x1402C62F4, 4, 1, 0x140000000}, // mov rcx, rva qword_145218600[rdi+rcx*8]
	{0x1402C636E, 3, 0, 0x1402C6375}, // lea r15, dword_145216E80
	{0x1402C6378, 3, 0, 0x1402C637F}, // lea rbp, dword_141C8B900
	{0x1402C63C4, 4, 2, 0x141C8B900}, // mov [rbp+rax*2+30580h], bx
	{0x1402C63EB, 3, 2, 0x141C8B900}, // lea rdx, [rbp+30580h]
	{0x1402C646D, 3, 2, 0x141C8B900}, // lea r12, [rbp+30580h]
	{0x1402C6518, 3, 0, 0x1402C651F}, // lea rbp, dword_141C8B900
	{0x1402C6958, 4, 1, 0x140000000}, // cmp rva qword_145218600[rcx+rdi*8], rax
	{0x1402C69A3, 3, 1, 0x140000000}, // test rva dword_145216E80[rsi+rax*4], edx
	{0x1402C69B7, 4, 1, 0x140000000}, // mov rcx, rva qword_145218608[rsi+rdi*8]
	{0x1402C69BF, 4, 1, 0x140000000}, // sub rcx, rva qword_145218600[rsi+rdi*8]
	{0x1402C6A57, 3, 1, 0x140000000}, // lea r12, rva qword_145218600[rcx]
	{0x1402C6B17, 4, 1, 0x140000000}, // mov edx, rva dword_141C8D080[rax+r14*4]
	{0x1402C6B37, 4, 1, 0x140000000}, // cmp rva dword_141C8D080[rsi+r14*4], 2000h
	{0x1402C6FBE, 3, 0, 0x1402C6FC5}, // lea r9, dword_141C8B900
	{0x1402C71A3, 3, 0, 0x1402C71AA}, // lea rax, dword_141C8B900
	{0x1402C76A2, 3, 0, 0x1402C76A9}, // lea rax, dword_141C8B900
	{0x1402C7E50, 3, 1, 0x140000000}, // mov rva dword_141C8D080[rsi+rax*4], 1FC000h
	{0x1402C7FC0, 3, 0, 0x1402C7FC7}, // lea r12, dword_141C8B900
	{0x1402C80B4, 3, 0, 0x1402C80BB}, // lea rcx, unk_141C8D0A0
	{0x1402C8212, 4, 1, 0x140000000}, // test rva dword_145216E80[r12+rax*4], edx
	{0x1402C821C, 4, 1, 0x140000000}, // test rva dword_141C8B900[r12+rax*4], edx
	{0x1402C8267, 4, 1, 0x140000000}, // test rva dword_145216E80[r12+rax*4], edx
	{0x1402C8271, 4, 1, 0x140000000}, // test rva dword_141C8B900[r12+rax*4], edx
	{0x1402C8568, 4, 1, 0x140000000}, // test ss:rva dword_141C8B900[rbp+rcx*4], r8d
	{0x1402C86A3, 3, 0, 0x1402C86AA}, // lea rcx, word_141CBBE80
	{0x1402C86B5, 4, 1, 0x140000000}, // lea rdx, ds:1CBBE80h[r8*2]
	{0x1402C86E8, 4, 1, 0x140000000}, // movzx ecx, rva word_141CBBE80[rsi+rbx*2]
	{0x1402C86F0, 3, 1, 0x140000000}, // cmp rva dword_141C8D080[rsi+rcx*4], 2000h
	{0x1402C8711, 3, 1, 0x140000000}, // lea rdi, rva word_141CBBE80[rsi]
	{0x1402C872A, 3, 0, 0x1402C8731}, // lea rcx, word_141CBBE80
	{0x1402C8740, 4, 1, 0x140000000}, // movzx ecx, rva word_141CBBE80[rsi+rbx*2]
	{0x1402C8748, 4, 1, 0x140000000}, // cmp rva dword_141C8D080[rsi+rcx*4], r14d
	{0x1402C8758, 3, 1, 0x140000000}, // lea rdx, rva word_141CBBE80[rsi]
	{0x1402C87B0, 5, 1, 0x140000000}, // movzx r10d, rva word_141CBBE80[r14+rbx*2]
	{0x1402C87CD, 4, 1, 0x140000000}, // test rva dword_141C8B900[r14+rax*4], edx
	{0x1402C87E0, 3, 1, 0x140000000}, // lea r8, rva qword_145218600[r14]
	{0x1402C8819, 4, 1, 0x140000000}, // mov ecx, rva dword_141C8D080[rax+r10*4]
	{0x1402C88E1, 3, 1, 0x140000000}, // test rva dword_141C8B900[rcx+rax*4], edx
	{0x1402C89B0, 5, 1, 0x140000000}, // movzx r9d, rva word_141CBBE80[r12+r10*2]
	{0x1402C89CD, 4, 1, 0x140000000}, // test rva dword_141C8B900[r12+rax*4], edx
	{0x1402C89DB, 4, 1, 0x140000000}, // cmp rva dword_141C8D080[r12+r9*4], esi
	{0x1402C8A07, 4, 1, 0x140000000}, // lea rcx, ds:5218600h[rax*8]
	{0x1402C8A58, 4, 1, 0x140000000}, // test rva dword_141C8B900[r12+rax*4], edx
	{0x1402C8BB2, 3, 2, 0x145124100}, // mov r9, [r8+0F4510h]
	{0x1402C8BC4, 3, 2, 0x145124100}, // mov rdx, [rcx+0F4510h]
	{0x1402C8BEF, 3, 2, 0x145124100}, // mov rcx, [rcx+0F4500h]
	{0x1402C8BF8, 3, 2, 0x145124100}, // cmp [r8+0F4500h], rcx
	{0x1402C8C30, 3, 0, 0x1402C8C37}, // lea rcx, qword_145218600
	{0x1402C8C40, 3, 0, 0x1402C8C47}, // lea r9, dword_141C8B900
	{0x1402C8C4C, 4, 2, 0x141C8B900}, // mov eax, [r9+rax*4+1780h]
	{0x1402C8C54, 4, 2, 0x141C8B900}, // sub eax, [r9+r8*4+1780h]
	{0x1402C9DF1, 3, 0, 0x1402C9DF8}, // lea r9, dword_141C8B900
	{0x1402C9DF8, 3, 0, 0x1402C9DFF}, // lea rdx, dword_141C8D080
	{0x1402CA0A1, 3, 0, 0x1402CA0A8}, // lea rsi, dword_141C8B900
	{0x1402CA0EF, 4, 2, 0x141C8B900}, // cmp [rsi+r10*4+1780h], edi
	{0x1402CA263, 3, 0, 0x1402CA26A}, // lea rax, unk_145331E00
	{0x1402CA26E, 3, 0, 0x1402CA275}, // lea rdi, unk_14534C580
	{0x1402CA761, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r13+r8*4+0C7800h]
	{0x1402CA7B8, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r13+r8*4+0C7800h]
	{0x1402CA80E, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r13+r8*4+0C7800h]
	{0x1402CA865, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r13+r8*4+0C7800h]
	{0x1402CA8E8, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r13+r8*4+0C7800h]
	{0x1402CAAB0, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r12+r8*4+0C7800h]
	{0x1402CAB06, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r12+r8*4+0C7800h]
	{0x1402CAB5C, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r12+r8*4+0C7800h]
	{0x1402CABB2, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r12+r8*4+0C7800h]
	{0x1402CAC2A, 6, 2, 0x145124100}, // mulss xmm0, dword ptr [r12+r8*4+0C7800h]
	{0x1402CACFF, 6, 2, 0x145124100}, // mulss xmm2, dword ptr [r12+rcx*4+0C7800h]
	{0x1402CAD6B, 6, 2, 0x145124100}, // mulss xmm2, dword ptr [r12+rcx*4+0C7800h]
	{0x1402CADDE, 6, 2, 0x145124100}, // mulss xmm2, dword ptr [r12+rcx*4+0C7800h]
	{0x1402CAE52, 6, 2, 0x145124100}, // mulss xmm2, dword ptr [r12+rcx*4+0C7800h]
	{0x1402CAF01, 6, 2, 0x145124100}, // mulss xmm2, dword ptr [r12+rcx*4+0C7800h]
	};

	inline constexpr constant constants[] =
	{
	{0x14008AD99, 2, 1, 0x8, 0x9}, // shr esi, 8
	{0x14008AD9C, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, esi, 2EE0h
	{0x14008AE11, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x14008AE14, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x14008AE9C, 2, 1, 0x8, 0x9}, // shr edi, 8
	{0x14008AE9F, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edi, 2EE0h
	{0x14008AF9B, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008AF9E, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008B207, 3, 1, 0x8, 0x9}, // shr r14d, 8
	{0x14008B219, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008B21C, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008B277, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008B27A, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008B402, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008B405, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008B4D5, 3, 1, 0x8, 0x9}, // shr r14d, 8
	{0x14008B4E9, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008B4EC, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008B55A, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008B55D, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008B661, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x14008B698, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x14008B9D2, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008B9D5, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008BC1D, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x14008BC20, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x14008BCF0, 1, 4, 0x2EE0, 0x5DC0}, // mov ecx, 2EE0h
	{0x14008BD56, 1, 4, 0xBB80, 0x17700}, // add eax, 0BB80h
	{0x14008BEAD, 3, 1, 0x8, 0x9}, // shr r14d, 8
	{0x14008BEB1, 3, 4, 0x2EE0, 0x5DC0}, // imul eax, r14d, 2EE0h
	{0x14008BF49, 1, 4, 0xBB80, 0x17700}, // add eax, 0BB80h
	{0x14008C069, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008C06C, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008C095, 1, 4, 0x2EE0, 0x5DC0}, // mov ebp, 2EE0h
	{0x14008C13F, 3, 1, 0x8, 0x9}, // shr r11d, 8
	{0x14008C158, 3, 4, 0x2EE0, 0x5DC0}, // imul r9d, eax, 2EE0h
	{0x14008C18F, 3, 1, 0x8, 0x9}, // shr r8d, 8
	{0x14008C1BE, 1, 4, 0xBB80, 0x17700}, // add eax, 0BB80h
	{0x14008C281, 3, 4, -0x2EE0, -0x5DC0}, // add r10d, 0FFFFD120h
	{0x14008C2BE, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x14008C2C1, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x14008C372, 1, 4, 0xBB80, 0x17700}, // add eax, 0BB80h
	{0x14008C3B3, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x14008C3E2, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x14008C5BB, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008C5BE, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008C6C9, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008C6CC, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008C72B, 2, 1, 0xA, 0xB}, // shr edx, 0Ah
	{0x14008C72E, 2, 4, 0xBB80, 0x17700}, // imul eax, edx, 0BB80h
	{0x14008C949, 2, 4, 0xBB80, 0x17700}, // cmp ecx, 0BB80h
	{0x14008C957, 1, 4, 0xBB80, 0x17700}, // cmp eax, 0BB80h
	{0x14008C95E, 2, 4, -0xBB80, -0x17700}, // add ecx, 0FFFF4480h
	{0x14008C964, 1, 4, -0xBB80, -0x17700}, // add eax, 0FFFF4480h
	{0x140289960, 2, 4, 0x2EE0, 0x5DC0}, // add edi, 2EE0h
	{0x140289971, 2, 4, 0xBB80, 0x17700}, // cmp edi, 0BB80h
	{0x140289CAA, 1, 4, 0x2E, 0x5D}, // mov edx, 2Eh
	{0x140289E4C, 3, 4, 0x2EE0, 0x5DC0}, // cmp r9d, 2EE0h
	{0x14028A2CC, 2, 4, 0x1770, 0x2EE0}, // mov r8d, 1770h
	{0x14028A48A, 1, 4, 0x2E, 0x5D}, // mov ecx, 2Eh
	{0x14028A5E1, 1, 4, 0xBB, 0x177}, // mov ecx, 0BBh
	{0x14028A631, 1, 4, 0x1760, 0x2ED0}, // mov edx, 1760h
	{0x14028A6AF, 3, 4, 0x5DC0, 0xBB80}, // lea r8d, [rcx+5DC0h]
	{0x14028A6DA, 2, 4, 0x8CA0, 0x11940}, // lea edx, [rdi+8CA0h]
	{0x14028A6EF, 2, 4, 0x5DC0, 0xBB80}, // lea edx, [rbx+5DC0h]
	{0x14028A737, 2, 4, 0x8CA0, 0x11940}, // lea edx, [rbx+8CA0h]
	{0x14028A7E2, 2, 4, 0x2EE0, 0x5DC0}, // lea edx, [rdi+2EE0h]
	{0x14028A843, 2, 4, 0x2EE0, 0x5DC0}, // lea edx, [rbx+2EE0h]
	{0x14028A91B, 2, 4, 0x2EE0, 0x5DC0}, // cmp ecx, 2EE0h
	{0x14028B006, 3, 4, 0x2EE0, 0x5DC0}, // imul r10d, esi, 2EE0h
	{0x14028B1AC, 2, 4, 0x2EE0, 0x5DC0}, // lea ecx, [rbx+2EE0h]
	{0x14028B1D8, 2, 4, 0x5DC0, 0xBB80}, // lea ecx, [rbx+5DC0h]
	{0x14028B229, 2, 4, 0x2EE0, 0x5DC0}, // lea ecx, [rbx+2EE0h]
	{0x14028B247, 2, 4, 0x5DC0, 0xBB80}, // lea ecx, [rbx+5DC0h]
	{0x14028B26D, 2, 4, 0x8CA0, 0x11940}, // add ebx, 8CA0h
	{0x14028B66A, 2, 4, 0x2EE0, 0x5DC0}, // imul ebx, esi, 2EE0h
	{0x14028B776, 2, 4, 0x2EE0, 0x5DC0}, // add ebx, 2EE0h
	{0x1402C5505, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C5508, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C5536, 3, 4, 0xBB80, 0x17700}, // cmp r10d, 0BB80h
	{0x1402C5EB9, 2, 4, 0x2EE0, 0x5DC0}, // add ebx, 2EE0h
	{0x1402C5ECA, 2, 4, 0xBB80, 0x17700}, // cmp ebx, 0BB80h
	{0x1402C5FA6, 2, 4, 0x1770, 0x2EE0}, // mov r8d, 1770h
	{0x1402C62D9, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C62DC, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C63A8, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C63D6, 3, 4, 0xBB80, 0x17700}, // cmp r14d, 0BB80h
	{0x1402C6927, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C692A, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C6A3D, 2, 1, 0x8, 0x9}, // shr esi, 8
	{0x1402C6A40, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, esi, 2EE0h
	{0x1402C6D34, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, esi, 2EE0h
	{0x1402C6FE0, 3, 4, 0x2EE0, 0x5DC0}, // lea ecx, [r8+2EE0h]
	{0x1402C6FEF, 3, 4, 0x5DC0, 0xBB80}, // lea ecx, [r8+5DC0h]
	{0x1402C6FFE, 3, 4, 0x8CA0, 0x11940}, // lea ecx, [r8+8CA0h]
	{0x1402C7E3C, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C7E5B, 2, 4, 0x2EE0, 0x5DC0}, // lea eax, [rax+2EE0h]
	{0x1402C7E69, 2, 4, 0x2EE0, 0x5DC0}, // cmp ebx, 2EE0h
	{0x1402C7FF4, 2, 4, 0x2EE0, 0x5DC0}, // imul ebx, edi, 2EE0h
	{0x1402C8028, 2, 4, 0x2EE0, 0x5DC0}, // sub ebx, 2EE0h
	{0x1402C8035, 2, 4, 0x2EE0, 0x5DC0}, // cmp esi, 2EE0h
	{0x1402C80AF, 1, 4, 0x5DC, 0xBB8}, // mov edx, 5DCh
	{0x1402C81E8, 2, 4, 0x2EE0, 0x5DC0}, // imul ebx, edi, 2EE0h
	{0x1402C823A, 2, 4, 0x2EE0, 0x5DC0}, // sub ebx, 2EE0h
	{0x1402C8292, 2, 4, 0x2EE0, 0x5DC0}, // add edi, 2EE0h
	{0x1402C829F, 3, 4, 0x2EE0, 0x5DC0}, // cmp r14d, 2EE0h
	{0x1402C87ED, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C87F0, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C88BD, 3, 4, -0x2EE0, -0x5DC0}, // add r10d, 0FFFFD120h
	{0x1402C89F4, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C89F7, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C8A38, 3, 4, -0x2EE0, -0x5DC0}, // lea r8d, [r9-2EE0h]
	{0x1402C8B86, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C8B9E, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C8B89, 3, 4, 0x2EE0, 0x5DC0}, // imul r8d, edx, 2EE0h
	{0x1402C8BA5, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C8C1A, 2, 1, 0x8, 0x9}, // shr edx, 8
	{0x1402C8C1D, 2, 4, 0x2EE0, 0x5DC0}, // imul eax, edx, 2EE0h
	{0x1402C9E38, 1, 4, 0x5DC, 0xBB8}, // mov ecx, 5DCh
	{0x1402CA0FB, 3, 4, 0x2EE0, 0x5DC0}, // add r9d, 2EE0h
	{0x1402CA108, 2, 4, 0x2EE0, 0x5DC0}, // cmp ebx, 2EE0h
	{0x1402CA14B, 1, 4, 0x5DC, 0xBB8}, // mov eax, 5DCh
	{0x1402CA223, 1, 4, 0x5DC, 0xBB8}, // mov edx, 5DCh
	{0x1402CA688, 4, 4, 0xBB80, 0x17700}, // mov dword ptr [r10+rax*4+0BB80h], 0BA03126Fh
	{0x1402CA698, 4, 4, 0xBB80, 0x17700}, // mov dword ptr [r10+rax*4+0BB80h], 0BA03126Fh
	{0x1402CA6A8, 4, 4, 0xBB80, 0x17700}, // mov dword ptr [r10+rax*4+0BB80h], 0BA03126Fh
	{0x1402CA6B8, 4, 4, 0xBB80, 0x17700}, // mov dword ptr [r10+rax*4+0BB80h], 0BA03126Fh
	{0x1402CA6E4, 4, 4, 0xBB80, 0x17700}, // mov dword ptr [r10+rax*4+0BB80h], 0BA03126Fh
	{0x1402CA757, 3, 4, 0x2EE0, 0x5DC0}, // lea r9d, [rcx+2EE0h]
	{0x1402CA7AB, 3, 4, 0x2EE0, 0x5DC0}, // lea r9d, [rcx+2EE0h]
	{0x1402CA801, 3, 4, 0x2EE0, 0x5DC0}, // lea r9d, [rcx+2EE0h]
	{0x1402CA858, 3, 4, 0x2EE0, 0x5DC0}, // lea r9d, [rcx+2EE0h]
	{0x1402CA8DE, 3, 4, 0x2EE0, 0x5DC0}, // lea r9d, [rcx+2EE0h]
	{0x1402CA9E0, 4, 4, 0x17700, 0x2EE00}, // mov dword ptr [r10+rax*4+17700h], 3A03126Fh
	{0x1402CA9F0, 4, 4, 0x17700, 0x2EE00}, // mov dword ptr [r10+rax*4+17700h], 3A03126Fh
	{0x1402CAA00, 4, 4, 0x17700, 0x2EE00}, // mov dword ptr [r10+rax*4+17700h], 3A03126Fh
	{0x1402CAA10, 4, 4, 0x17700, 0x2EE00}, // mov dword ptr [r10+rax*4+17700h], 3A03126Fh
	{0x1402CAA3C, 4, 4, 0x17700, 0x2EE00}, // mov dword ptr [r10+rax*4+17700h], 3A03126Fh
	{0x1402CAAA6, 3, 4, 0x5DC0, 0xBB80}, // lea r9d, [rcx+5DC0h]
	{0x1402CAAF9, 3, 4, 0x5DC0, 0xBB80}, // lea r9d, [rcx+5DC0h]
	{0x1402CAB4F, 3, 4, 0x5DC0, 0xBB80}, // lea r9d, [rcx+5DC0h]
	{0x1402CABA5, 3, 4, 0x5DC0, 0xBB80}, // lea r9d, [rcx+5DC0h]
	{0x1402CAC20, 3, 4, 0x5DC0, 0xBB80}, // lea r9d, [rcx+5DC0h]
	{0x1402CAD09, 2, 4, 0x8CA0, 0x11940}, // lea eax, [rcx+8CA0h]
	{0x1402CAD57, 2, 4, 0x8CA0, 0x11940}, // lea eax, [rcx+8CA0h]
	{0x1402CADCA, 2, 4, 0x8CA0, 0x11940}, // lea eax, [rcx+8CA0h]
	{0x1402CAE3E, 2, 4, 0x8CA0, 0x11940}, // lea eax, [rcx+8CA0h]
	{0x1402CAF0B, 2, 4, 0x8CA0, 0x11940}, // lea eax, [rcx+8CA0h]
	};
}
