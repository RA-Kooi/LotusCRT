#ifndef __LOTUSCRT_CONTEXT_VALIDATION_H__
#define __LOTUSCRT_CONTEXT_VALIDATION_H__

#include <_lotuscrt.h>

__LOTUSCRT_BEGIN_C_HEADER

struct _CONTEXT;
typedef struct _CONTEXT* PCONTEXT;

void _Lotus_validate_context_record(PCONTEXT const __contextRecord);

__LOTUSCRT_END_C_HEADER

#endif // __LOTUSCRT_CONTEXT_VALIDATION_H__
