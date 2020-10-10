; This is the load config used to configure the PE loader. This is needed for
; Control Flow Guard to work, and I assume it helps with process hardening
; in general. Implemented in assembly because clang doesn't like some
; truncation going on in the C code (initializer element is not a compile time
; constant). It obviously works in MSVC, but I think they detect it as a
; special case. Also yes, you're indeed looking at almost 300 lines of assembly
; code just to fill out a struct, isn't cross platform (bitness?) asm wonderful?

%ifidn __OUTPUT_FORMAT__, win64
	default rel

	%define sword qword
	%define ress resq
	%define dss dq ; ds is a segment
%else
	%define sword dword
	%define ress resd
	%define dss dd ; ds is a segment
%endif

; All these names are either linker generated, or looked for by the linker.
; As such we can't slap a Lotus branding on them to differ from MSVC CRT.
%ifidn __OUTPUT_FORMAT__, win32
	extern ___security_cookie:sword
	extern ___enclave_config:sword
	extern ___volatile_metadata:sword

	extern ___safe_se_handler_table:dword
	extern ___safe_se_handler_count:byte

	extern ___guard_check_icall_fptr:proc
	extern ___guard_xfg_check_icall_fptr:proc
	extern ___guard_fids_table:sword
	extern ___guard_fids_count:sword
	extern ___guard_flags:dword
	extern ___guard_iat_table:sword
	extern ___guard_iat_count:sword
	extern ___guard_longjmp_table:sword
	extern ___guard_longjmp_count:sword
	extern ___guard_eh_cont_table:sword
	extern ___guard_eh_cont_count:sword

	%define __security_cookie            ___security_cookie
	%define __enclave_config             ___enclave_config
	%define __volatile_metadata          ___volatile_metadata

	%define __guard_check_icall_fptr     ___guard_check_icall_fptr
	%define __guard_xfg_check_icall_fptr ___guard_xfg_check_icall_fptr
	%define __guard_fids_table           ___guard_fids_table
	%define __guard_fids_count           ___guard_fids_count
	%define __guard_flags                ___guard_flags
	%define __guard_iat_table            ___guard_iat_table
	%define __guard_iat_count            ___guard_iat_count
	%define __guard_longjmp_table        ___guard_longjmp_table
	%define __guard_longjmp_count        ___guard_longjmp_count
	%define __guard_eh_cont_table        ___guard_eh_cont_table
	%define __guard_eh_cont_count        ___guard_eh_cont_count
%else
	extern __security_cookie:sword
	extern __enclave_config:sword
	extern __volatile_metadata:sword

	extern __guard_check_icall_fptr:proc
	extern __guard_xfg_check_icall_fptr:proc
	extern __guard_fids_table:sword
	extern __guard_fids_count:sword
	extern __guard_flags:dword
	extern __guard_iat_table:sword
	extern __guard_iat_count:sword
	extern __guard_longjmp_table:sword
	extern __guard_longjmp_count:sword
	extern __guard_eh_cont_table:sword
	extern __guard_eh_cont_count:sword
%endif

%ifidn __OUTPUT_FORMAT__, win64
	extern __guard_dispatch_icall_fptr:proc
	extern __guard_xfg_dispatch_icall_fptr:proc
	extern __guard_xfg_table_dispatch_icall_fptr:proc
%endif

struc IMAGE_LOAD_CONFIG_CODE_INTEGRITY
.flags:         resw 1
.catalog:       resw 1
.catalogOffset: resd 1
.reserved:      resd 1
endstruc

; https://docs.microsoft.com/en-us/windows/win32/debug/pe-format#the-load-configuration-structure-image-only
struc load_config
.characteristics:                    resd 1

.timestamp:                          resd 1

.majorVersion:                       resw 1
.minorVersion:                       resw 1

.globalFlagsClear:                   resd 1
.globalFlagsSet:                     resd 1

.criticalSectionDefaultTimeout:      resd 1

.decommitFreeBlockTreshold:          ress 1
.decommitTotalFreeTreshold:          ress 1

.lockPrefixTable:                    ress 1

.maxAllocationSize:                  ress 1
.virtualMemTreshold:                 ress 1

.processAffinityMask:                ress 1
.processHeapFlags:                   resd 1

.CDSVersion:                         resw 1

.reserved:                           resw 1
.editList:                           ress 1 ; reserved

.securityCookie:                     ress 1

; SAFESEH (x86 only)
.SEHandlerTable:                     ress 1
.SEHandlerCount:                     ress 1

; Controlflow guard; /guard:cf and /GUARD:CF
.guardCFCheckFunction:               ress 1
.guardCFDispatchFunction:            ress 1
.guardCFFunctionTable:               ress 1
.guardCFFunctionCount:               ress 1
.guardFlags:                         resd 1

.codeIntegrity: resb IMAGE_LOAD_CONFIG_CODE_INTEGRITY_size

; Guard support, not sure what kind.
.guardAddressTakenIATEntryTable:     ress 1
.guardAddressTakenIATEntryCount:     ress 1
.guardLongJumpTargetTable:           ress 1
.guardLongJumpTargetCount:           ress 1

; This is not present on MSDN, but is present in MSVC CRT sources
; and virtualbox sources. Plz no sue :pray:
.dynamicValueRelocationTable:        ress 1
.CHPEMetadataPointer:                ress 1

; Returnflow guard
.guardRFFailureRoutine:              ress 1
.guardRFFailureRoutineFunction:      ress 1

.dynamicValueRelocationTableOffset:  resd 1
.dynamicValueRelocationTableSection: resw 1

.reserved2:                          resw 1

; Returnflow guard
.guardRFVerifyStackPointerFunction:  ress 1

.hotPatchTableOffset:                resd 1

.reserved3:                          resd 1

.enclaveConfigPtr:                   ress 1

.volatileMetadataPtr:                ress 1

; EH guard /guard:ehcont
.guardEHContinuationTable:           ress 1
.guardEHContinuationCount:           ress 1

; Guard support, not sure what kind.
.guardXFGCheckFunction:              ress 1
.guardXFGDispatchFunction:           ress 1
.guardXFGTableDispatchFunction:      ress 1
endstruc

section .rdata

; The name here matches what you would find in the MSVC CRT, which makes sense
; as it is a symbol the linker looks for, so we have to preserve its name.
%ifidn __OUTPUT_FORMAT__, win64
global _load_config_used

_load_config_used:
%else
global __load_config_used

__load_config_used:
%endif
	istruc load_config
		at load_config.characteristics,                    dd load_config_size

		at load_config.timestamp,                          dd 0

		at load_config.majorVersion,                       dw 0
		at load_config.minorVersion,                       dw 0

		at load_config.globalFlagsClear,                   dd 0
		at load_config.globalFlagsSet,                     dd 0

		at load_config.criticalSectionDefaultTimeout,      dd 0

		at load_config.decommitFreeBlockTreshold,          dss 0
		at load_config.decommitTotalFreeTreshold,          dss 0

		at load_config.lockPrefixTable,                    dss 0

		at load_config.maxAllocationSize,                  dss 0
		at load_config.virtualMemTreshold,                 dss 0

		at load_config.processAffinityMask,                dss 0
		at load_config.processHeapFlags,                   dd 0

		at load_config.CDSVersion,                         dw 0

		at load_config.reserved,                           dw 0
		at load_config.editList,                           dss 0

		at load_config.securityCookie,                     dss __security_cookie

%ifidn __OUTPUT_FORMAT__, win32
		at load_config.SEHandlerTable,                     dd ___safe_se_handler_table
		at load_config.SEHandlerCount,                     dd ___safe_se_handler_count
%else
		at load_config.SEHandlerTable,                     dd 0
		at load_config.SEHandlerCount,                     dd 0
%endif

		at load_config.guardCFCheckFunction,               dss __guard_check_icall_fptr
%ifidn __OUTPUT_FORMAT__, win64
		at load_config.guardCFDispatchFunction,            dq __guard_dispatch_icall_fptr
%else
		at load_config.guardCFDispatchFunction,            dd 0
%endif
		at load_config.guardCFFunctionTable,               dss __guard_fids_table
		at load_config.guardCFFunctionCount,               dss __guard_fids_count
		at load_config.guardFlags,                         dd __guard_flags

		at load_config.codeIntegrity,                      dw 0, 0
			db 0, 0

		at load_config.guardAddressTakenIATEntryTable,     dss __guard_iat_table
		at load_config.guardAddressTakenIATEntryCount,     dss __guard_iat_count
		at load_config.guardLongJumpTargetTable,           dss __guard_longjmp_table
		at load_config.guardLongJumpTargetCount,           dss __guard_longjmp_count

		at load_config.dynamicValueRelocationTable,        dss 0
		at load_config.CHPEMetadataPointer,                dss 0

		at load_config.guardRFFailureRoutine,              dss 0
		at load_config.guardRFFailureRoutineFunction,      dss 0

		at load_config.dynamicValueRelocationTableOffset,  dd 0
		at load_config.dynamicValueRelocationTableSection, dw 0

		at load_config.reserved2,                          dw 0

		at load_config.guardRFVerifyStackPointerFunction,  dss 0

		at load_config.hotPatchTableOffset,                dd 0

		at load_config.reserved3,                          dd 0

		at load_config.enclaveConfigPtr,                   dss __enclave_config

		at load_config.volatileMetadataPtr,                dss __volatile_metadata

		at load_config.guardEHContinuationTable,           dss __guard_eh_cont_table
		at load_config.guardEHContinuationCount,           dss __guard_eh_cont_count

%ifidn __OUTPUT_FORMAT__, win64
		at load_config.guardXFGCheckFunction,              dq __guard_xfg_check_icall_fptr
		at load_config.guardXFGDispatchFunction,           dq __guard_xfg_dispatch_icall_fptr
		at load_config.guardXFGTableDispatchFunction,      dq __guard_xfg_table_dispatch_icall_fptr
%else
		at load_config.guardXFGCheckFunction,              db 0
		at load_config.guardXFGDispatchFunction,           db 0
		at load_config.guardXFGTableDispatchFunction,      db 0
%endif
	iend
