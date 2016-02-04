// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semutil.h"
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
extern const CobolLabel* CurPar;
extern bool CodeBegan;

static bool s_ErrorFlag = false;

void WriteError (const char* fmt, ...) noexcept
{
    try {
	va_list args;
	va_start (args, fmt);
	string str;
	str.vformat (fmt, args);
	va_end (args);

	if (CodeBegan && CurPar) {
	    cerr.format ("%s:%d: in paragraph ", g_Config.SourceFile.c_str(), CurrentLine);
	    cerr << *CurPar << ":\n";
	}
	cerr.format ("%s:%d: error: %s.\n", g_Config.SourceFile.c_str(), CurrentLine, str.c_str());
	#ifndef NDEBUG
	    if (g_Config.GenDebug)
		PrintStack();
	#endif
    } catch (...) {}
    s_ErrorFlag = true;
}

void WriteWarning (const char* fmt, ...) noexcept
{
    try {
	va_list args;
	va_start (args, fmt);
	string str;
	str.vformat (fmt, args);
	va_end (args);

	if (CodeBegan && CurPar) {
	    cerr.format ("%s:%d: in paragraph: ", g_Config.SourceFile.c_str(), CurrentLine);
	    cerr << *CurPar << ":\n";
	}
	cerr.format ("%s:%d: warning: %s.\n", g_Config.SourceFile.c_str(), CurrentLine, str.c_str());
	#ifndef NDEBUG
	    if (g_Config.GenDebug)
		PrintStack();
	#endif
    } catch (...) {}
}

#ifndef NDEBUG
void PrintStackEntry (const StackEntry& se)
{
    switch (se.kind) {
	case SE_Error:		cerr << "error token"; 	break;
	case SE_Mark:		cerr << "mark";		break;
	case SE_Null:		cerr << "null";		break;
	case SE_Integer:	cerr << "integer " << se.ival; break;
	case SE_Float:		cerr << "float " << se.fval; break;
	case SE_String:		cerr << "string " << se.ident; break;
	case SE_Identifier:	cerr << "identifier " << se.ident; break;
	case SE_Picture:	cerr << "picture [" << se.ident << "]"; break;
	case SE_Bool:		cerr << "boolean";	break;
	case SE_Connector:	cerr << "connector";	break;
	case SE_Operator:	cerr << "operator";	break;
	case SE_Quote:		cerr << "quote";	break;
    }
}

void PrintStack (void)
{
    cerr << "Stack:\n";
    eachfor (auto, i, g_Tokens) {
	cerr << "\t";
	PrintStackEntry (*i);
	cerr << "\n";
    }
}
#endif

void NIY (const char* str)
{
    GenComment ("%s is unimplemented", str);
    WriteWarning ("%s is unimplemented", str);
}

void GenComment (const char* fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    string str;
    str.vformat (fmt, args);
    va_end (args);
    codef.format ("// %s\n", str.c_str());
}

bool ErrorOccured (void)
{
#ifndef NDEBUG
    if (g_Config.GenDebug)
	cerr << "ErrorOccured: " << s_ErrorFlag << "\n";
#endif
    return s_ErrorFlag;
}

void PrintConstant (const StackEntry& entry, ostringstream& os)
{
    CobolConstant ctp = entry;	// Just to keep all the stuff in one place
    os << ctp;
}

void PrintIdentifier (const char* id, ostringstream& os)
{
    auto sym = LookupIdentifier<CobolSymbol> (id);
    if (sym)
	os << *sym;
    else
	os << "_Unknown_Identifier";
}

vector<StackEntry> PopStatement (void)
{
    vector<StackEntry> stmt;
    for (bool mark = false; !g_Tokens.empty() && !mark; g_Tokens.pop_back())
	if (!(mark = (g_Tokens.back().kind == SE_Mark)))
	    stmt.emplace_back (move (g_Tokens.back()));
    return stmt;
}

StackEntry PopIdentifier (void)
{
    if (g_Tokens.empty()) {
	WriteError ("identifier expected");
	return StackEntry();
    }
    StackEntry e (move (g_Tokens.back()));
    g_Tokens.pop_back();
    return e;
}

void GenIndent (void)
{
    auto nTabs = NestingLevel / 2;
    for (auto i = 0; i < nTabs; ++i)
	codef << '\t';
    if (NestingLevel % 2)
	codef << "    ";
}

void Push (StackEntryKind kind)
{
    g_Tokens.push_back();
    auto& e = g_Tokens.back();
    e.kind = kind;
    e.fval = e.ival = 0;
    switch (kind) {
	case SE_Integer:
	    e.ival = ival;
	    break;
	case SE_Float:
	    e.fval = fval;
	    break;
	case SE_String:
	case SE_Identifier:
	case SE_Picture:
	case SE_Bool:
	case SE_Connector:
	    e.ident = StringBuffer;
	    break;
	case SE_Operator:
	    e.opkind = opkind;
	    break;
	default:	// For others, simply set the type
	    break;
    }
}
