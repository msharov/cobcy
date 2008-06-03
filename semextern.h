// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SEMEXTERN_H_00C77BAD5E27EA510B548B8E1FFD9409
#define SEMEXTERN_H_00C77BAD5E27EA510B548B8E1FFD9409

#include "config.h"
#include "semtypes.h"
#include "semutil.h"

#define MAX_FILENAME	30

/***--| Cobcy global configuration options |--***/
typedef struct {
    /* Filename locations */
    char 	SourceFile [MAX_FILENAME];	/* Input COBOL file */
    char 	CodeFile [MAX_FILENAME];	/* Output .c file */
    char 	DeclFile [MAX_FILENAME];	/* Output .h file */

    /* Build options */
    bool	GenDebug;	/* Generate debugging code? */
} CobcyConfigType;

// All of these are defined in seminit.cc
extern vector<StackEntry*> SemStack;
typedef map<string,CobolSymbol*> symtable_t;
extern symtable_t SymTable;
extern int NestingLevel;
extern ofstream codef, declf;
extern CobcyConfigType CobcyConfig;

#endif

