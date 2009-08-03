// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMLABEL_H_65C3ED8A24D067CC5020766B36AE2E2B
#define SYMLABEL_H_65C3ED8A24D067CC5020766B36AE2E2B

#include "symbase.h"

/// Defines a label symbol
class CobolLabel : public CobolSymbol {
public:
			CobolLabel (void);
    inline CobolSymbolType	Kind (void)	{ return (CS_Label); }
    void		GenDeclare (ostringstream& os);
    void		GenJump (ostringstream& os, CobolLabel * dest);
		       ~CobolLabel (void);
public:
    bool		Undeclared;	// For forward references
};

#endif
