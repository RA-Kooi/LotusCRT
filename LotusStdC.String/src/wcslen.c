#include <wchar.h>

__LOTUSCRT_NO_BUILTIN(wcslen)
__LOTUSCRT_MSVC_FUNCTION(wcslen)
__LOTUSCRT_API size_t __LOTUSCRT_CDECL wcslen(wchar_t const *__str)
{
	size_t len = 0;

	while(__str[len])
		++len;

	return len;
}
