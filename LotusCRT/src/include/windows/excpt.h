#ifndef __LOTUSCRT_EXCPT_H__
#define __LOTUSCRT_EXCPT_H__

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum _EXCEPTION_DISPOSITION
{
	ExceptionContinueExecution = 0,
	ExceptionContinueSearch    = 1,
	ExceptionNestedException   = 2,
	ExceptionCollidedUnwind    = 3
} EXCEPTION_DISPOSITION;

#define EXCEPTION_EXECUTE_HANDLER 1
#define EXCEPTION_CONTINUE_SEARCH 0
#define EXCEPTION_CONTINUE_EXECUTION (-1)

#define GetExceptionCode        _exception_code
#define GetExceptionInformation (struct _EXCEPTION_POINTERS*)_exception_info

unsigned long __cdecl _exception_code(void);
void *        __cdecl _exception_info(void);

#ifdef __cplusplus
}
#endif

#endif // __LOTUSCRT_EXCPT_H__
