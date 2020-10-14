#ifndef __LOTUSCRT_NT_HELPERS_H__
#define __LOTUSCRT_NT_HELPERS_H__

#include <_lotuscrt.h>

#include <stdint.h>
#include <stdbool.h>

__LOTUSCRT_BEGIN_C_HEADER

/* wrap msvc intrinsics onto gcc builtins */
#if defined(__LOTUSCRT_COMPILER_GNULIKE) || defined(__LOTUSCRT_COMPILER_MSVC_CLANG)
#undef  _ReturnAddress
#undef  _AddressOfReturnAddress
#define _ReturnAddress()		__builtin_return_address(0)
#define _AddressOfReturnAddress()	__builtin_frame_address (0)
#endif // __LOTUSCRT_COMPILER_GNULIKE

__LOTUSCRT_NORETURN void _Lotus_raise_hard_error(
	uint32_t __errorCode,
	bool __useStackCookie,
	uintptr_t __stackCookie);

__LOTUSCRT_END_C_HEADER

#endif // __LOTUSCRT_NT_HELPERS_H__
