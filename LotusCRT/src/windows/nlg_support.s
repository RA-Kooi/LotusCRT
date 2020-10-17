; This is some kind of internal API that I think the Visual Studio debugger
; or WinDBG hooks into. I really have no idea if it's even required to be
; present for proper debugging support, but I don't care to find out.

%ifidn __OUTPUT_FORMAT__, win64
default rel
%else
global __NLG_Destination
%endif

section .data
__NLG_Destination: db 0x20, 0x05, 0x93, 0x19
	times 12 db 0

%ifidn __OUTPUT_FORMAT__, win32
	global __NLG_Notify
	global __NLG_Go
	global __NLG_Call
%else
global _NLG_Notify
%endif
global __NLG_Dispatch2
global __NLG_Return2
section .text

%ifidn __OUTPUT_FORMAT__, win32
; This is a symbol in libvcruntime.lib, used by _CallSettingFrame,
; void _NLG_Notify1
__NLG_Notify1:
	; Save non-volatile registers
	push ebx
	push ecx

	mov ebx, __NLG_Destination
	jmp __NLG_Go
	lea ecx, [ecx]
%endif

; This apparently notifies the debugger of an incoming stack unwind.
; It looks like it does nothing, but I think the debugger hooks it.
; void _NLG_Notify(
;	void *__offsetDestination,
;	void *__offsetFramePointer,
;	ULONG __code)
%ifidn __OUTPUT_FORMAT__, win64
_NLG_Notify:
	mov qword[rsp + 0x08], rcx ; __offsetDestination
	mov qword[rsp + 0x18], rdx ; __offsetFramePointer
	mov dword[rsp + 0x10], r8d ; __code
	mov r9, [__NLG_Destination]

	jmp __NLG_Dispatch2
%else
__NLG_Notify:
	push ebx
	push ecx
	mov ebx, __NLG_Destination
	mov ecx, dword[esp + 0x0C]

; void __NLG_Go();
__NLG_Go:
	mov dword[ebx + 0x08], ecx ; NLG_CODE
	mov dword[ebx + 0x04], eax ; funclet/handler
	mov dword[ebx + 0x0C], ebp ; target frame pointer
	push ebp
	push ecx
	push eax
%endif

; Cleans up the stack after dispatching a notification to the debugger.
; void __NLG_Dispatch2();
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
; Used to call the __finally handler during unwinding.
; void __naked _NLG_Call();
__NLG_Call:
	call eax
%endif

; void __NLG_Return2();
__NLG_Return2:
	ret
