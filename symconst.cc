// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include <string.h>
#include "symconst.h"
#include "semutil.h"

CobolConstant::CobolConstant (void)
{
    CurKind = CC_Undefined;
    data.cval = NULL;
}

CobolConstant::CobolConstant (const char* cval)
{
    int length;
    length = strlen (cval);
    data.cval = new char [length + 1];
    strncpy (data.cval, cval, length);
    data.cval [length] = '\x0';
    CurKind = CC_String;
}

CobolConstant::CobolConstant (long int ival)
{
    data.ival = ival;
    CurKind = CC_Integer;
}

CobolConstant::CobolConstant (double fval)
{
    data.fval = fval;
    CurKind = CC_Float;
}

CobolConstant& CobolConstant::operator= (const char* cval)
{
    int length;
    length = strlen (cval);
    if (CurKind == CC_String && data.cval != NULL)
	free (data.cval);
    data.cval = new char [length + 1];
    strncpy (data.cval, cval, length);
    data.cval [length] = '\x0';
    CurKind = CC_String;
    return (*this);
}

CobolConstant& CobolConstant::operator= (long int ival)
{
    if (CurKind == CC_String && data.cval != NULL) {
	delete [] data.cval;
	data.cval = NULL;
    }
    data.ival = ival;
    CurKind = CC_Integer;
    return (*this);
}

CobolConstant& CobolConstant::operator= (double fval)
{
    if (CurKind == CC_String && data.cval != NULL) {
	delete [] data.cval;
	data.cval = NULL;
    }
    data.fval = fval;
    CurKind = CC_Float;
    return (*this);
}

CobolConstant& CobolConstant::operator= (StackEntry * se)
{
    int length;

    if (CurKind == CC_String && data.cval != NULL)
	free (data.cval);

    switch (se->kind) {
	case SE_String:	length = strlen (se->ident);
			data.cval = new char [length + 1];
			strncpy (data.cval, se->ident, length);
			data.cval [length] = '\x0';
			CurKind = CC_String;
			break;

	case SE_Quote:	length = strlen ("\\\"");
			data.cval = new char [length + 1];
			strncpy (data.cval, "\\\"", length);
			data.cval [length] = '\x0';
			CurKind = CC_String;
			break;

	case SE_Integer: data.ival = se->ival;
			 CurKind = CC_Integer;
			 break;

	case SE_Float: 	data.fval = se->fval;
			CurKind = CC_Float;
			break;

	default:		WriteError ("internal: se not a constant");
				break;
    }
    return (*this);
}

void CobolConstant::text_write (ostringstream& os) const
{
    switch (CurKind) {
	case CC_String:	os << "\"" << data.cval << "\"";	break;
	case CC_Integer:	os << data.ival;	break;
	case CC_Float:	os << data.fval;	break;
	default:		WriteError ("constant type unknown for writing");
				break;
    }
}

void CobolConstant::GenWrite (ostringstream& os, const char* stream)
{
    GenIndent();
    os << "fprintf (" << stream << ", \"";
    switch (CurKind) {
	case CC_String:	os << data.cval;	break;
	case CC_Integer:	os << data.ival;	break;
	case CC_Float:	os << data.fval;	break;
	default:		WriteError ("constant type unknown for writing");
				break;
    }
    os << "\");\n";
}

CobolConstant::~CobolConstant (void)
{
    if (CurKind == CC_String && data.cval != NULL) {
	delete [] data.cval;
	data.cval = NULL;
    }
}
