/* semarith.cc
**
**	Implements semantic arithmetic for COBOL compiler.
*/

#include "semextern.h"
#include "symvar.h"
#include "symrec.h"
#include "symconst.h"

bool RoundResult = false;

void GenMove (void)
{
WORD nIds, i;
CobolVar * src = NULL, * dest = NULL;
CobolConstant csrc;
StackEntry ** prms, * CurEntry = NULL;
typedef StackEntry * StackEntryPtr;
bool ConstantSource = false;

#ifndef NDEBUG
    cout << "\tIn GenMove\n";
#endif

    // Pop all stack entries
    nIds = CountIdentifiers();
    prms = new StackEntryPtr [nIds + 1];
    for (i = 0; i < nIds; ++ i)		// the move to
       prms[i + 1] = SemStack.Pop();
    prms[0] = SemStack.Pop();		// the move from

    // First get the value (or identifier) to assign to everything
    CurEntry = prms[0];
    if (CurEntry->kind == SE_Identifier) {
       if ((src = (CobolVar*) LookupIdentifier (CurEntry->ident)) == NULL) {
	  delete CurEntry;
	  delete prms[0];
	  delete [] prms;
	  return;
       }
    }
    else {
       csrc = CurEntry;
       ConstantSource = true;
    }

    for (i = 0; i < nIds; ++ i) {
       CurEntry = prms[i + 1];
       if ((dest = (CobolVar*) LookupIdentifier (CurEntry->ident)) == NULL) {
	  delete CurEntry;
	  delete prms[0];
	  delete [] prms;
	  return;
       }
       delete CurEntry;

#ifndef NDEBUG
       if (ConstantSource)
	  cout << "\t\tMoving " << csrc << " to " << *dest << "\n";
       else
	  cout << "\t\tMoving " << *src << " to " << *dest << "\n";
#endif

       if (ConstantSource)
	  dest->GenMove (codef, csrc);
       else
	  dest->GenMove (codef, src);
    }
    delete prms[0];
    delete [] prms;
}

void SetResultRounding (void)
{
    RoundResult = true;
}

static void GenericArithmetic (const char* OpName, bool SourceFirst, char OpChar)
{
WORD nIds, i;
CobolVar * dest = NULL;
CobolConstant ConstSrc;
Streamable * src1, * src2;
StackEntry ** prms, * SrcEntry = NULL, * CurEntry = NULL, * DestEntry = NULL;
char ErrorBuffer[80];
typedef StackEntry * StackEntryPtr;
bool ConstantSource = false;

#ifndef NDEBUG
    cout << "\tIn GenericArithmetic " << OpName << "\n";
#endif

    DestEntry = SemStack.Pop();

    if (!SourceFirst)
       SrcEntry = SemStack.Pop();

    nIds = CountIdentifiers();

    // Only two operands can be specified (including SrcEntry) with GIVING opt
    if (DestEntry->kind != SE_Null && nIds > 1) {
       sprintf (ErrorBuffer, "Cannot have multiple operands with GIVING option \
       				in %s", OpName);
       WriteError (ErrorBuffer);

       delete DestEntry;
       if (!SourceFirst)
	  delete SrcEntry;
       return;
    }

    // If giving option is present, the source is last in prms array
    prms = new StackEntryPtr [nIds];
    for (i = 0; i < nIds; ++ i)
       prms[i] = SemStack.Pop();

    if (SourceFirst)
       SrcEntry = SemStack.Pop();

    // First get the value (or identifier) of source
    switch (SrcEntry->kind) {
       case SE_Identifier:
       		src1 = (CobolVar*) LookupIdentifier (SrcEntry->ident);
		break;
       default:
       		ConstSrc = SrcEntry;
		ConstantSource = true;
       		break;
    }

    if (DestEntry->kind == SE_Null) {
       // Perform the operation on every dest parameter
       for (i = 0; i < nIds; ++ i) {
	  CurEntry = prms[i];
	  if ((dest = (CobolVar*) SymTable.Lookup (CurEntry->ident)) == NULL) {
	     sprintf (ErrorBuffer, "Bad dest operand %s to %s", 
			   CurEntry->ident, OpName);
	     WriteError (ErrorBuffer);
	     return;
	  }
	  delete CurEntry;

          if (ConstantSource)
	     dest->GenArith (codef, dest, &ConstSrc, OpChar);
	  else
	     dest->GenArith (codef, dest, src1, OpChar);
       }
    }
    else {
       if ((dest = (CobolVar*) SymTable.Lookup (DestEntry->ident)) == NULL) {
	  sprintf (ErrorBuffer, "Bad dest operand %s to %s", 
			CurEntry->ident, OpName);
	  WriteError (ErrorBuffer);
	  return;
       }

       CurEntry = prms[0];
       if (CurEntry->kind == SE_Identifier) {
	  if ((src2 = SymTable.Lookup (CurEntry->ident)) == NULL) {
	     sprintf (ErrorBuffer, "Bad src operand %s to %s", 
			   CurEntry->ident, OpName);
	     WriteError (ErrorBuffer);
	     return;
	  }
       }

       dest->GenArith (codef, src2, src1, OpChar);

       delete CurEntry;
    }

    delete SrcEntry;
    delete [] prms;
    RoundResult = false;
}

void GenAdd (void)
{
    GenericArithmetic ("ADD", true, '+');
}

void GenSubtract (void)
{
    GenericArithmetic ("SUBTRACT", true, '-');
}

void GenMultiply (void)
{
    GenericArithmetic ("MULTIPLY", false, '*');
}

void GenDivide (void)
{
    GenericArithmetic ("DIVIDE", false, '/');
}

void GenCompute (void)
{
int nArgs, i;
StackEntry ** prms;
typedef StackEntry * StackEntryPtr;
CobolVar * dest;

#ifndef NDEBUG
    cout << "\tIn GenCompute\n";
#endif

    nArgs = CountIdentifiers();

    prms = new StackEntryPtr [nArgs];

    // Pop them so that prms will have them in the same order as in cobol code
    // i.e. prms[0] prms[1] prms[2] ...
    for (i = 0; i < nArgs; ++ i)
       prms[nArgs - (i + 1)] = SemStack.Pop();

    // Corresponding C code is almost identical :)
    GenIndent();
    for (i = 0; i < nArgs; ++ i) {
       switch (prms[i]->kind) {
	  case SE_Identifier:
	  	// i > 0 because we only want casting on the righthand side
		//	entry 0 is the variable where stuff will be placed
	        if (i > 0 && RoundResult)
		   codef << "(double) ";
	  	PrintIdentifier (prms[i]->ident, codef);
		break;
	  case SE_Operator:
	  	switch (prms[i]->opkind) {
		   case OP_Addition:		codef << " + "; break;
		   case OP_Subtraction:		codef << " - "; break;
		   case OP_Multiplication:	codef << " * "; break;
		   case OP_Division:		codef << " / "; break;
		   case OP_LParen:		codef << "("; break;
		   case OP_RParen:		codef << ")"; break;
		   case OP_Equal:		codef << " = "; break;
		}
		break;
	  case SE_Integer:
	  case SE_Float:
	        if (RoundResult)
		   codef << "(double) ";
	  	PrintConstant (prms[i], codef);
		break;
	  default:
	  	WriteError ("Invalid expression in COMPUTE");
		break;
       }

       // This will place the function call right after the equal sign
       if (i == 1 && RoundResult)
	  codef << "_RoundResult (";
    }

    // The whole expression will be on one line...
    if (RoundResult) {
       codef << ", ";
       dest = (CobolVar*) LookupIdentifier (prms[0]->ident);
       if (dest != NULL)
	  dest->WritePicture (codef);
       codef << ")";
       RoundResult = false;
    }
    codef << ";\n";
    
    for (i = 0; i < nArgs; ++ i)
       delete prms[i];
    delete [] prms;
}

