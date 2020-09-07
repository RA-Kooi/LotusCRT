; This is x86 only

section .rodata
; Exception flags
EXCEPTION_UNWINDING:      equ 0x2 ; Unwind is in progress
EXCEPTION_EXIT_UNWIND:    equ 0x4 ; Exit unwind is in progress
EXCEPTION_UNWIND_CONTEXT: equ 0x6 ; EXCEPTION_UNWINDING | EXCEPTION_EXIT_UNWIND

; Exception disposition
ExceptionContinueSearch:   equ 1
ExceptionCollidedUnwind:   equ 3

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

; LONG __fastcall _Lotus_SEH_call_filter_func(
;	PEXCEPTION_FILTER_x86 __filterFunc,
;	PCHAR __framePointer);
$@_Lotus_SEH_call_filter_func@8:
	push ebp ; Save old call frame

	; Save non-volatile registers
	push esi
	push edi
	push ebx

	mov ebp, edx ; Overwrite the frame pointer with the given frame pointer

	; Clear all general purpose registers, don't want to leak state into the
	; filter function.
	xor eax, eax
	xor ebx, ebx
	xor edx, edx
	xor esi, esi
	xor edi, edi

	call ecx ; call __filterFunc

	; Restore non-volatile registers
	pop ebx
	pop edi
	pop esi

	pop ebp ; Restore old call frame
	ret

; void _Noreturn __fastcall _Lotus_SEH_global_unwind(
;	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
;	PEXCEPTION_RECORD __exceptionRecord);
$@_Lotus_SEH_global_unwind@8:
	push ebp ; Save old call frame
	mov ebp, esp ; Create new call frame

	; Save non-volatile registers
	push ebx
	push esi
	push edi

	push 0              ; Return value
	push edx            ; ExceptionRecord = __exceptionRecord
	push __return_point ; TargetIp
	push ecx            ; TargetFrame = __establisherFrame
	call _RtlUnwind@16

__return_point:
	; Restore non-volatile registers
	pop edi
	pop esi
	pop ebx

	pop ebp ; Restore old call frame
	ret

; void __fastcall _Lotus_SEH_local_unwind(
;	PEXCEPTION_REGISTRATION_RECORD __establisherFrame,
;	ULONG __targetLevel,
;	PCHAR __framePointer,
;	PUINT_PTR __cookie);
$@_Lotus_SEH_local_unwind@16:
	push ebp ; Save old call frame
	mov ebp, dword[esp + 0x8] ; Overwrite the frame pointer with the given
	; frame pointer

	push edx               ; __targetLevel
	push ecx               ; __establisherFrame
	push dword[esp + 0x14] ; __cookie
	call __Lotus_local_unwind

	add esp, 12 ; pop arguments from the stack
	pop ebp     ; Restore old call frame

	ret 8 ; Return and pop 8 bytes from the stack

; void __naked __Lotus_local_unwind(
;	PUINT_PTR __cookie,
;	PEXCEPTION_REGISTRATION_RECORD __establisherFrame.
;	ULONG __targetLevel);
__Lotus_local_unwind:
	; Save non-volatile registers
	push ebx
	push esi
	push edi

	mov edx, dword[esp + 0x10] ; __cookie
	mov eax, dword[esp + 0x14] ; __establisherFrame
	mov ecx, dword[esp + 0x18] ; __targetLevel

	; Set up the exception handler to handle nested exceptions
	push ebp ; __framePointer
	push edx ; __cookie
	push eax ; __establisherFrame
	push ecx ; __targetLevel
	push ecx ; __targetLevel (becomes EH cookie)
	push __Lotus_unwind_handler

	push dword[fs: 0000h] ; Save the current exception handler

	; Generate EH cookie and overwrite last pushed ecx (__targetLevel)
	mov eax, dword[___security_cookie]
	xor eax, esp
	mov dword[esp + 0x8], eax

	mov dword[fs: 0000h], esp ; Install the new SEH handler

.unwind_top:
	; Capture local variables
	mov eax, dword[esp + 0x30] ; registrationNode
	mov ebx, dword[eax + 0x08] ; registrationNode->encodedScopeTable
	mov esi, dword[eax + 0x0C] ; registrationNode->tryLevel

	; Generate the EH cookie for this frame
	mov ecx, dword[esp + 0x2C] ; __cookie
	xor ebx, dword[ecx + 0x00] ; __cookie

	cmp esi, 0xFFFFFFFE        ; registrationNode->tryLevel == TOPMOST_TRY_LEVEL
	je .unwind_done

	; If the target level is the topmost try level, we don't need to check if
	; the current try level is less or equal to our target level.
	mov edx, dword[esp + 0x34] ; __targetLevel
	cmp edx, 0xFFFFFFFE        ; __targetLevel == TOPMOST_TRY_LEVEL
	je .skip

	cmp esi, edx               ; registrationNode->tryLevel <= __targetLevel
	jbe .unwind_done

.skip:
	; Calculate the address of the current scope table record.

	lea esi, [esi + esi * 0x2] ; registrationNode->tryLevel * 3
	; __cookie + registrationNode->tryLevel * 4 + 16
	lea ebx, [ebx + esi * 0x4 + 0x10]

	; Make the current exception frame points to the scope of the previous
	; __try block.
	mov ecx, dword[ebx]       ; scopeRecord->enclosingLevel
	; registrationNode->tryLevel = scopeRecord->enclosingLevel
	mov dword[eax + 0xC], ecx

	cmp dword[ebx + 0x4], 0   ; Determine if a __finally block exists.
	jne .unwind_top           ; There is no __finally block, go up a level.

	; Notify the debugger of the address of the __finally block.
	push 0x101
	mov eax, dword[ebx + 0x8]
	call __NLG_Notify

	; Call the __finally block.
	mov ecx, 1
	mov eax, dword[ebx + 0x8]
	call __NLG_Call

	jmp .unwind_top ; Continue unwinding.

.unwind_done:
	pop dword[fs: 0000h] ; Restore the old exception handler

	add esp, 24 ; Reclaim stack space

	; Restore non-volatile registers
	pop edi
	pop esi
	pop ebx

	ret

; This is the exception handler that gets called if another exception gets
; thrown during unwinding.
; EXCEPTION_DISPOSITION __stdcall __Lotus_unwind_handler(
;	PEXCEPTION_RECORD __exceptionRecord,
;	PVOID __establisherFrame,
;	PCONTEXT __contextRecord,
;	PVOID __dispatcherContext);
__Lotus_unwind_handler:
	; If the exception is in an unwind context, simply keep looking for handlers.
	mov ecx, dword[esp + 0x4] ; __exceptionRecord
	; __exceptionRecord->ExceptionFlags == EXCEPTION_UNWIND_CONTEXT
	test dword[ecx + 0x4], EXCEPTION_UNWIND_CONTEXT
	mov eax, ExceptionContinueSearch
	je .end

	; Check the stack for corruption.
	mov eax, dword[esp + 0x8] ; __establisherFrame (this is a frame pointer)
	mov ecx, dword[eax + 0x8] ; Frame's EH cookie
	xor ecx, eax
	call $@__security_check_cookie@4

	; We have a nested exception, we unwind the original exception
	; and let the kernel know the exception got collided with another one.
	push ebp                   ; Save current frame pointer
	mov ebp, dword[eax + 0x18] ; Overwrite the frame pointer with the old one.

	push dword[eax + 0x0C] ; __targetLevel
	push dword[eax + 0x10] ; __establisherFrame
	; (^ This is the PEXCEPTION_REGISTRATION_RECORD in __Lotus_local_unwind)
	push dword[eax + 0x14] ; __cookie
	call __Lotus_local_unwind ; Keep unwinding our original exception.

	add esp, 12
	pop ebp

	; Generate a "new" exception and let the kernel know it
	; collided with the first.
	mov eax, dword[esp + 0x08] ; __establisherFrame
	mov edx, dword[esp + 0x10] ; __dispatcherContext
	mov dword[edx], eax    ; __dispatcherContext.ControlPC = __establisherFrame
	mov eax, ExceptionCollidedUnwind

.end
	ret

; void _Noreturn __fastcall _Lotus_SEH_transfter_to_handler(
;	PEXCEPTION_HANDLER_X86 __handlerAddress,
;	PCHAR __framePointer);
$@_Lotus_SEH_transfer_to_handler@8:
	mov ebp, edx ; Overwrite the frame pointer with the given one
	mov esi, ecx ; __handlerAddress
	mov eax, ecx ; __handlerAddress

	push 1
	call __NLG_Notify

	; Clear general purpose registers
	xor eax, eax
	xor ebx, ebx
	xor ecx, ecx
	xor edx, edx
	xor edi, edi

	jmp esi ; tail-call the handler
