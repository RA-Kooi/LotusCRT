#ifndef __LOTUSCRT_STRING_H__
#define __LOTUSCRT_STRING_H__

#include <_lotuscrt.h>

#include <stddef.h>

__LOTUSCRT_SYSTEM_HEADER

#ifdef __cplusplus
extern "C"
{
#endif
__LOTUSCRT_MSVC_INTRINSIC(memset)
__LOTUSCRT_API void * __LOTUSCRT_CDECL memset(
	void *__dest,
	int __value,
	size_t __count);
#ifdef __cplusplus
}
#endif

#endif // __LOTUSCRT_STRING_H__
