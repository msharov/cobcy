// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include <string.h>
#include "symconst.h"
#include "semutil.h"

CobolConstant::CobolConstant (void)
:_kind (CC_Undefined)
,_ival (0)
,_fval (0)
,_cval()
{
}

CobolConstant::CobolConstant (const char* cval)
:_kind (CC_String)
,_ival (0)
,_fval (0)
,_cval (cval, strlen(cval))
{
}

CobolConstant::CobolConstant (long ival)
:_kind (CC_Integer)
,_ival (ival)
,_fval (0)
,_cval()
{
}

CobolConstant::CobolConstant (double fval)
:_kind (CC_Float)
,_ival (0)
,_fval (fval)
,_cval()
{
}

CobolConstant::CobolConstant (const StackEntry& e)
: CobolConstant()
{
    operator= (e);
}

CobolConstant& CobolConstant::operator= (const char* cval)
{
    _kind = CC_String;
    _cval = cval;
    return *this;
}

CobolConstant& CobolConstant::operator= (long ival)
{
    _kind = CC_Integer;
    _ival = ival;
    return *this;
}

CobolConstant& CobolConstant::operator= (double fval)
{
    _kind = CC_Float;
    _fval = fval;
    return *this;
}

CobolConstant& CobolConstant::operator= (const StackEntry& e)
{
    switch (e.kind) {
	case SE_Bool:
	case SE_Integer:operator= (e.ival); break;
	case SE_Float:	operator= (e.fval); break;
	case SE_String:	operator= (e.ident); break;
	case SE_Quote:	operator= ("\\\""); break;
	default:	break;
    };
    return *this;
}

void CobolConstant::text_write (ostringstream& os) const
{
    switch (_kind) {
	case CC_String:		os << "\"" << _cval << "\"";	break;
	case CC_Integer:	os << _ival;	break;
	case CC_Float:		os << _fval;	break;
	default:		WriteError ("constant type %u unknown for writing", _kind);
				break;
    }
}

void CobolConstant::GenWrite (ostringstream& os, const char* stream) const
{
    GenIndent();
    if (!strcmp (stream, "stdout"))
	os << "printf (\"";
    else
	os << "fprintf (" << stream << ", \"";
    switch (_kind) {
	case CC_String:		os << _cval;	break;
	case CC_Integer:	os << _ival;	break;
	case CC_Float:		os << _fval;	break;
	default:		WriteError ("constant type %u unknown for writing", _kind);
				break;
    }
    os << "\");\n";
}
