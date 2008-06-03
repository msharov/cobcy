// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMCONST_H_06AC2DD61E5065A760B47DBD4CA38338
#define SYMCONST_H_06AC2DD61E5065A760B47DBD4CA38338

#include "symbase.h"
#include "semtypes.h"

/// Defines a constant data type. This never enters the symbol table.
class CobolConstant : public CobolSymbol {
protected:
    union {
        char *		cval;
	long int	ival;
	double		fval;
    }				data;
    enum {
        CC_Undefined,
        CC_String,
	CC_Integer,
	CC_Float
    }				CurKind;

public:
				CobolConstant (void);
				CobolConstant (const char* cval);
				CobolConstant (long int ival);
				CobolConstant (double fval);
    CobolConstant&		operator= (const char* cval);
    CobolConstant&		operator= (long int ival);
    CobolConstant&		operator= (double fval);
    CobolConstant&		operator= (StackEntry * se);
    virtual CobolSymbolType	Kind (void) { return (CS_Constant); }
    virtual void		text_write (ostringstream& os) const;
    void			GenWrite (ostringstream& os, const char* stream);
    inline bool			IsNumeric (void);
    virtual		       ~CobolConstant (void);
};

/*---------------------------------------------------------------------------*/

inline bool CobolConstant :: IsNumeric (void)
{
    return (CurKind == CC_Integer || CurKind == CC_Float);
}

#endif

