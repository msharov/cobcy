/* semutil.h
**
**	Prototypes semantic utilities for COBOL compiler.
*/

#ifndef __SEMUTIL_H
#define __SEMUTIL_H

#include <fstream.h>
#include <mdefs.h>
#include "semsedef.h"
#include "symbol.h"

/*-----------------------------------------------------------------------*/
  void 	WriteError (char * str);
  void 	WriteWarning (char * str);
  BOOL	ErrorOccured (void);
  void 	PrintConstant (StackEntry * entry, ofstream& os);
  void	LookupParent (char * symbol, char * prefix);
  void	BuildPrefix (char * symbol, char * prefix);
  void 	GenIndent (void);
  void 	ReadVariable (CobolSymbol * attr, ofstream& os, 
  			char * stream, BOOL nl = TRUE);
  void 	PrintVariable (CobolSymbol * attr, ofstream& os,
  			char * stream, BOOL nl = TRUE);
  void 	ReadRecord (CobolSymbol * attr, ofstream& os, 
  			char * stream, BOOL nl = TRUE);
  void 	PrintRecord (CobolSymbol * attr, ofstream& os, 
  			char * stream, BOOL nl = TRUE);
  void 	LookupParent (char * symbol, char * prefix);
  void 	CloseScopeLevels (int LastLevel);
  void 	BuildPrefix (char * symbol, char * prefix);
  WORD 	CountIdentifiers (void);
  void 	ReverseIdentifiers (WORD nIds);
  void	Push (StackEntryKind kind);
/*-----------------------------------------------------------------------*/

#endif

