/* cobfunc.h
**
**	Defines cobol function library.
*/

#ifndef __COBFUNC_H
#define __COBFUNC_H

/*-------------------------------------------------------------------*/
  int 		_Alphabetic (char * str);
  int 		_AlphabeticCase (char * str, int what);
  void 		_RuntimeError (char * message);
  void 		_ReadStringVar (FILE * stream, char * var, char * pic);
  void 		_ReadIntegerVar (FILE * stream, long int var, char * pic);
  void 		_ReadFloatVar (FILE * stream, double var, char * pic);
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
  void		_OpenRelativeFile (FILE ** fp, char * filename, char * mode);
  void		_OpenIndexedFile (FILE ** fpd, FILE ** fpi, 
  				  char * filename, char * mode);
  void		_CloseSequentialFile (FILE * fp);
  void		_CloseRelativeFile (FILE * fp);
  void		_CloseIndexedFile (FILE * fpd, FILE * fpi);
  void		_SeekRelativeFile (FILE * fp, long int offset);
  void		_SeekIndexedFile (FILE * fpd, FILE * fpi, char * key);
  double 	_RoundResult (double num, char * pic);
/*-------------------------------------------------------------------*/

#endif

