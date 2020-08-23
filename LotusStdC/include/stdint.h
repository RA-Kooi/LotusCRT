#ifndef __LOTUSCRT_STDINT_H__
#define __LOTUSCRT_STDINT_H__

#include <_lotuscrt.h>

__LOTUSCRT_SYSTEM_HEADER

#if defined(__LOTUSCRT_COMPILER_MSVC_CLANG) \
	|| defined(__LOTUSCRT_COMPILER_GNULIKE)
#include_next <stdint.h>
#elif defined(__LOTUSCRT_COMPILER_MSVC_ONLY)
// int8_t
typedef __int8 int8_t;

// uint8_t
typedef unsigned __int8 uint8_t;

// int16_t
typedef __int16 int16_t;

// uint16_t
typedef unsigned __int16 uint16_t;

// int32_t
typedef __int32 int32_t;

// uint32_t
typedef unsigned __int32 uint32_t;

// int64_t
typedef __int64 int64_t;

// uint64_t
typedef unsigned __int64 uint64_t;
#else
	#error "Unsupported compiler/platform"
#endif // __LOTUSCRT_COMPILER_MSVC_ONLY

#endif // __LOTUSCRT_STDINT_H__
