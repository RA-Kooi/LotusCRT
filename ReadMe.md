LotusCRT is an open source libc and C runtime.  
The project is licensed under a to be determined permissive license.

The project aims to be a replacement for mingw-w64-crt on Windows, and provide compatibility with MSVC, Clang, and GCC.  
Apart from Windows, the project also aims to support other platforms such as Linux and Windows Kernel Mode.  

Other goals are to provide extensions to the C standard library, such as:

* Making re-entrant versions of functions where possible. (Not formally bounds checked like Annex-K)
* Fixes to locale usage.
* Low level control over memory, allowing the user to install pre and post callbacks to memory allocation functions.
* Locale-less versions of string manipulation functions for safety critical and embedded use.
* UTF-8 aware string manipulation versions.
* Configurable builds with presets for ANSI/ISO C, desktop, safety critical, embedded, and Windows Kernel Mode.

The default build will be a fully fledged ANSI/ISO libc and CRT.
