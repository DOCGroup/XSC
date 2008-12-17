#ifndef XSC_UTILS_EXPORT_H
#define XSC_UTILS_EXPORT_H

#if !defined(XSC_UTILS_HAS_DLL)
# if defined(XSC_UTILS_AS_STATIC_LIBS)
#   define XSC_UTILS_HAS_DLL 0
# else
#   define XSC_UTILS_HAS_DLL 1
# endif
#endif

#if (XSC_UTILS_HAS_DLL == 1)
#  if defined(__SUNPRO_CC) && (__SUNPRO_CC >= 0x550)
#    if defined(XSC_UTILS_BUILD_DLL)
#      define XSC_UTILS_Export __symbolic
#    else
#      define XSC_UTILS_Export __global
#    endif
#  elif defined(WIN32) || defined(UNDER_CE) || defined(__CYGWIN__)
#    if defined(XSC_UTILS_BUILD_DLL)
#      define XSC_UTILS_Export __declspec(dllexport)
#    else
#      define XSC_UTILS_Export __declspec(dllimport)
#    endif
#  elif (defined(__GNUC__) && (__GNUC__ >= 4))
#    if defined(XSC_UTILS_BUILD_DLL)
#      define XSC_UTILS_Export __attribute__((visibility("default")))
#    else
#      define XSC_UTILS_Export
#    endif
#  else
#    define XSC_UTILS_Export
#  endif
#else
#  define XSC_UTILS_Export
#endif

#endif
