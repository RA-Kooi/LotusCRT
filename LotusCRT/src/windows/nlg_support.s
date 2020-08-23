%ifidn __OUTPUT_FORMAT__, win64
default rel
%else
global __NLG_Destination
section .data
__NLG_Destination: db 0x20, 0x05, 0x93, 0x19
%endif

%ifidn __OUTPUT_FORMAT__, win32
	global __NLG_Notify
	global __NLG_Notify1
	global __NLG_Go
	global __NLG_Call
%else
global _NLG_Notify
%endif
global __NLG_Dispatch2
global __NLG_Return2
section .text

%ifidn __OUTPUT_FORMAT__, win32
; void _NLG_Notify1
__NLG_Notify1:
	push ebx
	push ecx
	mov ebx, __NLG_Destination
	jmp __NLG_Go
	lea ecx, [ecx]
%endif

; void _NLG_Notify(
;	void *offsetDestination,
;	void *offsetFramePointer,
;	ULONG code)
%ifidn __OUTPUT_FORMAT__, win64
_NLG_Notify:
	mov qword[rsp + 0x08], rcx
	mov qword[rsp + 0x18], rdx
	mov dword[rsp + 0x10], r8d
	mov r9, 0x19930520

	jmp __NLG_Dispatch2
%else
__NLG_Notify:
	push ebx
	push ecx
	mov ebx, __NLG_Destination
	mov ecx, dword[esp + 0x0C]

; void __NLG_Go
__NLG_Go:
	mov dword[ebx + 0x08], ecx
	mov dword[ebx + 0x04], eax
	mov dword[ebx + 0x0C], ebp
	push ebp
	push ecx
	push eax
%endif

; void __NLG_Dispatch2()
__NLG_Dispatch2:
%ifidn __OUTPUT_FORMAT__, win32
	pop eax
	pop ecx
	pop ebp
	pop ecx
	pop ebx
	ret 4
%else
	ret
%endif

%ifidn __OUTPUT_FORMAT__, win32
; _NLG_Call
__NLG_Call:
	call eax
%endif

; void __NLG_Return2()
__NLG_Return2:
	ret
