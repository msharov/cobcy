/* semtypes.h
**
**	Various types used by semantic routines.
*/

#ifndef __SEMTYPES_H
#define __SEMTYPES_H

#define STACK_IDENT_LENGTH	50


/*----------------------| StackEntry stuff |----------------------------*/

typedef enum {
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
} StackEntryKind;

typedef enum {
    OP_Addition,
    OP_Subtraction,
    OP_Multiplication,
    OP_Division,
    OP_LParen,
    OP_RParen,
    OP_Equal
} SEOperatorKind;

typedef struct {
    StackEntryKind	kind;
    char		ident [STACK_IDENT_LENGTH];
    long int		ival;
    double		fval;
    SEOperatorKind	opkind;
} StackEntry;


/*--------------------------| File declaration enums |--------------------*/

typedef enum {
    AM_Sequential,
    AM_Random,
    AM_Dynamic
} AccessModeType;

typedef enum {
    ORG_Sequential,
    ORG_Line_sequential,
    ORG_Relative,
    ORG_Indexed
} OrganizationType;

typedef enum {
    OM_Input,
    OM_Output,
    OM_Extend,
    OM_InputOutput
} OpenModeType;


/*---------------------| Display and accept enums |----------------------*/

typedef enum {
    AS_Date,
    AS_Day,
    AS_Weekday,
    AS_Time,
    AS_Console
} AcceptSourceType;

#endif


