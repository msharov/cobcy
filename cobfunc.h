/* cobfunc.h
**
**	Defines cobol function library.
*/

#ifndef __COBFUNC_H
#define __COBFUNC_H

#include "dbf/dbf.h"
#include "dbf/ndx.h"

/*----------------------------------------------------------------------------*/
  int 		_Alphabetic (char * str);
  int 		_AlphabeticCase (char * str, int what);
  void 		_RuntimeError (char * message);
  void 		_ReadStringVar (FILE * stream, char * var, char * pic);
  void 		_ReadIntegerVar (FILE * stream, long int * var, char * pic);
  void 		_ReadFloatVar (FILE * stream, double * var, char * pic);
  void 		_WriteStringVar (FILE * stream, char * var, char * pic);
  void 		_WriteIntegerVar (FILE * stream, long int var, char * pic);
  void 		_WriteFloatVar (FILE * stream, double var, char * pic);
  char * 	_IntegerToString (long int var, char * pic);
  char * 	_FloatToString (double var, char * pic);
  long int	_StringToInteger (char * string);
  double	_StringToFloat (char * string);
  void 		_AssignVarString (char * var1, char * var2, 
  				  int p1, int p2);
  void		_OpenSequentialFile (FILE ** fp, char * filename, char * mode);
  void		_OpenRelativeFile (DBF_FILE ** fp, char * filename, 
  				   char * sig, char * mode);
  void		_OpenIndexedFile (DBF_FILE ** fpd, char * filename, 
  				  char * sig, char * mode);
  void		_CloseSequentialFile (FILE * fp);
  void		_CloseRelativeFile (DBF_FILE ** fp);
  void		_CloseIndexedFile (DBF_FILE ** fpd);
  void		_OpenIndexFile (NDX_FILE ** ifd, char * filename, 
  				  char * sig, char * mode);
  void		_CloseIndexFile (NDX_FILE ** ifd);
  void		_SeekRelativeFile (DBF_FILE * fp, long int record);
  void		_SeekIndexedFile (DBF_FILE * fpd, NDX_FILE * fpi, char * key);
  double 	_RoundResult (double num, char * pic);
/*----------------------------------------------------------------------------*/

#endif

