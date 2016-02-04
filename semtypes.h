// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "config.h"

enum { STACK_IDENT_LENGTH = 50 };

//----------------------| StackEntry stuff |----------------------------

enum StackEntryKind {
    SE_Error,
    SE_Mark,
    SE_Null,
    SE_Integer,
    SE_Float,
    SE_String,
    SE_Identifier,
    SE_Picture,
    SE_Bool,
    SE_Connector,
    SE_Operator,
    SE_Quote
};

enum SEOperatorKind {
    OP_Addition,
    OP_Subtraction,
    OP_Multiplication,
    OP_Division,
    OP_LParen,
    OP_RParen,
    OP_Equal
};

struct StackEntry {
    StackEntryKind	kind;
    SEOperatorKind	opkind;
    long int		ival;
    double		fval;
    string		ident;
};

//--------------------------| File declaration enums |--------------------

enum AccessModeType {
    AM_Sequential,
    AM_Random,
    AM_Dynamic
};

enum OrganizationType {
    ORG_Sequential,
    ORG_Line_sequential,
    ORG_Relative,
    ORG_Indexed
};

enum OpenModeType {
    OM_Input,
    OM_Output,
    OM_Extend,
    OM_InputOutput
};


//---------------------| Display and accept enums |----------------------

enum AcceptSourceType {
    AS_Date,
    AS_Day,
    AS_Weekday,
    AS_Time,
    AS_Console
};
