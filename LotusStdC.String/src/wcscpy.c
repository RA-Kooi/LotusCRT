#include <wchar.h>

#include <string.h>

__LOTUSCRT_MSVC_FUNCTION(wcscpy)
__LOTUSCRT_API wchar_t * __LOTUSCRT_CDECL wcscpy(
	wchar_t * __LOTUSCRT_RESTRICT __dest,
	wchar_t const * __LOTUSCRT_RESTRICT __src)
{
	memcpy(__dest, __src, wcslen(__src));
	return __dest;
}
