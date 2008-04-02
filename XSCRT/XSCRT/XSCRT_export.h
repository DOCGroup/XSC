// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl XSCRT
// ------------------------------

#ifndef _XSCRT_EXPORT_H_
#define _XSCRT_EXPORT_H_

#if defined (XSCRT_AS_STATIC_LIBS) && !defined (XSCRT_HAS_DLL)
#  define XSCRT_HAS_DLL 0
#endif /* XSCRT_AS_STATIC_LIBS && XSCRT_HAS_DLL */

#if !defined (XSCRT_HAS_DLL)
#  define XSCRT_HAS_DLL 1
#endif /* ! XSCRT_HAS_DLL */

#if defined (XSCRT_HAS_DLL) && (XSCRT_HAS_DLL == 1)
#  if defined (XSCRT_BUILD_DLL)
#    define XSCRT_Export __declspec (dllexport)
#  else /* XSCRT_BUILD_DLL */
#    define XSCRT_Export __declspec (dllimport)
#  endif /* XSCRT_BUILD_DLL */
#else /* XSCRT_HAS_DLL == 1 */
#  define XSCRT_Export
#endif /* XSCRT_HAS_DLL == 1 */

#endif /* _XSCRT_EXPORT_H_ */

// End of auto generated file.
