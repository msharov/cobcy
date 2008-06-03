// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "symdata.h"
#include "symfile.h"

CobolData :: CobolData (void)
{
    CSize = 0;
    DeclLevel = 0;
    AssociatedStream = NULL;
}

CobolFile * CobolData :: GetStream (void)
{
    return (AssociatedStream);
}

void CobolData :: AssociateWithStream (CobolFile * NewStream)
{
    AssociatedStream = NewStream;
}

void CobolData :: GenRead (ostringstream& os)
{
    GenRead (os, AssociatedStream->GetFullCName());
}

void CobolData :: GenWrite (ostringstream& os)
{
    GenWrite (os, AssociatedStream->GetFullCName());
}

CobolData :: ~CobolData (void)
{
}

