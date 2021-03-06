#pragma once

#if defined(__urn_begin)
  #error This header must be included first
#endif

#if defined(_WIN32) || defined(_WIN64) // {{{1
  #define WIN32_NO_STATUS
  #include <windows.h>
  #undef WIN32_NO_STATUS
  #include <winternl.h>
  #include <ntstatus.h>
  #pragma comment(lib, "ntdll")
#endif // }}}1
