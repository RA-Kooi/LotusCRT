#include <string.h>

__LOTUSCRT_NO_BUILTIN(memcmp)
__LOTUSCRT_MSVC_FUNCTION(memcmp)
__LOTUSCRT_API int __LOTUSCRT_CDECL memcmp(
	void const *__lhs,
	void const *__rhs,
	size_t __count)
{
	unsigned char const *lhs = __lhs;
	unsigned char const *rhs = __rhs;

	while(__count && *lhs == *rhs)
		(void)(--__count), (void)(++lhs), ++rhs;

	return __count ? *lhs - *rhs : 0;
}
