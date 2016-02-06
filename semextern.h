// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"
#include "semtypes.h"
#include "semutil.h"

/// Cobcy global configuration options
struct CobcyConfigType {
    string	SourceFile;	// Input COBOL file
    string	CodeFile;	// Output .c file
    bool	GenDebug;			// Generate debugging code?
    inline	CobcyConfigType (void) : SourceFile(),CodeFile(),GenDebug(false) {}
};

// All of these are defined in seminit.cc
extern vector<StackEntry> g_Tokens;
extern int NestingLevel;
extern ostringstream codef, declf;
extern CobcyConfigType g_Config;
