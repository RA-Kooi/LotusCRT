#ifndef __LOTUSCRT_H__
#define __LOTUSCRT_H__

#include <_lotus_compiler.h>
#include <_lotus_compiler_traits.h>
#include <_lotus_config.h>
#include <_lotus_platform.h>

__LOTUSCRT_SYSTEM_HEADER

#ifdef __LOTUSCRT_COMPILER_MSVC
	#ifndef _VC_NODEFAULTLIB
		#error "Can't mix with MSVCRT, compile with /Zl"
	#endif

	#pragma comment(lib, "ntdll")
	#pragma comment(linker, "/defaultlib:libLotusCRT.lib")

	// Disallow Microsoft CRT
	// VS .NET 2003- /ML single threaded CRT
	#pragma comment(linker, "/disallowlib:libc.lib")
	#pragma comment(linker, "/disallowlib:libcd.lib")

	// VS .NET 2003- STL
	#pragma comment(linker, "/disallowlib:libcp.lib")
	#pragma comment(linker, "/disallowlib:libcpd.lib")

	// VS .NET 2002- iostream.h iostreams
	#pragma comment(linker, "/disallowlib:msvcirt.lib")
	#pragma comment(linker, "/disallowlib:msvcirtd.lib")

	// Static CRT
	#pragma comment(linker, "/disallowlib:libcmt.lib")
	#pragma comment(linker, "/disallowlib:libcmtd.lib")

	// Static STL
	#pragma comment(linker, "/disallowlib:libcpmt.lib")
	#pragma comment(linker, "/disallowlib:libcpmtd.lib")

	// Dynamic CRT
	#pragma comment(linker, "/disallowlib:msvcrt.lib")
	#pragma comment(linker, "/disallowlib:msvcrtd.lib")

	// Dynamic STL
	#pragma comment(linker, "/disallowlib:msvcprt.lib")
	#pragma comment(linker, "/disallowlib:msvcprtd.lib")

	// Managed CRT
	#pragma comment(linker, "/disallowlib:msvcmrt.lib")
	#pragma comment(linker, "/disallowlib:msvcmrtd.lib")

	// Pure managed CRT
	#pragma comment(linker, "/disallowlib:msvcurt.lib")
	#pragma comment(linker, "/disallowlib:msvcurtd.lib")

	// Static universal CRT
	#pragma comment(linker, "/disallowlib:libucrt.lib")
	#pragma comment(linker, "/disallowlib:libucrtd.lib")

	// Dynamic universal CRT
	#pragma comment(linker, "/disallowlib:ucrt.lib")
	#pragma comment(linker, "/disallowlib:ucrtd.lib")

	// Static C++ runtime
	#pragma comment(linker, "/disallowlib:libvcruntime.lib")
	#pragma comment(linker, "/disallowlib:libvcruntimed.lib")

	// Dynamic C++ runtime
	#pragma comment(linker, "/disallowlib:vcruntime.lib")
	#pragma comment(linker, "/disallowlib:vcruntimed.lib")
#endif

#endif // __LOTUSCRT_H__
