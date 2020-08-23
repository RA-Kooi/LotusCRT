; This is x86 only

extern _RtlUnwind@16
extern $@__security_check_cookie@4
extern ___security_cookie
extern __NLG_Notify
extern __NLG_Call

global $@_Lotus_SEH_call_filter_func@8
global $@_Lotus_SEH_global_unwind@8
global $@_Lotus_SEH_local_unwind@16
global $@_Lotus_SEH_transfer_to_handler@8

section .text

$@_Lotus_SEH_call_filter_func@8:
	push ebp
	push esi
	push edi
	push ebx
	mov ebp, edx
	xor eax, eax
	xor ebx, ebx
	xor edx, edx
	xor esi, esi
	xor edi, edi
	call ecx
	pop ebx
	pop edi
	pop esi
	pop ebp
	ret

$@_Lotus_SEH_global_unwind@8:
	push ebp
	mov ebp, esp
	push ebx
	push esi
	push edi
	push 0
	push edx
	push __return_point
	push ecx
	call _RtlUnwind@16

__return_point:
	pop edi
	pop esi
	pop ebx
	pop ebp
	ret

$@_Lotus_SEH_local_unwind@16:
	push ebp
	mov ebp, dword[esp + 0x8]
	push edx
	push ecx
	push dword[esp + 0x14]
	call __Lotus_local_unwind
	add esp, 0xC
	pop ebp
	ret 8

__Lotus_local_unwind:
	push ebx
	push esi
	push edi
	mov edx, dword[esp + 0x10]
	mov eax, dword[esp + 0x14]
	mov ecx, dword[esp + 0x18]
	push ebp
	push edx
	push eax
	push ecx
	push ecx
	push __Lotus_unwind_handler
	push dword[fs: 0000h]
	mov eax, dword[___security_cookie]
	xor eax, esp
	mov dword[esp + 0x8], eax
	mov dword[fs: 0000h], esp
.unwind_top:
	mov eax, dword[esp + 0x30]
	mov ebx, dword[eax + 0x08]
	mov ecx, dword[esp + 0x2C]
	xor ebx, dword[ecx + 0x00]
	mov esi, dword[eax + 0x0C]
	cmp esi, 0xFFFFFFFE
	je .unwind_done
	mov edx, dword[esp + 0x34]
	cmp edx, 0xFFFFFFFE
	je .skip
	cmp esi, edx
	jbe .unwind_done
.skip:
	lea esi, [esi + esi * 0x2]
	lea ebx, [ebx + esi * 0x4 + 0x10]
	mov ecx, dword[ebx]
	mov dword[eax + 0xC], ecx
	cmp dword[ebx + 0x4], 0
	jne .unwind_top
	push 0x101
	mov eax, dword[ebx + 0x8]
	call __NLG_Notify
	mov ecx, 1
	mov eax, dword[ebx + 0x8]
	call __NLG_Call
	jmp .unwind_top
.unwind_done:
	pop dword[fs: 0000h]
	add esp, 0x18
	pop edi
	pop esi
	pop ebx
	ret

__Lotus_unwind_handler:
	mov ecx, dword[esp + 0x4]
	test dword[ecx + 0x4], 6
	mov eax, 1
	je .end
	mov eax, dword[esp + 0x8]
	mov ecx, dword[eax + 0x8]
	xor ecx, eax
	call $@__security_check_cookie@4
	push ebp
	mov ebp, dword[eax + 0x18]
	push dword[eax + 0x0C]
	push dword[eax + 0x10]
	push dword[eax + 0x14]
	call __Lotus_local_unwind
	add esp, 0xC
	pop ebp
	mov eax, dword[esp + 0x08]
	mov edx, dword[esp + 0x10]
	mov dword[edx], eax
	mov eax, 3
.end
	ret

$@_Lotus_SEH_transfer_to_handler@8:
	mov ebp, edx
	mov esi, ecx
	mov eax, ecx
	push 1
	call __NLG_Notify
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor edi, edi
	jmp esi
