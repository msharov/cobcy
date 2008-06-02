// cobfunc.h
//
//	Defines cobol function library.
//

#ifndef COBFUNC_H_5F505570391DF1261B8F7F0537665407
#define COBFUNC_H_5F505570391DF1261B8F7F0537665407

#include "../dbf/dbf.h"
#include "../dbf/ndx.h"

//----------------------------------------------------------------------

int 	_Alphabetic (const char* str);
int 	_AlphabeticCase (const char* str, int what);
void 	_RuntimeError (const char* message);
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

//----------------------------------------------------------------------

#endif
