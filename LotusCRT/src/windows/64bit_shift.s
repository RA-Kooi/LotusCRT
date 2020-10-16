; Implement 64bit shift operations for 32 bit.
; Needed for MSVC, clang somehow has these already.

global __allshl
global __aullshr

; 64 bit left shift
__allshl:
	cmp cl, 64    ; Shift is 64 bits or more?
	jae .zero_all ; Yes, set all bits to 0

	cmp cl, 32       ; Shift is between 32 or 64 bits?
	jae .shift_upper ; Yes, shift upper bits

	; No, shift lower bits
	shld edx, eax, cl ; Shift eax to the left, preserving bits > 31 in edx
	shl eax, cl       ; Shift eax to the left, preserving no bits
	ret

.shift_upper
	mov edx, eax ; Copy eax (lower) to edx (upper)
	xor eax, eax ; Zero out eax
	and cl, 31   ; Get remaining > 31 shifts
	shl edx, cl  ; Shift upper bits
	ret

.zero_all
	xor eax, eax ; Zero lower 32 bits
	xor edx, edx ; Zero upper 32 bits
	ret

; 64 bit unsigned right shift
__aullshr:
	cmp cl, 64    ; Shift is 64 bits or more?
	jae .zero_all ; Yes, set all bits to 0

	cmp cl, 32       ; Shift is between 32 or 64 bits?
	jae .shift_lower ; Yes, shift lower bits

	; No, shift lower bits
	shrd eax, edx, cl ; Shift edx to the right, preserving bits < 0 in eax
	shr edx, cl       ; Shift edx to the right, preserving no bits
	ret

.shift_lower
	mov eax, edx ; Copy edx (upper) to eax (lower)
	xor edx, edx ; Zero out edx
	and cl, 31   ; Get remaining > 31 shifts
	shr eax, cl  ; Shift lower bits
	ret

.zero_all
	xor eax, eax ; Zero lower 32 bits
	xor edx, edx ; Zero upper 32 bits
	ret
