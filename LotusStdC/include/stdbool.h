#ifndef __LOTUSCRT_STDBOOL_H__
#define __LOTUSCRT_STDBOOL_H__

#include <_lotuscrt.h>

__LOTUSCRT_SYSTEM_HEADER

#ifndef __cplusplus
	#if !(__LOTUSCRT_C_VERSION >= 11 || __LOTUSCRT_C_VERSION == 99)
		#define bool int
	#else
		#define bool _Bool
	#endif

	#define true  1
	#define false 0
#endif

#define __bool_true_false_are_defined 1

#endif // __LOTUSCRT_STDBOOL_H__
