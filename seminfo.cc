// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"

void SetProgramName (void)
{
    StackEntry* entry = SemStack.back(); SemStack.pop_back();
    if (entry) {
       if (entry->kind == SE_Identifier) {
          codef << "/* This program is called ";
	  codef << entry->ident << " */\n";
       }
       else
	  WriteError ("Program name must be one word");
       delete (entry);
    }
    else
       WriteError ("Program name expected");
}

void SetSourceComputer (void)
{
    StackEntry* entry = SemStack.back(); SemStack.pop_back();
    if (entry) {
       if (entry->kind == SE_Identifier)
          codef << "/* Written on " << entry->ident << " */\n";
       else
	  WriteError ("Source computer name must be one word");
       delete (entry);
    }
    else
       WriteError ("Source computer name expected");
}

void SetObjectComputer (void)
{
    StackEntry* entry = SemStack.back(); SemStack.pop_back();
    if (entry) {
	if (entry->kind == SE_Integer) {
	    entry = SemStack.back(); SemStack.pop_back();
	}
	if (entry->kind == SE_Identifier)
	    codef << "/* Written for " << entry->ident << " */\n\n";
	else
	    WriteError ("Object computer name must be one word");
	delete (entry);
    }
    else
	WriteError ("Object computer name expected");
}

