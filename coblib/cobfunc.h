// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// Cobol function library.

#pragma once
#include "dbf.h"
#include "ndx.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define UNUSED	__attribute__((unused))

#ifndef min
#define min(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)	((a) < (b) ? (b) : (a))
#endif

extern char _space_var [201];
extern long _zero_var;
extern long _index;

void*	_AllocateBytes (size_t n) __attribute__((malloc));
void*	_ReallocateBytes (void* p, size_t n) __attribute__((malloc));
void	_InstallSignalHandlers (void);
bool 	_Alphabetic (const char* str);
bool 	_AlphabeticCase (const char* str, int what);
void 	_RuntimeError (const char* message, ...) __attribute__((__format__(__printf__,1,2))) __attribute__((noreturn));
void 	_ReadStringVar (FILE * stream, char* var, const char* pic);
void 	_ReadIntegerVar (FILE * stream, long int * var, const char* pic);
void 	_ReadFloatVar (FILE * stream, double * var, const char* pic);
void 	_WriteStringVar (FILE * stream, const char* var, const char* pic);
void 	_WriteIntegerVar (FILE * stream, long int var, const char* pic);
void 	_WriteFloatVar (FILE * stream, double var, const char* pic);
const char* _IntegerToString (long int var, const char* pic);
const char* _FloatToString (double var, const char* pic);
long	_StringToInteger (const char* string);
double	_StringToFloat (const char* string);
void	_AssignVarString (char* var1, const char* var2, int p1, int p2);
void	_OpenSequentialFile (FILE ** fp, const char* filename, const char* mode);
void	_OpenRelativeFile (DBF_FILE** fp, const char* filename, const char* sig, const char* mode);
void	_OpenIndexedFile (DBF_FILE** fpd, const char* filename, const char* sig, const char* mode);
void	_CloseSequentialFile (FILE* fp);
void	_CloseRelativeFile (DBF_FILE** fp);
void	_CloseIndexedFile (DBF_FILE** fpd);
void	_OpenIndexFile (NDX_FILE** ifd, const char* filename, const char* sig, const char* mode);
void	_CloseIndexFile (NDX_FILE** ifd);
void	_SeekRelativeFile (DBF_FILE* fp, long int record);
void	_SeekIndexedFile (DBF_FILE* fpd, NDX_FILE* fpi, const char* key);
double 	_RoundResult (double num, const char* pic);
