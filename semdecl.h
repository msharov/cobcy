/* semdecl.h
**
**	Prototypes all the semantic actions in semdecl.cc
*/

#ifndef __SEMDECL_H
#define __SEMDECL_H

#define STACK_IDENT_LENGTH	50

/*------------------------------------------------*/
  void 	DeclareRecordLevel (void);
  void 	CloseScopeLevels (unsigned int LastLevel);
  void 	InitializeVariables (void);
/*------------------------------------------------*/

#endif

