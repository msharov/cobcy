// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMREC_H_07577D003AAC209C30EF0B4E53998B58
#define SYMREC_H_07577D003AAC209C30EF0B4E53998B58

#include "symdata.h"
#include "symfile.h"

/// Defines a cobcy record symbol
class CobolRecord : public CobolData {
public:
				CobolRecord (void);
    void			AddChild (CobolData* NewChild);
    CobolSymbolType		Kind (void);
    void			GenDeclareBegin (ostringstream& os);
    void			GenDeclareEnd (ostringstream& os);
    void			GenRead (ostringstream& os, const char* stream);
    void			GenWrite (ostringstream& os, const char* stream);
    void			GenSignature (ostringstream& os);
private:
    vector<CobolData*>		ChildList;
};

#endif
