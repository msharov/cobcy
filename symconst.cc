/* symconst.h
**
**	Defines a constant data type. This never enters the symbol table.
*/

#include <string.h>
#include "symconst.h"
#include "semutil.h"

CobolConstant :: CobolConstant (void)
{
    CurKind = CC_Undefined;
    data.cval = NULL;
}

CobolConstant :: CobolConstant (char* cval)
{
int length;
    length = strlen (cval);
    data.cval = new char [length + 1];
    strncpy (data.cval, cval, length);
    data.cval [length] = '\x0';
    CurKind = CC_String;
}

CobolConstant :: CobolConstant (StackEntry* pEntry)
{
    CurKind = CC_Undefined;
    data.cval = NULL;
    operator= (pEntry);
}

CobolConstant :: CobolConstant (long int ival)
{
    data.ival = ival;
    CurKind = CC_Integer;
}

CobolConstant :: CobolConstant (double fval)
{
    data.fval = fval;
    CurKind = CC_Float;
}

CobolConstant& CobolConstant :: operator= (char * cval)
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

CobolConstant& CobolConstant :: operator= (long int ival)
{
    if (CurKind == CC_String && data.cval != NULL) {
       delete [] data.cval;
       data.cval = NULL;
    }
    data.ival = ival;
    CurKind = CC_Integer;
    return (*this);
}

CobolConstant& CobolConstant :: operator= (double fval)
{
    if (CurKind == CC_String && data.cval != NULL) {
       delete [] data.cval;
       data.cval = NULL;
    }
    data.fval = fval;
    CurKind = CC_Float;
    return (*this);
}

CobolConstant& CobolConstant :: operator= (StackEntry * se)
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

void CobolConstant :: WriteTextStream (ostream& os)
{
    switch (CurKind) {
       case CC_String:	os << "\"" << data.cval << "\"";	break;
       case CC_Integer:	os << data.ival;	break;
       case CC_Float:	os << data.fval;	break;
       default:		WriteError ("constant type unknown for writing");
       			break;
    }
}

void CobolConstant :: GenWrite (ostream& os, char * stream)
{
    GenIndent();
    os << "_WriteStringVar (" << stream << ", \"";
    switch (CurKind) {
       case CC_String:	os << data.cval;	break;
       case CC_Integer:	os << data.ival;	break;
       case CC_Float:	os << data.fval;	break;
       default:		WriteError ("constant type unknown for writing");
       			break;
    }
    os << "\", NULL);\n";
}

CobolConstant :: ~CobolConstant (void)
{
    if (CurKind == CC_String && data.cval != NULL) {
       delete [] data.cval;
       data.cval = NULL;
    }
}

