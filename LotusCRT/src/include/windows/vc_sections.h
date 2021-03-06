#ifndef __LOTUSCRT_VC_SECTIONS_H__
#define __LOTUSCRT_VC_SECTIONS_H__

#include <_lotuscrt.h>

__LOTUSCRT_BEGIN_C_HEADER

__LOTUSCRT_DECLARE_SECTION(.CRT$XIA,  long, read) // First C global constructor
__LOTUSCRT_DECLARE_SECTION(.CRT$XIAA, long, read) // CRT C global constructor
__LOTUSCRT_DECLARE_SECTION(.CRT$XIZ,  long, read) // Last C global constructor

__LOTUSCRT_DECLARE_SECTION(.00cfg, read) // Guard support table

typedef void (*_PVFV)(void);
typedef int (*_PIFV)(void);

// First C global constructor
extern __LOTUSCRT_SECTION(.CRT$XIA) _PIFV __xi_a[];

// Last C global constructor
extern __LOTUSCRT_SECTION(.CRT$XIZ) _PIFV __xi_z[];

int _Lotus_do_global_constructors(_PIFV *__begin, _PIFV *__end);

__LOTUSCRT_END_C_HEADER

#endif // __LOTUSCRT_VC_SECTIONS_H__
