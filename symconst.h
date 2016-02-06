// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "symbase.h"
#include "semtypes.h"

/// Defines a constant data type. This never enters the symbol table.
class CobolConstant : public CobolSymbol {
public:
				CobolConstant (void);
				CobolConstant (const char* cval);
				CobolConstant (long ival);
				CobolConstant (double fval);
				CobolConstant (const StackEntry& e);
    CobolConstant&		operator= (const char* cval);
    CobolConstant&		operator= (long ival);
    CobolConstant&		operator= (double fval);
    CobolConstant&		operator= (const StackEntry& se);
    virtual CobolSymbolType	Kind (void) const override { return CS_Constant; }
    virtual void		text_write (ostringstream& os) const override;
    void			GenWrite (ostringstream& os, const char* stream) const;
    inline bool			IsNumeric (void) const	{ return _kind == CC_Integer || _kind == CC_Float; }
protected:
    enum EKind {
        CC_Undefined,
        CC_String,
	CC_Integer,
	CC_Float
    };
protected:
    EKind		_kind;
    long		_ival;
    double		_fval;
    string		_cval;
};
