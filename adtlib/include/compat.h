/* compat.h
**
**	Defines various macros for interplatform compatibility.
*/

#ifndef __COMPAT_H
#define __COMPAT_H

/* This is needed to export C functions. */
#ifndef EXTERNC
#if defined(__cplusplus) || defined(__cplusplus__) || defined(_cplusplus)
#   define EXTERNC	extern "C"
#else
#   define EXTERNC	extern 
#endif
#endif
                 
/* Borland C++ does not like constructors to have inline keyword.
** 	They still can be inlined though. Use: INLINE_CTOR constructor
*/
#if defined(__BORLANDC__)
#   define	INLINE_CTOR
#   define	__MSC	/* for full compatibility with Microsoft C  */
#else
#   define	INLINE_CTOR	inline
#endif                                                       

#endif

