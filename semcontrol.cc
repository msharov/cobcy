/* semcontrol.cc
 **
 **	Control statement semantics.
 */

#ifdef __MSDOS__
#include "semexter.h"
#include "semcontr.h"
#else
#include "semextern.h"
#include "semcontrol.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include "symlabel.h"
#include "symvar.h"
#include "symconst.h"

// This is just a very rough estimate of the maximum
//	It will not really place the limit on how many paragraphs
//	you may have, but if you have 40000 paragraphs and use
//	STOP RUN in paragraph 3, it will not work!
#define MAX_PARAGRAPHS		32000

/*-----------------------| Globals |--------------------------*/
Queue<CobolLabel> 	ParagraphList;
CobolLabel *		CurPar = NULL;
char			CurLoopVar [32] = "";
int			LoopNesting = 0;
/*------------------------------------------------------------*/

void GenParagraph (void)
{
    CobolLabel * NewLabel;
    StackEntry * CurEntry;
    char ErrorBuffer [80];

    CurEntry = SemStack.Pop();

    if ((NewLabel = (CobolLabel*) SymTable.Lookup (CurEntry->ident)) == NULL) {
	// Allocate new symbol table entry
	NewLabel = new CobolLabel;
	NewLabel->SetName (CurEntry->ident);
	NewLabel->Undeclared = FALSE;
	// Enter it in the table
#ifndef NDEBUG
	cout << "DBG: Inserting paragraph " << CurEntry->ident;
	cout << " into symbol table.\n";
#endif
	SymTable.Insert (CurEntry->ident, NewLabel);
    }
    else {
	if (NewLabel->Kind() == CS_Label && NewLabel->Undeclared)
	    NewLabel->Undeclared = FALSE;
	else {
	    sprintf (ErrorBuffer, "name %s is already used", 
		    NewLabel->GetName());
	    WriteError (ErrorBuffer);
	    delete CurEntry;
	    return;
	}
    }
    delete (CurEntry);

    // A paragraph finishes previous proc and starts a new one.
    GenEndProc();

    // Then declare a new function
    NewLabel->GenDeclare (codef);
    // Change the current paragraph
    CurPar = NewLabel;

    // Setup for insertion in main proc
    ParagraphList.Append (NewLabel);
}

void GenCall (void)
{
    typedef struct {
	enum { constant, variable } id;
	StackEntry*	pc;
	CobolData*	pv;
    } SArg;

    int nArgs = CountIdentifiers();
    SArg* argArray = NULL;
    if (nArgs > 0) {
	argArray = new SArg [nArgs];
	assert (argArray != NULL);
	for (int i = 0; i < nArgs; ++ i) {
	    StackEntryPtr pEntry = SemStack.Pop();
	    argArray[i].id = SArg::variable;
	    if (pEntry->kind == SE_Identifier) {
		argArray[i].pv = (CobolData*) SymTable.Lookup (pEntry->ident);
		if (argArray[i].pv == NULL)
		    WriteError ("unknown argument to CALL");
	    }
	    else if (pEntry->kind == SE_String)
		argArray[i].pc = pEntry;
	    else
		WriteError ("invalid function name to CALL. Must be ID or String");
	}
    }
    int nFuncs = CountIdentifiers();
    if (nFuncs == 0)
	WriteError ("no function names to CALL");
    for (int i = 0; i < nFuncs; ++ i) {
	StackEntryPtr pEntry = SemStack.Pop();
	GenIndent();
	if (pEntry->kind == SE_Identifier) {
	    CobolData* pv = (CobolData*) SymTable.Lookup (pEntry->ident);
	    if (pv == NULL) {
		WriteError ("unknown argument to CALL");
		continue;
	    }
	    codef << *pv;
	}
	else if (pEntry->kind == SE_String)
	    codef << pEntry->ident;
	else
	    WriteError ("invalid argument to CALL");
	codef << " (";
	for (int j = 0; j < nArgs; ++ j) {
	    if (argArray[j].id == SArg::variable && argArray[j].pv != NULL)
		codef << *argArray[j].pv;
	    else if (argArray[j].id == SArg::constant)
		codef << argArray[j].pc->ident;
	    if (j < nArgs - 1)
		codef << ", ";
	}
	codef << ");\n";
    }
    for (int j = 0; j < nArgs; ++ j)
	if (argArray[j].id == SArg::constant)
	    delete argArray[j].pc;
}

void GenGoto (void)
{
    CobolLabel * DestLabel;
    StackEntry * CurEntry;

    CurEntry = SemStack.Pop();

    if ((DestLabel = (CobolLabel*) SymTable.Lookup (CurEntry->ident)) == NULL) {
	DestLabel = new CobolLabel;
	DestLabel->SetName (CurEntry->ident);
#ifndef NDEBUG
	cout << "DBG: Forward declaring label " << CurEntry->ident << "\n";
#endif
	DestLabel->Undeclared = TRUE;
	SymTable.Insert (CurEntry->ident, DestLabel);
    }
    delete (CurEntry);

    CurPar->GenJump (codef, DestLabel);
}

void GenPerform (void)
{
    StackEntry * Proc, * EndProc, * Count, * UntilOption;
    CobolLabel * ProcAttr, * EndProcAttr = NULL;

    UntilOption = SemStack.Pop();
    Count = SemStack.Pop();
    EndProc = SemStack.Pop();
    Proc = SemStack.Pop();

    if ((ProcAttr = (CobolLabel*) SymTable.Lookup (Proc->ident)) == NULL) {
	ProcAttr = new CobolLabel;
	ProcAttr->SetName (Proc->ident);
	ProcAttr->Undeclared = TRUE;
#ifndef NDEBUG
	cout << "DBG: Forward declaring proc " << Proc->ident << "\n";
#endif
	SymTable.Insert (Proc->ident, ProcAttr);
    }

    if (EndProc->kind == SE_Identifier &&
	    (EndProcAttr = (CobolLabel*) SymTable.Lookup (EndProc->ident)) == NULL)
    {
	EndProcAttr = new CobolLabel;
	EndProcAttr->SetName (EndProc->ident);
	EndProcAttr->Undeclared = TRUE;
#ifndef NDEBUG
	cout << "DBG: Forward declaring end proc " << EndProc->ident << "\n";
#endif
	SymTable.Insert (EndProc->ident, EndProcAttr);
    }

    if (UntilOption->kind == SE_Bool) {	// marking an UNTIL
	GenIndent(); codef << "do\n";
	GenIndent(); codef << "{\n";
	++ NestingLevel;
    }
    if (Count->ival > 1) {
	GenIndent();
	codef << "for (_index = 0; _index < " << Count->ival;
	codef << "; ++ _index)\n";
	++ NestingLevel;
    }

    GenIndent();
    if (EndProcAttr == NULL) {
	ProcAttr->WriteTextStream (codef);
	codef << "();\n";
    }
    else {
	codef << "_CallParagraphSequence (_pi_";
	ProcAttr->WriteTextStream (codef);
	codef << ", _pi_";
	EndProcAttr->WriteTextStream (codef);
	codef << ");\n";
    }

    if (Count->ival > 1)
	-- NestingLevel;

    // Close the UNTIL loop and prepare for the booleans
    if (UntilOption->kind == SE_Bool) {
	-- NestingLevel;
	GenIndent(); codef << "}\n";
	GenIndent(); codef << "while (";
    }

    // Take care of the loops generated by VARYING statements
    while (LoopNesting > 0) {
	-- LoopNesting;
	-- NestingLevel;
    }

    delete Count;
    delete Proc;
    delete EndProc;
    delete UntilOption;
}

void GenEndPerformUntil (void)
{
    codef << ");\n";
}

// This ends either a procedure or a paragraph
void GenEndProc (void)
{
    // A paragraph finishes previous proc and starts a new one.
    GenIndent();
    codef << "return(1);\n";

    -- NestingLevel;
    GenIndent();
    codef << "}\n\n";
}

void BeginCompound (void)
{
    GenIndent();
    codef << "{\n";
    ++ NestingLevel;
}

void EndCompound (void)
{
    -- NestingLevel;
    GenIndent();
    codef << "}\n";
}

void GenStartProc (void)
{
    GenParagraph();
}

void GenStartIf (void)
{
    GenIndent();
    codef << "if (";
}

void GenStartElsif (void)
{
    GenIndent();
    codef << "else if (";
}

void GenEndIf (void)
{
    codef << ")\n";
}

void GenElse (void)
{
    GenIndent();
    codef << "else\n";
}

void GenBool (void)
{
    StackEntry *entry[3];
    CobolVar *attrs[2];
    int i;
#ifndef NDEBUG
    cout << "\tIn GenBool\n";
#endif

    entry[0] = SemStack.Pop(); 
    entry[1] = SemStack.Pop();

    if (!(strcmp(entry[0]->ident,"alphabetic")) ||
	     !(strcmp(entry[0]->ident,"alphabetic-upper")) ||
	     !(strcmp(entry[0]->ident,"alphabetic-lower")))
    {  
	if ((attrs[0] = (CobolVar*) LookupIdentifier (entry[1]->ident)) == NULL) {
	    WriteError ("unknown identifier in boolean");
	    return;
	}

	if (attrs[0]->IsNumeric()) {
	    WriteError ("cannot do alphabetic tests on non-strings");
	    return;
	}

	if (!strcmp(entry[0]->ident,"alphabetic"))
	    codef << "(_Alphabetic (" << *attrs[0] << "))";
	if (!strcmp(entry[0]->ident,"alphabetic-upper"))
	    codef << "(_AlphabeticCase (" << *attrs[0] << ",2))";
	if (!strcmp(entry[0]->ident,"alphabetic-lower"))
	    codef << "(_AlphabeticCase (" << *attrs[0] << ",1))";

	delete entry[0]; 
	delete entry[1];
    }
    else {
	entry[2] = SemStack.Pop();

	if (entry[0]->kind == SE_Identifier)
	    if ((attrs[0] = (CobolVar*) LookupIdentifier (entry[0]->ident)) == NULL)
		return;
	if (entry[2]->kind == SE_Identifier)
	    if ((attrs[1] = (CobolVar*) LookupIdentifier (entry[2]->ident)) == NULL)
		return;

 	if (attrs[0]->IsNumeric() != attrs[2]->IsNumeric())
	    WriteError ("comparing numeric and non-numeric value");

	if (entry[0]->kind == SE_Identifier && !attrs[0]->IsNumeric()) {
	    #ifndef NDEBUG
	        cout << "\tIn GenBool Character\n";
	    #endif
	    codef << "(strcmp(";
	    if (entry[2]->kind == SE_Identifier)
		attrs[1]->WriteTextStream (codef); 
	    else
		PrintConstant (entry[2], codef); 
	    codef << " , ";
	    if (entry[0]->kind == SE_Identifier)
		attrs[0]->WriteTextStream (codef);
	    else
		PrintConstant (entry[0], codef);
	    codef <<") " << entry[1]->ident << " 0 ";
	    codef << ")";
	}
	else {
	    #ifndef NDEBUG
	    	cout << "\tIn GenBool Numeric\n";
	    #endif
	    codef << "(";
	    if (entry[2]->kind == SE_Identifier)
		attrs[1]->WriteTextStream (codef); 
	    else
		PrintConstant (entry[2], codef); 
	    codef << " " << entry[1]->ident << " ";
	    if (entry[0]->kind == SE_Identifier)
		attrs[0]->WriteTextStream (codef);
	    else
		PrintConstant (entry[0], codef);
	    codef << ")";
	}
	for (i = 0; i < 3; ++ i)
	    delete entry[i];
    } 
}

void GenConnect (void)
{
    StackEntry * CurEntry;

    CurEntry = SemStack.Pop();
    codef << " " << CurEntry->ident << " ";
    delete (CurEntry);
}

void GenStopRun (void)
{
    // The point is to make cpi large enough to quit.
    //	Nesting level is obviously important, since an overflow
    //	may occur when adding 1 to cpi each iteration...
    //	But that should not happen until you nest MAXINT paragraphs,
    //	which is a very big number!
    GenIndent();
    codef << "return (" << MAX_PARAGRAPHS + 1 << ");\n";
}

void GenParagraphCalls (void)
{
    CobolLabel * CurLabel;
    Queue<CobolLabel> TempBufQueue;
    long int pi = 0, nPars = 0;

    // First count the paragraphs
    while (!ParagraphList.IsEmpty()) {
	TempBufQueue.Append (ParagraphList.Serve());
	++ nPars;
    }
    // Now make the references, restoring ParagraphList for further analysis
    declf << "const int _pi__FirstParagraph = 0;\n";
    for (pi = 1; pi <= nPars; ++ pi) {
	CurLabel = TempBufQueue.Serve();
	declf << "const int _pi_";
	CurLabel->WriteTextStream (declf);
	declf << " = " << pi << ";\n";
	ParagraphList.Append (CurLabel);
    }

    // The calling structure is basically a while loop with a switch statement
    //	inside. There is a current paragraph variable _cpi, which is parsed in
    //	that switch statement to decide which paragraph to execute. This is
    //	done to support complex calling chains with gotos.
    //	And it is <= because you can call par 10 if there are 10 pars
    GenIndent(), codef << "_cpi = _startCpi;\n";
    GenIndent(), codef << "if (_endCpi < 0 || _endCpi > " << nPars << ")\n";
    ++ NestingLevel;
    GenIndent(), codef << "_endCpi = " << nPars << ";\n";
    -- NestingLevel;
    GenIndent(), codef << "while (_cpi <= _endCpi)\n";
    GenIndent(), codef << "{\n";
    ++ NestingLevel;

    // Debugging information is generated with -g switch
    if (CobcyConfig.GenDebug) {
	GenIndent();
	codef << "printf (\"DEBUG: _cpi = %ld in %ld paragraphs.\\n\"";
	codef << ", _cpi, " << nPars << ");\n";
    }

    // This is the switch statement
    GenIndent();
    codef << "switch (_cpi)\n";
    GenIndent();
    codef << "{\n";
    ++ NestingLevel;

    // Each paragraph call will return the displacement from current
    GenIndent();
    codef << "case 0: _cpi += _FirstParagraph(); break;\n";
    // Could use the above defined constants here, but they would just
    //	add more clutter :)
    pi = 0;
    while (!ParagraphList.IsEmpty()) {
	++ pi;
	CurLabel = ParagraphList.Serve();
	GenIndent();
	codef << "case " << pi << ": _cpi += ";
	CurLabel->WriteTextStream (codef);
	codef << "(); break;\n";
    }

    // This should never happen, but if it does, quit gracefully
    GenIndent();
    codef << "default:\n";
    NestingLevel += 2;
    GenIndent();
    codef << "fprintf (stderr, \"Broken paragraph chain!\\n\");\n";
    GenIndent();
    codef << "_cpi = " << nPars + 1 << ";\n";
    GenIndent();
    codef << "break;\n";
    NestingLevel -= 2;

    // First close the switch, then while loop
    -- NestingLevel;
    GenIndent();
    codef << "}\n";
    -- NestingLevel;
    GenIndent();
    codef << "}\n";
}

void GenStartLoop (void)
{
    GenIndent();
    codef << "for (";
    ++ LoopNesting;
    ++ NestingLevel;
}

void GenLoopInit (void)
{
    StackEntry *ivar, *ival;    // Stack contains the iterator variable and its
    // 	initial value

    ival = SemStack.Pop();
    ivar = SemStack.Pop();
    strcpy (CurLoopVar, ivar->ident);

    // Generate the x = 0 type assignment in the beginning of the loop
    // Note: no indent is generated.
    PrintIdentifier (CurLoopVar, codef);

    codef << " = ";
    if (ival->kind == SE_Identifier)
	PrintIdentifier (ival->ident, codef);
    else
	PrintConstant (ival, codef);
    codef << "; ";

    // This will be followed by a boolean or an end-value condition, which is
    //	a special case of a boolean.

    delete (ival);
    delete (ivar);
}

// This proc handles the case VARYING x FROM y TO z
//	The bug here (feature?) is that you cannot say 
//	'varying x from 10 to 1 by -1'
//	The problem is that from, to, and increment values can be variables.
void GenLoopCondition (void)
{
    StackEntry * endval;

    endval = SemStack.Pop();

    PrintIdentifier (CurLoopVar, codef);
    codef << " <= ";
    if (endval->kind == SE_Identifier)
	PrintIdentifier (endval->ident, codef);
    else
	PrintConstant (endval, codef);

    // Let GenLoopIncrement take care of the semicolon

    delete (endval);
}

// This simply handles the BY option
//	Note that the increment can be a variable
void GenLoopIncrement (void)
{
    StackEntry * incr;

    incr = SemStack.Pop();

    // First, close the booleans
    codef << "; ";

    PrintIdentifier (CurLoopVar, codef);
    codef << " += ";
    if (incr->kind == SE_Identifier)
	PrintIdentifier (incr->ident, codef);
    else
	PrintConstant (incr, codef);
    codef << ")\n";
    // No braces are needed, since there will only be one statement in the
    //	innermost loop (the procedure call)

    delete (incr);
}

void GenEmptyClause (void)
{
    GenIndent();
    GenIndent();
    codef << ";\n";
}

