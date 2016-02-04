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
    string	DeclFile;	// Output .h file
    bool	GenDebug;			// Generate debugging code?
    inline	CobcyConfigType (void) : SourceFile(),CodeFile(),DeclFile(),GenDebug(false) {}
};

// All of these are defined in seminit.cc
extern vector<StackEntry> g_Tokens;
extern int NestingLevel;
extern ofstream codef, declf;
extern CobcyConfigType g_Config;
