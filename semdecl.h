/* semdecl.h
**
**	Prototypes all the semantic actions in semdecl.cc
*/

#ifndef __SEMDECL_H
#define __SEMDECL_H

#include "adtlib/mdefs.h"

#define STACK_IDENT_LENGTH	50

/*------------------------------------------------*/
  void 	DeclareRecordLevel (void);
  void 	CloseScopeLevels (WORD LastLevel);
  void 	InitializeVariables (void);
  void	DeclareSpecialName (void);
/*------------------------------------------------*/

#endif

