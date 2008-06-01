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
#include "symbase.h"
#include "symconst.h"
#include "symlabel.h"

extern long int ival;
extern double fval;
extern char StringBuffer[];
extern SEOperatorKind opkind;
extern int CurrentLine;
extern CobolLabel * CurPar;
extern BOOL CodeBegan;

BOOL ErrorFlag = FALSE;
BOOL CursesUsed = FALSE;

void WriteError (char * str)
{
    if (CodeBegan && CurPar != NULL) {
       cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
       cerr << "in paragraph " << *CurPar << ":\n";
    }
    cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
    cerr << "error: ";
    cerr << str << ".\n";
#ifndef NDEBUG
    PrintStack();
#endif
    ErrorFlag = TRUE;
}

void WriteWarning (char * str)
{
    if (CodeBegan && CurPar != NULL) {
       cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
       cerr << "in paragraph " << *CurPar << ":\n";
    }
    cerr << CobcyConfig.SourceFile << ":" << CurrentLine << ": ";
    cerr << "warning: ";
    cerr << str << ".\n";
#ifndef NDEBUG
    PrintStack();
#endif
}

#ifndef NDEBUG
void PrintStackEntry (StackEntry * se)
{
    switch (se->kind) {
	case SE_Error: 	cerr << "error token"; 	break;
	case SE_Mark: 	cerr << "mark"; 		break;
	case SE_Null:	cerr << "null";		break;
	case SE_Integer:
			cerr << "integer " << se->ival;
			break;
	case SE_Float:
			cerr << "float " << se->fval;
			break;
	case SE_String: 
			cerr << "string " << se->ident;
			break;
	case SE_Identifier:
			cerr << "identifier " << se->ident;
			break;
	case SE_Picture:
			cerr << "picture [" << se->ident << "]";	
			break;
	case SE_Bool:		cerr << "boolean";	break;
	case SE_Connector:	cerr << "connector";	break;
	case SE_Operator:	cerr << "operator";	break;
	case SE_Quote:		cerr << "quote";	break;
    }
}

void PrintStack (void)
{
StackEntry * se;
Stack<StackEntry> TempStack;
int index = 1;

    cerr << "Stack:\n";
    while (!SemStack.IsEmpty()) {
       se = SemStack.Pop();
       cerr << "\t" << index++ << ") ";
       PrintStackEntry (se);
       cerr << "\n";
       TempStack.Push (se);
    }
    while (!TempStack.IsEmpty())
       SemStack.Push (TempStack.Pop());
}
#endif

void NIY (char * str)
{
char MessageBuf [80];

    GenIndent();
    sprintf (MessageBuf, "%s is unimplemented", str);
    codef << "/* " << MessageBuf << " */\n";
    WriteWarning (MessageBuf);
}

void GenComment (char * str)
{
    GenIndent();
    codef << "/* " << str << " */\n";
}

BOOL ErrorOccured (void)
{
#ifndef NDEBUG
    cerr << "ErrorOccured: ";
    cerr << (ErrorFlag ? "TRUE" : "FALSE");
    cerr << "\n";
#endif
    return (ErrorFlag);
}

void PrintConstant (StackEntry * entry, ostream& os)
{
CobolConstant ctp;	// Just to keep all the stuff in one place
    ctp = entry;
    os << ctp;
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

void PrintIdentifier (char * id, ostream& os)
{
    CobolSymbol* sym = LookupIdentifier (id);
    if (sym != NULL)
	os << *sym;
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

void SetCursesUsed (BOOL newValue)
{
    CursesUsed = newValue;
}

BOOL GetCursesUsed (void)
{
    return (CursesUsed);
}

