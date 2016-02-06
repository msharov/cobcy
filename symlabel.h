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
    virtual CobolSymbolType	Kind (void) const override	{ return CS_Label; }
    void		GenDeclare (ostringstream& os) const;
    void		GenJump (ostringstream& os, const CobolLabel* dest) const;
    void		GenPrototype (ostringstream& os) const;
public:
    bool		Undeclared;	// For forward references
};
