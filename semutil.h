/* semutil.h
**
**	Prototypes semantic utilities for COBOL compiler.
*/

#ifndef __SEMUTIL_H
#define __SEMUTIL_H

#include "semtypes.h"
#include "symbase.h"

/*-----------------------------------------------------------------------*/
  void 	WriteError (const char* str);
  void 	WriteWarning (const char* str);
  void	NIY (const char* str);
  void  GenComment (const char* str);
  bool	ErrorOccured (void);
  void 	PrintConstant (StackEntry * entry, ostream& os);
  CobolSymbol *	LookupIdentifier (const char* id);
  void	PrintIdentifier (const char* id, ostream& os);
  void 	GenIndent (void);
  WORD 	CountIdentifiers (void);
  void 	ReverseIdentifiers (WORD nIds);
  void	Push (StackEntryKind kind);
  bool	IsInSet (char c, const char* set);
#ifndef NDEBUG
  void	PrintStackEntry (StackEntry * se);
  void	PrintStack (void);
#endif
/*-----------------------------------------------------------------------*/

#endif

