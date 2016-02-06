// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"

void SetProgramName (void)
{
    auto e = PopIdentifier();
    if (e.kind == SE_Identifier)
	GenComment (declf, "This program is called %s", e.ident.c_str());
    else
	WriteError ("program name must be one word");
}

void SetSourceComputer (void)
{
    auto e = PopIdentifier();
    if (e.kind == SE_Identifier)
	GenComment (declf, "Written on %s", e.ident.c_str());
    else
	WriteError ("source computer name must be one word");
}

void SetObjectComputer (void)
{
    auto e = PopIdentifier();
    if (e.kind == SE_Integer)
	e = PopIdentifier();
    if (e.kind == SE_Identifier)
	GenComment (declf, "Written for %s", e.ident.c_str());
    else
	WriteError ("target computer name expected");
}
