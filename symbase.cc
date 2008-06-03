// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symbase.h"
#include <stdio.h>

CobolSymbol :: CobolSymbol (void)
{
    memset (ParentCobolName, 0, MAX_SYMBOL_LENGTH);
    memset (CobolName, 0, MAX_SYMBOL_LENGTH);
    memset (CName, 0, MAX_SYMBOL_LENGTH);
    memset (Prefix, 0, MAX_PREFIX_LENGTH);
    memset (FullCName, 0, MAX_FULLNAME_LENGTH);
}

void CobolSymbol :: CobolToCName (char* str)
{
    for (unsigned i = 0; i < strlen(str); ++ i)
	if (str[i] == '-')
	    str[i] = '_';
}

void CobolSymbol :: SetName (const char* NewName)
{
    strcpy (CobolName, NewName);
    strcpy (CName, NewName);
    CobolToCName (CName); 
    strcpy (FullCName, Prefix);
    strcat (FullCName, CName);
}

void CobolSymbol :: SetParent (const char* NewParent)
{
CobolSymbol * pattr;
char ErrorBuffer [80];

    if (NewParent == NULL) {
       memset (ParentCobolName, MAX_SYMBOL_LENGTH, 0);
       memset (Prefix, MAX_PREFIX_LENGTH, 0);
       strcpy (FullCName, CName);
    }
    else {
       if ((pattr = SymTable [NewParent]) == NULL) {
	  sprintf (ErrorBuffer, 
	  	   "cannot find parent %s; wanted for child support; reward", 
		   NewParent);
	  WriteError (ErrorBuffer);
	  return;
       }

       strcpy (Prefix, pattr->Prefix);
       strcat (Prefix, pattr->CName);
       strcat (Prefix, ".");

       strcpy (FullCName, Prefix);
       strcat (FullCName, CName);
    }
}

void CobolSymbol :: text_write (ostringstream& os) const
{
    os << FullCName;
}

CobolSymbol :: ~CobolSymbol (void)
{
    // Keep it around just in case...
}

