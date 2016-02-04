// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symrec.h"

CobolRecord::CobolRecord (void)
{
}

void CobolRecord::AddChild (CobolData * NewChild)
{
    ChildList.push_back (NewChild);
    CSize += NewChild->GetSize();
}

void CobolRecord::GenDeclareBegin (ostringstream& os)
{
    GenIndent();
    os << "struct {\n";
}

void CobolRecord::GenDeclareEnd (ostringstream& os)
{
    GenIndent();
    os << "} " << GetCName() << ";\n";
}

void CobolRecord::GenRead (ostringstream& os, const char* stream)
{
    #ifndef NDEBUG
	if (g_Config.GenDebug)
	    cout << "\tReading record " << *this << " from stream " << stream << "\n";
    #endif
    foreach (vector<CobolData*>::const_iterator, i, ChildList) {
	#ifndef NDEBUG
	    if (g_Config.GenDebug)
		cout << "\t\tReading child " << **i << "\n";
	#endif
	(*i)->GenRead (os, stream);
    }
}

void CobolRecord::GenWrite (ostringstream& os, const char* stream)
{
    #ifndef NDEBUG
	if (g_Config.GenDebug)
	    cout << "\tWriting record " << *this << " to stream " << stream << "\n";
    #endif
    foreach (vector<CobolData*>::const_iterator, i, ChildList) {
	#ifndef NDEBUG
	    if (g_Config.GenDebug)
		cout << "\t\tWriting child " << **i << "\n";
	#endif
	(*i)->GenWrite (os, stream);
    }
}

void CobolRecord::GenSignature (ostringstream& os)
{
    #ifndef NDEBUG
	if (g_Config.GenDebug)
	    cout << "\tSigning record " << *this << "\n";
    #endif
    foreach (vector<CobolData*>::const_iterator, i, ChildList) {
	#ifndef NDEBUG
	    if (g_Config.GenDebug)
		cout << "\t\tSigning child " << **i << "\n";
	#endif
	(*i)->GenSignature (os);
    }
}
