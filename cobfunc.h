/* cobfunc.h
**
**	Defines cobol function library.
*/

#ifndef __COBFUNC_H
#define __COBFUNC_H

/*-------------------------------------------------------------------*/
  int 	_alphabetic (char * str);
  int 	_alphab_up_low(char * str, int what);
  void 	_RuntimeError (char * message);
  void 	_ReadStringVar (FILE * stream, char * var, char * pic);
  void 	_ReadIntegerVar (FILE * stream, long int var, char * pic);
  void 	_ReadFloatVar (FILE * stream, double var, char * pic);
  void 	_WriteStringVar (FILE * stream, char * var, char * pic);
  void 	_WriteIntegerVar (FILE * stream, long int var, char * pic);
  void 	_IntegerToString (char * string, long int var, char * pic);
  void 	_WriteFloatVar (FILE * stream, double var, char * pic);
  void 	_FloatToString (char * string, double var, char * pic);
  void 	_AssignVarString (char * var1, char * var2, int p1, int p2);
  long int 	_RoundResult (double num);
/*-------------------------------------------------------------------*/

#endif

