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
			CobolVar (void)			: Picture() {}
    void		SetPicture (const char* pic)	{ CSize = Picture.Set (pic); }
    virtual CobolSymbolType	Kind (void) const override	{ return CS_Variable; }
    inline bool		IsNumeric (void) const			{ return Picture.IsNumeric(); }
    void		GenDeclare (ostringstream& os) const;
    void		GenMove (ostringstream& os, const CobolVar* data) const;
    void		GenMove (ostringstream& os, const CobolConstant& data) const;
    void		GenArith (ostringstream& os, const CobolSymbol* op1, const CobolSymbol* op2, char op) const;
    void		GenCharCast (ostringstream& os) const;
    virtual void	GenRead (ostringstream& os, const char* stream) const override;
    virtual void	GenWrite (ostringstream& os, const char* stream) const override;
    virtual void	GenSignature (ostringstream& os) const override;
    inline void		WritePicture (ostringstream& os) const	{ os << Picture; }
protected:
    PictureType		Picture;
};
