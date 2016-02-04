// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "symdata.h"
#include "symfile.h"

/// Defines a cobcy record symbol
class CobolRecord : public CobolData {
public:
				CobolRecord (void);
    void			AddChild (CobolData* NewChild);
    virtual CobolSymbolType	Kind (void) const override { return CS_Record; }
    void			GenDeclareBegin (ostringstream& os);
    void			GenDeclareEnd (ostringstream& os);
    virtual void		GenRead (ostringstream& os, const char* stream) override;
    virtual void		GenWrite (ostringstream& os, const char* stream) override;
    virtual void		GenSignature (ostringstream& os) override;
private:
    vector<CobolData*>		ChildList;
};
