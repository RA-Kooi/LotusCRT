#ifndef __LOTUSCRT_WCHAR_H__
#define __LOTUSCRT_WCHAR_H__

#include <_lotuscrt.h>

#include <stddef.h>

__LOTUSCRT_SYSTEM_HEADER

#ifdef __LOTUSCRT_PLATFORM_WINDOWS
#define __INC_WCHAR // Windows SDK compat
#endif

#ifdef __cplusplus
extern "C"
{
#endif
__LOTUSCRT_API size_t __LOTUSCRT_CDECL wcslen(wchar_t const *__str);
__LOTUSCRT_MSVC_INTRINSIC(wcslen)

#ifdef __LOTUSCRT_PLATFORM_WINDOWS
	__LOTUSCRT_API int __LOTUSCRT_CDECL _wcsicmp(
		wchar_t const *__lhs,
		wchar_t const *__rhs);
#endif
#ifdef __cplusplus
}
#endif

#endif // __LOTUSCRT_WCHAR_H__
