; _security_check_cookie(uintptr_t StackCookie)

%ifidn __OUTPUT_FORMAT__, win64
default rel

extern __security_cookie:qword
extern __report_gsfailure:proc
global __security_check_cookie
%else
extern ___security_cookie:dword
extern ___report_gsfailure:proc
%define __report_gsfailure ___report_gsfailure
global $@__security_check_cookie@4
%endif

section .text

%ifidn __OUTPUT_FORMAT__, win32
$@__security_check_cookie@4:
	cmp ecx, dword [___security_cookie]
	jnz .report_fail
	rep ret
%else
__security_check_cookie:
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
