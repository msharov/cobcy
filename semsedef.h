/* semsedef.h
**
**	Declares the stack entry type.
*/

#ifndef __SEMSEDEF_H
#define __SEMSEDEF_H

#define STACK_IDENT_LENGTH	50

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
    SE_Quote
} StackEntryKind;

typedef struct {
    StackEntryKind	kind;
    char		ident [STACK_IDENT_LENGTH];
    long int		ival;
    double		fval;
} StackEntry;

#endif

