#ifndef __LOTUSCRT_STRING_H__
#define __LOTUSCRT_STRING_H__

#include <_lotuscrt.h>

#include <stddef.h>

__LOTUSCRT_SYSTEM_HEADER

#ifdef __LOTUSCRT_PLATFORM_WINDOWS
#define __INC_STRING // Windows SDK compat
#endif

#ifdef __cplusplus
extern "C"
{
#endif
__LOTUSCRT_API void * __LOTUSCRT_CDECL memset(
	void *__dest,
	int __value,
	size_t __count);
__LOTUSCRT_MSVC_INTRINSIC(memset)
#ifdef __cplusplus
}
#endif

#endif // __LOTUSCRT_STRING_H__
