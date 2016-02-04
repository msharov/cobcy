// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "symbase.h"

/// Defines a label symbol
class CobolLabel : public CobolSymbol {
public:
			CobolLabel (void);
    inline CobolSymbolType	Kind (void)	{ return CS_Label; }
    void		GenDeclare (ostringstream& os);
    void		GenJump (ostringstream& os, CobolLabel * dest);
		       ~CobolLabel (void);
public:
    bool		Undeclared;	// For forward references
};
