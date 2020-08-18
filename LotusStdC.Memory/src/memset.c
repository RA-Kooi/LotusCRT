#include <string.h>

__LOTUSCRT_NO_BUILTIN(memset)
__LOTUSCRT_MSVC_FUNCTION(memset)
__LOTUSCRT_API void * __LOTUSCRT_CDECL memset(
	void *__dest,
	int __value,
	size_t __count)
{
	unsigned char *dest = __dest;
	unsigned char value = (unsigned char)__value;

	for(size_t i = 0; i < __count; ++i)
	{
		dest[i] = value;
	}

	return __dest;
}
