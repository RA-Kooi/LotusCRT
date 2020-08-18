; _security_check_cookie(uintptr_t StackCookie)

default rel

extern __security_cookie:qword
extern __report_gsfailure:proc

global __security_check_cookie
section .text

__security_check_cookie:
%ifidn __OUTPUT_FORMAT__, win32
	cmp ecx, qword [__security_cookie]
	jnz .report_fail
	retn 0
%else
	cmp rcx, qword [__security_cookie]
	jnz .report_fail
	rol rcx, 0x10
	test cx, 0xFFFF
	jnz .restore_rcx
	rep retn
.restore_rcx:
	ror rcx, 0x10
%endif

.report_fail:
	jmp __report_gsfailure
