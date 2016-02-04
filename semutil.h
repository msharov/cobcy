// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "semtypes.h"
#include "symbase.h"

void WriteError (const char* fmt, ...) noexcept __attribute__((__format__(__printf__, 1, 2)));
void WriteWarning (const char* fmt, ...) noexcept __attribute__((__format__(__printf__, 1, 2)));
void NIY (const char* str);
void GenComment (const char* fmt, ...) __attribute__((__format__(__printf__, 1, 2)));
bool ErrorOccured (void);
void PrintConstant (const StackEntry& entry, ostringstream& os);
void PrintIdentifier (const char* id, ostringstream& os);
void GenIndent (void);
StackEntry PopIdentifier (void);
vector<StackEntry> PopStatement (void);
void Push (StackEntryKind kind);
#ifndef NDEBUG
void PrintStackEntry (StackEntry * se);
void PrintStack (void);
#define	DTRACE(...)	if (g_Config.GenDebug) cerr.format (__VA_ARGS__)
#else
#define DTRACE(...)	do {} while(false)
#endif

namespace {

static inline bool IsInSet (char c, const char* s)
{
    return strchr (s, c);
}

} // namespace

template <typename Sym>
Sym* LookupIdentifier (const string& id)
{
    auto sym = g_Symbols.find (id);
    if (!sym) {
	::WriteError ("identifier '%s' is unknown", id.c_str());
	return nullptr;
    }
    auto rv = dynamic_cast<Sym*>(sym);
    if (!rv) {
	::WriteError ("identifier '%s' is of a different type", id.c_str());
	return nullptr;
    }
    return rv;
}
