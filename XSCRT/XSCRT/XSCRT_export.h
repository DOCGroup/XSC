#ifndef XSCRT_EXPORT_H
#define XSCRT_EXPORT_H

#if !defined(XSCRT_HAS_DLL)
# if defined(XSCRT_AS_STATIC_LIBS)
#   define XSCRT_HAS_DLL 0
# else
#   define XSCRT_HAS_DLL 1
# endif
#endif

#if (XSCRT_HAS_DLL == 1)
#  if defined(__SUNPRO_CC) && (__SUNPRO_CC >= 0x550)
#    if defined(XSCRT_BUILD_DLL)
#      define XSCRT_Export __symbolic
#    else
#      define XSCRT_Export __global
#    endif
#  elif defined(WIN32) || defined(UNDER_CE) || defined(__CYGWIN__)
#    if defined(XSCRT_BUILD_DLL)
#      define XSCRT_Export __declspec(dllexport)
#    else
#      define XSCRT_Export __declspec(dllimport)
#    endif
#  elif (defined(__GNUC__) && (__GNUC__ >= 4))
#    if defined(XSCRT_BUILD_DLL)
#      define XSCRT_Export __attribute__((visibility("default")))
#    else
#      define XSCRT_Export
#    endif
#  else
#    define XSCRT_Export
#  endif
#else
#  define XSCRT_Export
#endif

#endif
