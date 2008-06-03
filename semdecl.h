// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SEMDECL_H_047C309F443754C00B988A6E6B1E97A6
#define SEMDECL_H_047C309F443754C00B988A6E6B1E97A6

#include "config.h"

#define STACK_IDENT_LENGTH	50

/*------------------------------------------------*/
  void 	DeclareRecordLevel (void);
  void 	CloseScopeLevels (uint32_t LastLevel);
  void 	InitializeVariables (void);
  void	DeclareSpecialName (void);
/*------------------------------------------------*/

#endif

