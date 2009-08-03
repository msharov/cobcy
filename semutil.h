// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SEMUTIL_H_6A73F48510972692670B68D112C25CA4
#define SEMUTIL_H_6A73F48510972692670B68D112C25CA4

#include "semtypes.h"
#include "symbase.h"

//-----------------------------------------------------------------------
  void 	WriteError (const char* str);
  void 	WriteWarning (const char* str);
  void	NIY (const char* str);
  void  GenComment (const char* str);
  bool	ErrorOccured (void);
  void 	PrintConstant (StackEntry * entry, ostringstream& os);
  CobolSymbol *	LookupIdentifier (const char* id);
  void	PrintIdentifier (const char* id, ostringstream& os);
  void 	GenIndent (void);
  uint32_t 	CountIdentifiers (void);
  void 	ReverseIdentifiers (uint32_t nIds);
  void	Push (StackEntryKind kind);
  bool	IsInSet (char c, const char* set);
#ifndef NDEBUG
  void	PrintStackEntry (StackEntry * se);
  void	PrintStack (void);
#endif
//-----------------------------------------------------------------------

#endif
