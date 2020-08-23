#include <string.h>

__LOTUSCRT_NO_BUILTIN(memcpy)
__LOTUSCRT_MSVC_FUNCTION(memcpy)
__LOTUSCRT_API void * __LOTUSCRT_CDECL memcpy(
	void * __LOTUSCRT_RESTRICT __dest,
	void const * __LOTUSCRT_RESTRICT __src,
	size_t __count)
{
	unsigned char *dest = __dest;
	unsigned char const *src = __src;

	while(__count--)
		*dest++ = *src++;

	return __dest;
}
