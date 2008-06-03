// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMVAR_H_34B141462380B541113CC1CA5AF76B44
#define SYMVAR_H_34B141462380B541113CC1CA5AF76B44

#include "symdata.h"
#include "symconst.h"
#include "sympic.h"

/// Defines a variable symbol
class CobolVar : public CobolData {
protected:
    PictureType		Picture;

public:
			CobolVar (void);
    void		SetPicture (const char* NewPic);
    virtual CobolSymbolType	Kind (void);
    inline bool		IsNumeric (void);

    void		GenDeclare (ostringstream& os);
    void		GenRead (ostringstream& os, const char* stream);
    void		GenWrite (ostringstream& os, const char* stream);
    void		GenMove (ostringstream& os, CobolVar * data);
    void		GenMove (ostringstream& os, CobolConstant& data);
    void		GenArith (ostringstream& os, CobolSymbol* op1, CobolSymbol* op2, char op);
    void		GenSignature (ostringstream& os);
    void		GenCharCast (ostringstream& os);
    inline void		WritePicture (ostringstream& os);

    virtual	       ~CobolVar (void);
};

/*-----------------------------------------------------------------------*/

inline bool CobolVar :: IsNumeric (void)
{
    return (Picture.IsNumeric());
}

inline void CobolVar :: WritePicture (ostringstream& os)
{
    os << Picture;
}

#endif

