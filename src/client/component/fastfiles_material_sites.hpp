#pragma once

#include "fastfiles_image_sites.hpp"

namespace material_bitset_sites
{
	using image_pool_sites::reloc;
	using image_pool_sites::constant;

	inline constexpr reloc relocs[] =
	{
		{0x140289860, 3, 2, 0x145124100}, // lea rax, [rdx+0C7280h]
		{0x1402898A7, 2, 2, 0x145124100}, // add rax, 0C5C80h
		{0x140289BBF, 3, 0, 0x140289BC6}, // lea rcx, unk_14320C850
		{0x140289BCD, 3, 0, 0x140289BD4}, // lea rax, dword_1451E9D80
		{0x140289C33, 3, 0, 0x140289C3A}, // lea rax, dword_1451EB380
		{0x140289E65, 4, 1, 0x140000000}, // or rva dword_1451E9D80[r13+r8*4], edx
		{0x14028A001, 3, 2, 0x145124100}, // or [rsi+rdi*4+0C7280h], eax
		{0x14028A126, 3, 0, 0x14028A12D}, // lea rcx, dword_14320FB40
		{0x14028A292, 3, 0, 0x14028A299}, // lea rcx, dword_14320FB40
		{0x14028A2D7, 3, 0, 0x14028A2DE}, // lea rbp, dword_1451E9D80
		{0x14028A305, 3, 0, 0x14028A30C}, // lea rsi, dword_1451EB380
		{0x14028A3B6, 3, 0, 0x14028A3BD}, // lea rcx, unk_14320C850
		{0x14028A57A, 3, 0, 0x14028A581}, // lea rcx, unk_14320C850
		{0x14028A5A6, 6, 2, 0x145124100}, // movdqu xmm1, xmmword ptr [r10+r9+0C7280h]
		{0x14028A94D, 4, 1, 0x140000000}, // or rva dword_14320FB40[r12+r14*4], eax
		{0x14028AA37, 4, 2, 0x145124100}, // or [r14+rdi*4+0C5C80h], edx
		{0x14028AC1C, 4, 2, 0x145124100}, // or [r12+rdi*4+0C5C80h], edx
		{0x14028AC47, 4, 2, 0x145124100}, // or [r12+rdi*4+0C5C80h], edx
		{0x14028AD07, 4, 2, 0x145124100}, // or [r12+rdx*4+0C5C80h], eax
		{0x14028AD2A, 3, 0, 0x14028AD31}, // lea rcx, dword_1451E9D80
		{0x14028AD3B, 3, 0, 0x14028AD42}, // lea rcx, unk_1451EA300
		{0x14028AD4C, 3, 0, 0x14028AD53}, // lea rcx, unk_1451EA880
		{0x14028AD5D, 3, 0, 0x14028AD64}, // lea rcx, unk_1451EAE00
		{0x14028ADFE, 3, 2, 0x145124100}, // or [r8+0C5C80h], edx
		{0x14028AE24, 3, 2, 0x145124100}, // or [r8+0C6200h], edx
		{0x14028AE4A, 3, 2, 0x145124100}, // or [r8+0C6780h], edx
		{0x14028AE6D, 3, 2, 0x145124100}, // or [r8+0C6D00h], edx
		{0x14028B31A, 3, 2, 0x145124100}, // or [r8+0C5C80h], edx
		{0x14028B33D, 3, 2, 0x145124100}, // or [r8+0C6200h], edx
		{0x14028B360, 3, 2, 0x145124100}, // or [r8+0C6780h], edx
		{0x14028B388, 2, 2, 0x145124100}, // or [rdx+0C6D00h], edi
		{0x14028B5C8, 3, 2, 0x145124100}, // or [rbp+rdi*4+0C7280h], eax
		{0x14028B76C, 4, 2, 0x145124100}, // or [rdi+r8*4+0C5C80h], edx
		{0x14028B87C, 4, 2, 0x145124100}, // or [r13+rdi*4+0C7280h], eax
		{0x1402C5DBA, 3, 0, 0x1402C5DC1}, // lea r13, dword_1451E9D80
		{0x1402C5DE0, 3, 2, 0x1451E9D80}, // mov r12d, [r13+1600h]
		{0x1402C5EBF, 3, 2, 0x1451E9D80}, // add rsi, 580h
		{0x1402C84C0, 4, 2, 0x140000000}, // mov esi, [r15+rbp+51E9D80h]
		{0x1402C84CC, 4, 2, 0x140000000}, // and esi, [r15+rbp+51EB380h]
		{0x1402CA292, 2, 0, 0x1402CA298}, // mov eax, cs:dword_1451EB380
		{0x1402CA340, 4, 2, 0x145124100}, // mov r8d, [r12+r15*4+0C7280h]
		{0x1402CA3D1, 4, 2, 0x145124100}, // test [r9+rax*4+0C5C80h], ebp
		{0x1402CA5E8, 4, 2, 0x145124100}, // test [r9+r14*4+0C6200h], ebp
		{0x1402CA940, 4, 2, 0x145124100}, // test [r8+r14*4+0C6780h], ebp
		{0x1402CAC90, 4, 2, 0x145124100}, // test [r12+r14*4+0C6D00h], ebp
	};

	inline constexpr constant constants[] =
	{
		{0x14028996A, 3, 4, 0x580, 0x800}, // add rbx, 580h
		{0x140289BB6, 1, 4, 0x2C, 0x40}, // mov edx, 2Ch
		{0x140289C3A, 1, 4, 0xB, 0x10}, // mov edx, 0Bh
		{0x14028A299, 2, 4, 0x580, 0x800}, // mov r8d, 580h
		{0x14028A2E3, 2, 4, 0x1600, 0x2000}, // mov r8d, 1600h
		{0x14028A311, 2, 4, 0x580, 0x800}, // mov r8d, 580h
		{0x14028A3C1, 3, 1, 0x2C, 0x40}, // lea edx, [r8+2Ch]
		{0x14028A42B, 2, 1, 0xB, 0x10}, // lea ecx, [rdx+0Bh]
		{0x14028A574, 2, 4, 0x1600, 0x2000}, // mov r8d, 1600h
		{0x14028A589, 1, 4, 0x2C, 0x40}, // mov edx, 2Ch
		{0x14028B762, 3, 4, 0x160, 0x200}, // imul rax, 160h
		{0x1402C859E, 3, 4, 0x2980, 0x3E80}, // cmp r14d, 2980h
		{0x1402CA333, 3, 4, 0x14C, 0x1F4}, // cmp r15d, 14Ch
		{0x1402CAF8C, 2, 4, 0x530, 0x7D0}, // mov r8d, 530h
	};
}
