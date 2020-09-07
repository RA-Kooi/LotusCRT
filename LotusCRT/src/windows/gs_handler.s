; This is x64 only

default rel

extern __security_check_cookie:proc

global __GSHandlerCheck
section .text

; EXCEPTION_DISPOSITION __fastcall _GSHandlerCheck(
;	PEXCEPTION_RECORD __exceptionRecord,
;	PVOID __establisherFrame,
;	PCONTEXT __contextRecord,
;	PDISPATCHER_CONTEXT __dispatcherContext);
__GSHandlerCheck:
	sub rsp, 0x28               ; Align stack and allocate shadow space

	mov r8, [r9 + 0x38]         ; __GSHandlerData
	mov rcx, rdx                ; __establisherFrame
	mov rdx, r9                 ; __dispatcherContext
	call __GSHandlerCheckCommon

	mov eax, 1                  ; EXCEPTION_EXECUTE_HANDLER
	add rsp, 0x28               ; Deallocate stack space

; void __fastcall __GSHandlerCheckCommon(
;	PVOID __establisherFrame,
;	PDISPATCHER_CONTEXT __dispatcherContext,
;	PGS_HANDLER_DATA __GSHandlerData);
__GSHandlerCheckCommon:
	push rbx      ; Save RBX (non-volatile)

	sub rsp, 0x20 ; Allocate shadow space

	mov r11d, dword[r8]  ; __GSHandlerData->CookieOffset
	mov rbx, rdx         ; __dispatcherContext
	mov r9, rcx          ; __establisherFrame

	and r11d, 0xFFFFFFF8 ; Remove flags from the cookie offset
	test byte[r8], 4     ; Check if the alignment flag has been set
	mov r10, rcx         ; __establisherFrame
	jz .jmp1

	mov eax, [r8 + 0x8]         ; __GSHandlerData->Alignment
	movsxd r10, dword[r8 + 0x4] ; __GSHandlerData->AlignedBaseOffset
	neg eax                     ; alignment = -alignment
	add r10, rcx                ; alignedBaseOffset += __establisherFrame
	movsxd rcx, eax             ; Generate alignment bitmask
	and r10, rcx                ; Apply the alignment bitmask to align the frame

.jmp1
	movsxd rax, r11d      ; SignExtend(cookieOffset)
	mov rdx, [rax + r10]  ; cookie = *(__establisherFrame + cookieOffset)
	mov rax, [rbx + 0x10] ; __dispatcherContext->FunctionEntry
	mov ecx, [rax + 0x8]  ; functionEntry->UnwindData
	add rcx, [rbx + 0x8]  ; __establisherFrame += __dispatcherContext->ImageBase

	test byte[rcx + 0x3], 0 ; Test the 4th 8 bits of the stack cookie
	jz .end

	; Not sure what's happening here, I think the stack cookie used to have
	; a different kind of value, so this part adjusts it in case we're dealing
	; with old code.
	movzx eax, byte[rcx + 0x3]
	and eax, 0xFFFFFFF0
	cdqe
	add r9, rax

.end
	xor r9, rdx   ; "Decrypt" the stack cookie
	mov rcx, r9   ; Move the cookie to rcx, required by __security_check_cookie

	add rsp, 0x20 ; Deallocate shadow space
	pop rbx       ; Restore RBX

	jmp __security_check_cookie ; Tail-call __security_check_cookie
