/* semutil.h
**
**	Prototypes semantic utilities for COBOL compiler.
*/

#ifndef __SEMUTIL_H
#define __SEMUTIL_H

#include <fstream.h>
#include <mdefs.h>
#include "semsedef.h"
#include "symbase.h"

/*-----------------------------------------------------------------------*/
  void 	WriteError (char * str);
  void 	WriteWarning (char * str);
  void	NIY (char * str);
  void  GenComment (char * str);
  BOOL	ErrorOccured (void);
  void 	PrintConstant (StackEntry * entry, ofstream& os);
  CobolSymbol *	LookupIdentifier (char * id);
  void	PrintIdentifier (char * id, ofstream& os);
  void 	GenIndent (void);
  WORD 	CountIdentifiers (void);
  void 	ReverseIdentifiers (WORD nIds);
  void	Push (StackEntryKind kind);
  BOOL	IsInSet (char c, char * set);
#if DEBUG
  void	PrintStackEntry (StackEntry * se);
  void	PrintStack (void);
#endif
/*-----------------------------------------------------------------------*/

#endif

