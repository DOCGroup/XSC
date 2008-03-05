// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl XSCRT_UTILS
// ------------------------------

#ifndef _XSCRT_UTILS_EXPORT_H_
#define _XSCRT_UTILS_EXPORT_H_

#if defined (XSCRT_UTILS_AS_STATIC_LIBS) && !defined (XSCRT_UTILS_HAS_DLL)
#  define XSCRT_UTILS_HAS_DLL 0
#endif /* XSCRT_UTILS_AS_STATIC_LIBS && XSCRT_UTILS_HAS_DLL */

#if !defined (XSCRT_UTILS_HAS_DLL)
#  define XSCRT_UTILS_HAS_DLL 1
#endif /* ! XSCRT_UTILS_HAS_DLL */

#if defined (XSCRT_UTILS_HAS_DLL) && (XSCRT_UTILS_HAS_DLL == 1)
#  if defined (XSCRT_UTILS_BUILD_DLL)
#    define XSCRT_UTILS_Export __declspec (dllexport)
#  else /* XSCRT_UTILS_BUILD_DLL */
#    define XSCRT_UTILS_Export __declspec (dllimport)
#  endif /* XSCRT_UTILS_BUILD_DLL */
#else /* XSCRT_UTILS_HAS_DLL == 1 */
#  define XSCRT_UTILS_Export
#endif /* XSCRT_UTILS_HAS_DLL == 1 */

#endif /* _XSCRT_UTILS_EXPORT_H_ */

// End of auto generated file.
