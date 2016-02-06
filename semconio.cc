// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "semconio.h"
#include "symdata.h"
#include "symconst.h"
#include "symfile.h"
#include "semcontrol.h"

//---------------------| Globals |--------------------------------

static string s_DisplayOutput;
static AcceptSourceType s_AcceptSource;

//----------------------------------------------------------------

void GenAccept (void)
{
    DTRACE ("\tIn GenAccept\n");
    auto stmt = PopStatement();
    for (auto& e : stmt) {
	if (e.kind == SE_Identifier) {
	    auto attr = LookupIdentifier<CobolData> (e.ident);
	    if (!attr)
		return;
	    switch (s_AcceptSource) {
		case AS_Console:
		    attr->GenRead (codef, "stdin");
		    GenEmptyClause();
		    break;
		case AS_Date:
		    GenIndent();
		    attr->text_write (codef);
		    codef << " = ";
		    codef << "_GetDate();\n";
		    break;
		case AS_Day:
		    GenIndent();
		    attr->text_write (codef);
		    codef << " = ";
		    codef << "_GetDay();\n";
		    break;
		case AS_Weekday:
		    NIY ("Weekdays");
		    break;
		case AS_Time:
		    GenIndent();
		    attr->text_write (codef);
		    codef << " = ";
		    codef << "_GetTime();\n";
		    break;
	    }
	}
    }
}

void SetAcceptSource (AcceptSourceType NewSrc)
{
    s_AcceptSource = NewSrc;
}

void GenDisplay (void)
{
    DTRACE ("\tIn GenDisplay\n");
    auto stmt = PopStatement();
    reverse (stmt.begin(), stmt.end());
    for (auto& e : stmt) {
	if (e.kind == SE_Identifier) {
	    auto attr = LookupIdentifier<CobolData> (e.ident);
	    if (!attr)
		return;
	    attr->GenWrite (codef, s_DisplayOutput);
	} else {
	    CobolConstant ce = e;
	    ce.GenWrite (codef, s_DisplayOutput);
	}
    }
    CobolConstant("\\n").GenWrite (codef, s_DisplayOutput);
}

void SetDisplayOutput (void)
{
    auto id = PopIdentifier();
    auto outStrSym = LookupIdentifier<CobolFile> (id.ident);
    if (outStrSym)
	s_DisplayOutput = outStrSym->GetFullCName();
    DTRACE ("\tDISPLAY output set to %s\n", s_DisplayOutput.c_str());
}
