/* cobfunc.h
**
**	Defines cobol function library.
*/

#ifndef __COBFUNC_H
#define __COBFUNC_H

/*-------------------------------------------------------------*/
  int   _alphabetic (char * str);
  int   _alphab_up_low (char * str, int what);
  void 	_RuntimeError (char * message);
  void 	_AssignVar (char * var1, char * var2, int p1, int p2);
/*-------------------------------------------------------------*/

#endif

