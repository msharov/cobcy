/* semutil.cc
**
**	Implements semantic utilities for COBOL compiler.
*/

#ifdef __MSDOS__
#include "semexter.h"		// includes semutil.h
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif

extern long int ival;
extern double fval;
extern char StringBuffer[];
extern SEOperatorKind opkind;

BOOL ErrorFlag = FALSE;

void WriteError (char * str)
{
    cerr << ">>> ERROR: " << str << ".\n";
    ErrorFlag = TRUE;
}

void WriteWarning (char * str)
{
    cerr << ">>> WARNING: " << str << ".\n";
}

void NIY (char * str)
{
    GenIndent();
    codef << "\n/* " << str <<  " not implemented yet */" <<  "\n";
}

void Comment (char * str)
{
    GenIndent();
    codef << "/* " << str << " */\n";
}

BOOL ErrorOccured (void)
{
    return (ErrorFlag);
}

void PrintConstant (StackEntry * entry, ofstream& os)
{
    switch (entry->kind) {
        case SE_String:		os << "\"" << entry->ident << "\"";
				break;
        case SE_Integer:	os << entry->ival;
				break;
        case SE_Float:		os << entry->fval;
				break;
	default:		WriteError ("Bad constant type");
				break;
    }
}

CobolSymbol * LookupIdentifier (char * id)
{
CobolSymbol * sym;
char ErrorBuffer [80];

    if ((sym = SymTable.Lookup (id)) == NULL) {
       sprintf (ErrorBuffer, "identifier '%s' is unknown", id);
       WriteError (ErrorBuffer);
       return (NULL);
    }
    return (sym);
}

void PrintIdentifier (char * id, ofstream& os)
{
CobolSymbol * sym;
char SymPrefix [80];

    sym = LookupIdentifier (id);
    BuildPrefix (id, SymPrefix);
    os << SymPrefix << sym->CName;
}

void ReadVariable (CobolSymbol * attr, ofstream& os, char * stream, BOOL nl)
{
char prefix [80];

    BuildPrefix (attr->CobolName, prefix);
    GenIndent();
    switch (attr->Picture.Kind) {
       case PictureType::String: 
			  os << "_ReadStringVar (";
			  os << stream << ", ";
			  break;
       case PictureType::Integer: 
			  os << "_ReadIntegerVar (";
			  os << stream << ", ";
			  break;
       case PictureType::Float: 
			  os << "_ReadFloatVar (";
			  os << stream << ", ";
			  break;
       case PictureType::Undefined:	
			  WriteError ("Undefined picture type");
			  break;
       default:		  WriteError ("Invalid picture type");
			  break;
    }
    os << prefix << attr->CName;
    os << ", \"" << attr->Picture.Text << "\");\n";

    if (nl) {
       GenIndent();
       os << "fgetc (" << stream << ");\n";	// Newline
    }
}

void PrintVariable (CobolSymbol * attr, ofstream& os, char * stream, BOOL nl)
{
char prefix [80];

    BuildPrefix (attr->CobolName, prefix);
    GenIndent();
    switch (attr->Picture.Kind) {
       case PictureType::String: 
			  os << "_WriteStringVar (";
			  os << stream << ", ";
			  break;
       case PictureType::Integer: 
			  os << "_WriteIntegerVar (";
			  os << stream << ", ";
			  break;
       case PictureType::Float: 
			  os << "_WriteFloatVar (";
			  os << stream << ", ";
			  break;
       case PictureType::Undefined:	
			  WriteError ("Undefined picture type");
			  break;
       default:		  WriteError ("Invalid picture type");
			  break;
    }
    os << prefix << attr->CName;
    os << ", \"" << attr->Picture.Text << "\");\n";

    if (nl) {
       GenIndent();
       os << "fprintf (" << stream << ", \"\\n\");\n";
    }
}

void ReadRecord (CobolSymbol * attr, ofstream& os, char * stream, BOOL nl)
{
int i;
CobolSymbol * child;

    attr->ChildList.Head();
    for (i = 0; i < attr->nChildren; ++ i) {
       child = attr->ChildList.LookAt();
       
       if (child->Kind == CobolSymbol::Record)
	  ReadRecord (child, os, stream, FALSE);
       else
	  ReadVariable (child, os, stream, FALSE);

       attr->ChildList.Next();
    }
    if (nl) {
       GenIndent();
       os << "fgetc (" << stream << ");\n";	// Newline
    }
}

void PrintRecord (CobolSymbol * attr, ofstream& os, char * stream, BOOL nl)
{
int i;
CobolSymbol * child;

    attr->ChildList.Head();
    for (i = 0; i < attr->nChildren; ++ i) {
       child = attr->ChildList.LookAt();
       
       if (child->Kind == CobolSymbol::Record)
	  PrintRecord (child, os, stream, FALSE);
       else
	  PrintVariable (child, os, stream, FALSE);

       attr->ChildList.Next();
    }
    if (nl) {
       GenIndent();
       os << "fprintf (" << stream << ", \"\\n\");\n";
    }
}

void LookupParent (char * symbol, char * prefix)
{
CobolSymbol * attr;

    if ((attr = SymTable.Lookup (symbol)) == NULL) {
       WriteError ("Unknown parent");
       return;
    }

    if (strlen (attr->ParentCName) > 0)
       LookupParent (attr->ParentCName, prefix);

    strcat (prefix, attr->CName);
    strcat (prefix, ".");
}

void BuildPrefix (char * symbol, char * prefix)
{
CobolSymbol * attr;
char ErrorBuffer [80];

    if ((attr = SymTable.Lookup (symbol)) == NULL) {
       sprintf (ErrorBuffer, "Cannot find symbol %s in BuildPrefix", symbol);
       WriteError (ErrorBuffer);
       return;
    }

    memset (prefix, 0, strlen (attr->ParentCName) + 2);

    if (strlen (attr->ParentCName) > 0)
       LookupParent (attr->ParentCName, prefix);
}

WORD CountIdentifiers (void)
{
Stack<StackEntry> TempStack;
StackEntry * CurEntry;
WORD count = 0;

    do {
       CurEntry = SemStack.Pop();
       TempStack.Push (CurEntry);
       ++ count;
    }
    while (CurEntry->kind != SE_Mark && !SemStack.IsEmpty());

    if (CurEntry->kind != SE_Mark)
       WriteError ("No mark found");

    -- count;			// Subtract the mark
    delete (TempStack.Pop());	// Delete the mark

    while (!TempStack.IsEmpty())
       SemStack.Push (TempStack.Pop());

    return (count);
}

void ReverseIdentifiers (WORD nIds)
{
Queue<StackEntry> TempQueue;
WORD i;

    for (i = 0; i < nIds; ++ i)
       TempQueue.Append (SemStack.Pop());
    for (i = 0; i < nIds; ++ i)
       SemStack.Push (TempQueue.Serve());
}

void GenIndent (void)
{
int i;
    for (i = 0; i < NestingLevel; ++ i)
       codef << "    ";
}

void Push (StackEntryKind kind)
{
StackEntry * NewEntry;

    NewEntry = new StackEntry;
    NewEntry->kind = kind;
    memset (NewEntry->ident, 0, STACK_IDENT_LENGTH);
    NewEntry->fval = NewEntry->ival = 0;

    switch (kind) {
	case SE_Integer:
		NewEntry->ival = ival;
		break;
	case SE_Float:
		NewEntry->fval = fval;
		break;
	case SE_String:
	case SE_Identifier:
	case SE_Picture:
	case SE_Bool:
	case SE_Connector:
		strcpy (NewEntry->ident, StringBuffer);
		break;
	case SE_Operator:
		NewEntry->opkind = opkind;
		break;
	default:		// For others, simply set the type
		break;
    }

    SemStack.Push (NewEntry);
}

BOOL IsInSet (char c, char * set)
{
register int i, sl;

    sl = strlen (set);
    for (i = 0; i < sl; ++ i) {
       if (c == set[i])
	  return (TRUE);
    }
    return (FALSE);
}

