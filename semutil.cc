// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symbase.h"
#include "symconst.h"
#include "symlabel.h"
#include <stdio.h>

extern long int ival;
extern double fval;
extern char StringBuffer[];
extern SEOperatorKind opkind;
extern int CurrentLine;
extern CobolLabel * CurPar;
extern bool CodeBegan;

bool ErrorFlag = false;

void WriteError (const char* str)
{
    if (CodeBegan && CurPar != NULL) {
       cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
       cerr << "in paragraph " << *CurPar << ":\n";
    }
    cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
    cerr << "error: ";
    cerr << str << ".\n";
#ifndef NDEBUG
    PrintStack();
#endif
    ErrorFlag = true;
}

void WriteWarning (const char* str)
{
    if (CodeBegan && CurPar != NULL) {
       cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
       cerr << "in paragraph " << *CurPar << ":\n";
    }
    cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
    cerr << "warning: ";
    cerr << str << ".\n";
#ifndef NDEBUG
    PrintStack();
#endif
}

#ifndef NDEBUG
void PrintStackEntry (StackEntry * se)
{
    switch (se->kind) {
	case SE_Error: 	cerr << "error token"; 	break;
	case SE_Mark: 	cerr << "mark"; 		break;
	case SE_Null:	cerr << "null";		break;
	case SE_Integer:
			cerr << "integer " << se->ival;
			break;
	case SE_Float:
			cerr << "float " << se->fval;
			break;
	case SE_String: 
			cerr << "string " << se->ident;
			break;
	case SE_Identifier:
			cerr << "identifier " << se->ident;
			break;
	case SE_Picture:
			cerr << "picture [" << se->ident << "]";	
			break;
	case SE_Bool:		cerr << "boolean";	break;
	case SE_Connector:	cerr << "connector";	break;
	case SE_Operator:	cerr << "operator";	break;
	case SE_Quote:		cerr << "quote";	break;
    }
}

void PrintStack (void)
{
    cerr << "Stack:\n";
    eachfor (vector<StackEntry*>::reverse_iterator, i, SemStack) {
	cerr << "\t" << distance(SemStack.rbegin(),i) << ") ";
	PrintStackEntry (*i);
	cerr << "\n";
    }
}
#endif

void NIY (const char* str)
{
char MessageBuf [80];

    GenIndent();
    sprintf (MessageBuf, "%s is unimplemented", str);
    codef << "/* " << MessageBuf << " */\n";
    WriteWarning (MessageBuf);
}

void GenComment (const char* str)
{
    GenIndent();
    codef << "/* " << str << " */\n";
}

bool ErrorOccured (void)
{
#ifndef NDEBUG
    cerr << "ErrorOccured: ";
    cerr << (ErrorFlag ? "true" : "false");
    cerr << "\n";
#endif
    return (ErrorFlag);
}

void PrintConstant (StackEntry * entry, ostringstream& os)
{
    CobolConstant ctp;	// Just to keep all the stuff in one place
    ctp = entry;
    os << ctp;
}

CobolSymbol * LookupIdentifier (const char* id)
{
CobolSymbol * sym;
char ErrorBuffer [80];

    if ((sym = SymTable[id]) == NULL) {
       sprintf (ErrorBuffer, "identifier '%s' is unknown", id);
       WriteError (ErrorBuffer);
       return (NULL);
    }
    return (sym);
}

void PrintIdentifier (const char* id, ostringstream& os)
{
CobolSymbol * sym;

    sym = LookupIdentifier (id);
    os << *sym;
}

uint32_t CountIdentifiers (void)
{
    vector<StackEntry*>::iterator i = SemStack.end();
    while (i > SemStack.begin() && (*--i)->kind != SE_Mark) ;
    if (i < SemStack.end()) {
	if (i < SemStack.end() && (*i)->kind != SE_Mark)
	    WriteError ("No mark found");
	else {
	    delete *i;	// Delete the mark
	    i = SemStack.erase (i);
	}
    }
    return (distance (i, SemStack.end()));
}

void ReverseIdentifiers (uint32_t nIds)
{
    reverse (SemStack.end() - nIds, SemStack.end());
}

void GenIndent (void)
{
int i;
    for (i = 0; i < NestingLevel; ++ i)
       codef << "    ";
}

void Push (StackEntryKind kind)
{
    StackEntry* NewEntry = new StackEntry;
    NewEntry->kind = kind;
    memset (NewEntry->ident, 0, STACK_IDENT_LENGTH);
    NewEntry->fval = NewEntry->ival = 0;

    switch (kind) {
	case SE_Integer:
		NewEntry->ival = ival;
		break;
	case SE_Float:
		NewEntry->fval = fval;
		break;
	case SE_String:
	case SE_Identifier:
	case SE_Picture:
	case SE_Bool:
	case SE_Connector:
		strcpy (NewEntry->ident, StringBuffer);
		break;
	case SE_Operator:
		NewEntry->opkind = opkind;
		break;
	default:		// For others, simply set the type
		break;
    }

    SemStack.push_back (NewEntry);
}

bool IsInSet (char c, const char* set)
{
register int i, sl;

    sl = strlen (set);
    for (i = 0; i < sl; ++ i) {
       if (c == set[i])
	  return (true);
    }
    return (false);
}

