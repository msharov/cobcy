/* seminit.h
**
**	Defines initialization actions for COBOL compiler.
*/

#ifndef __SEMINIT_H
#define __SEMINIT_H

/*-------------------------------------------------*/
  void 	InitializeVariables (void);
  void 	FinishDecl (void);
  void 	StartCode (void);
  void 	EndCode (void);
  void 	StartProgram (void);
  void 	EndProgram (void);
  void  NIY (char * str);
  void  Comment (char * str);
/*-------------------------------------------------*/

#endif

