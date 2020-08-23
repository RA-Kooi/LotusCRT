#include <wchar.h>

__LOTUSCRT_API int __LOTUSCRT_CDECL _wcsicmp(
	wchar_t const *__lhs,
	wchar_t const *__rhs)
{
	while(__lhs && __rhs)
	{
		if(*__lhs != *__rhs)
			break;

		(void)(__lhs++), __rhs++;
	}

	return *__lhs - *__rhs;
}
