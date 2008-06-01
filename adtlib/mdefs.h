/* mdefs.h
**
**	Miscellaneous definitions for use in other files.
** defines some useful types and macros (like min and max)
*/

#ifndef __MDEFS_H
#define __MDEFS_H

#include <stdlib.h>
#include <stddef.h>
#include <limits.h>
#include <assert.h>
#include "compat.h"

#ifndef NeedFunctionPrototypes
#   define NeedFunctionPrototypes		1
#endif

#ifndef TRUE
#   define TRUE			1
#   define FALSE		0
#endif				 

#ifndef S_OK
#define S_OK			0
#define S_ERROR 		(-1)
#endif

#ifndef BYTE
typedef	unsigned char		BYTE;
typedef unsigned long int	WORD;
#endif
#ifndef BOOL
typedef BYTE			BOOL;
#endif

#define BYTE_MAX		UCHAR_MAX
#define WORD_MAX		ULONG_MAX

/*	In plain C define min and max as macros.
**	In C++ define them as templates for additional type checking.
*/    
#ifndef __cplusplus
#   include "macros.h"
#else
#   include "templats.h"
#endif

#endif

