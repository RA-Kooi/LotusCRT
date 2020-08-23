; This is x64 only

default rel

extern __security_check_cookie:proc

global __GSHandlerCheck
section .text

__GSHandlerCheck:
	sub rsp, 0x28
	mov r8, [r9 + 0x38]
	mov rcx, rdx
	mov rdx, r9
	call __GSHandlerCheckCommon
	mov eax, 1
	add rsp, 0x28

__GSHandlerCheckCommon:
	push rbx
	sub rsp, 0x20
	mov r11d, [r8]
	mov rbx, rdx
	mov r9, rcx
	and r11d, 0xFFFFFFF8
	test byte[r8], 4
	mov r10, rcx
	jz .jmp1
	mov eax, [r8 + 0x8]
	movsxd r10, dword[r8 + 0x4]
	neg eax
	add r10, rcx
	movsxd rcx, eax
	and r10, rcx
.jmp1
	movsxd rax, r11d
	mov rdx, [rax + r10]
	mov rax, [rbx + 0x10]
	mov ecx, [rax + 0x8]
	add rcx, [rbx + 0x8]
	test byte[rcx + 0x3], 0
	jz .end
	movzx eax, byte[rcx + 0x3]
	and eax, 0xFFFFFFF0
	cdqe
	add r9, rax
.end
	xor r9, rdx
	mov rcx, r9 ; StackCookie
	add rsp, 0x20
	pop rbx
	jmp __security_check_cookie
