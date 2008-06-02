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
#include <stdbool.h>

#ifndef S_OK
#define S_OK			0
#define S_ERROR 		(-1)
#endif

#ifndef BYTE
typedef	unsigned char		BYTE;
typedef unsigned long int	WORD;
#endif

#define BYTE_MAX		UCHAR_MAX
#define WORD_MAX		ULONG_MAX

#endif

