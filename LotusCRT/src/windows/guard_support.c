#include "guard_support.h"

#include <stdint.h>

#include <phnt_windows.h>
#include <phnt.h>

#include "vc_sections.h"

// Names generared by the MS linker.
extern void *volatile __guard_xfg_check_icall_fptr;
extern void *         __guard_fids_table[];
extern size_t         __guard_fids_count;
extern uint32_t       __guard_flags;
extern void *         __guard_iat_table[];
extern size_t         __guard_iat_count;
extern void *         __guard_longjmp_table[];
extern size_t         __guard_longjmp_count;
extern void *         __guard_eh_cont_table[];
extern size_t         __guard_eh_cont_count;

#ifdef __LOTUSCRT_ARCH_X64
extern void const *__guard_dispatch_icall_fptr;
extern void const *__guard_xfg_dispatch_icall_fptr;
extern void const *__guard_xfg_table_dispatch_icall_fptr;

extern const unsigned char _Lotus_guard_dispatch_call_nop;
extern const unsigned char _Lotus_guard_xfg_dispatch_call_nop;
#endif

__LOTUSCRT_SECTION(.00cfg)
__LOTUSCRT_SELECTANY
void *volatile __guard_check_icall_fptr = (void*)&_Lotus_guard_check_call_nop;

__LOTUSCRT_SECTION(.00cfg)
__LOTUSCRT_SELECTANY
void *volatile __guard_xfg_check_icall_fptr =
	(void*)&_Lotus_guard_check_call_nop;

#ifdef __LOTUSCRT_ARCH_X64
__LOTUSCRT_SECTION(.00cfg)
__LOTUSCRT_SELECTANY
void const *__guard_dispatch_icall_fptr =
	(void const*)&_Lotus_guard_dispatch_call_nop;

__LOTUSCRT_SECTION(.00cfg)
__LOTUSCRT_SELECTANY
void const *__guard_xfg_dispatch_icall_fptr =
	(void const*)&_Lotus_guard_xfg_dispatch_call_nop;

__LOTUSCRT_SECTION(.00cfg)
__LOTUSCRT_SELECTANY
void const *__guard_xfg_table_dispatch_icall_fptr =
	(void const*)&_Lotus_guard_xfg_dispatch_call_nop;
#endif

#ifdef __LOTUSCRT_ARCH_X64
extern inline int __LOTUSCRT_CDECL _guard_rf_checks_enforced(void)
{
	return 0;
}
#endif
