// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "symdata.h"
#include "symconst.h"
#include "sympic.h"

/// Defines a variable symbol
class CobolVar : public CobolData {
public:
			CobolVar (void);
    virtual		~CobolVar (void) override;
    void		SetPicture (const char* NewPic);
    virtual CobolSymbolType	Kind (void) const override { return CS_Variable; }
    inline bool		IsNumeric (void) const	{ return Picture.IsNumeric(); }
    void		GenDeclare (ostringstream& os);
    void		GenMove (ostringstream& os, CobolVar * data);
    void		GenMove (ostringstream& os, CobolConstant& data);
    void		GenArith (ostringstream& os, CobolSymbol* op1, CobolSymbol* op2, char op);
    void		GenCharCast (ostringstream& os);
    virtual void	GenRead (ostringstream& os, const char* stream) override;
    virtual void	GenWrite (ostringstream& os, const char* stream) override;
    virtual void	GenSignature (ostringstream& os) override;
    inline void		WritePicture (ostringstream& os)	{ os << Picture; } 
protected:
    PictureType		Picture;
};
