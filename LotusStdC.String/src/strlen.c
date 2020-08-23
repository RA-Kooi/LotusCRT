#include <string.h>

__LOTUSCRT_NO_BUILTIN(strlen)
__LOTUSCRT_MSVC_FUNCTION(strlen)
__LOTUSCRT_API size_t __LOTUSCRT_CDECL strlen(char const *__str)
{
	size_t len = 0;

	while(__str[len])
		++len;

	return len;
}
