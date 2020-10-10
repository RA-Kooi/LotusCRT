; This is x64 only

default rel

extern __guard_dispatch_icall_fptr:proc

global _Lotus_guard_dispatch_call_nop
global _Lotus_guard_xfg_dispatch_call_nop

section .text

_Lotus_guard_dispatch_call_nop:
	jmp rax ; Tailcall the generated dispatcher

_Lotus_guard_xfg_dispatch_call_nop:
	jmp [__guard_dispatch_icall_fptr] ; Tailcall the regular dispatcher
