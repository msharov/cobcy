/* semarith.cc
**
**	Implements semantic arithmetic for COBOL compiler.
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include <math.h>

BOOL RoundResult = FALSE;

void GenMove (void)
{
WORD nIds, i;
CobolSymbol * src = NULL, * dest = NULL;
StackEntry ** prms, * CurEntry = NULL;
char SrcPrefix[80], DestPrefix[80];
typedef StackEntry * StackEntryPtr;

    // Pop all stack entries
    nIds = CountIdentifiers();
    prms = new StackEntryPtr [nIds + 1];
    for (i = 0; i < nIds; ++ i)		// the move to
       prms[i + 1] = SemStack.Pop();
    prms[0] = SemStack.Pop();		// the move from

    // First get the value (or identifier) to assign to everything
    CurEntry = prms[0];
    if (CurEntry->kind == SE_Identifier) {
       if ((src = LookupIdentifier (CurEntry->ident)) == NULL)
	  return;
       BuildPrefix (CurEntry->ident, SrcPrefix);
    }

    for (i = 0; i < nIds; ++ i) {
       CurEntry = prms[i + 1];
       if ((dest = LookupIdentifier (CurEntry->ident)) == NULL)
	  return;
       BuildPrefix (CurEntry->ident, DestPrefix);
       delete CurEntry;

       GenIndent();
       switch (dest->Picture.Kind) {
	  case PictureType::String:
	       // Numerical variables need to be printed for copying
	       if (src != NULL) {
		  if (src->Picture.Kind == PictureType::Integer) {
		     codef << "_IntegerToString (_tmpbuf, ";
		     codef << SrcPrefix << src->CName << ", ";
		     codef << src->Picture.Text << ");\n";
		  }
		  else if (src->Picture.Kind == PictureType::Float) {
		     codef << "_FloatToString (_tmpbuf, ";
		     codef << SrcPrefix << src->CName << ", ";
		     codef << src->Picture.Text << ");\n";
		  }
	       }

	       codef << "_AssignVarString (";
	       codef << DestPrefix << dest->CName;
	       if (prms[0]->kind == SE_Identifier) {
		  if (src->Picture.Kind == PictureType::String)
		     codef << ", " << SrcPrefix << src->CName;
		  else
		     codef << ", _tmpbuf";
		  codef << ", " << dest->Picture.Size;
		  codef << ", " << src->Picture.Size;
	       }
	       else if (prms[0]->kind == SE_String) {
		  codef << ", ";
		  PrintConstant (prms[0], codef);
		  codef << ", " << dest->Picture.Size;
		  codef << ", 0";
	       }
	       else {
		  WriteError ("Incompatible types for MOVE");
		  break;
	       }
	       codef << ");\n";
	       break;
	  case PictureType::Integer:
	  case PictureType::Float:
	       codef << DestPrefix << dest->CName;
	       codef << " = ";
	       if (prms[0]->kind == SE_Identifier)
		  codef << SrcPrefix << src->CName;
	       else if (prms[0]->kind == SE_Integer ||
			prms[0]->kind == SE_Float) 
		  PrintConstant (prms[0], codef);
	       else {
		  WriteError ("Incompatible types for MOVE");
		  break;
	       }

	       // Truncate the result to fit the picture
	       codef << " % 1";
	       for (i = 0; i < dest->Picture.nDigitsBDP; ++ i)
		  codef << "0";

	       codef << ";\n";
	       break;
	  default:
	       WriteError ("Invalid picture type of operand");
	       break;
       }
    }
    delete prms[0];
    delete [] prms;
}

void SetResultRounding (void)
{
    RoundResult = TRUE;
}

static void GenericArithmetic (char * OpName, BOOL SourceFirst, char OpChar)
{
WORD nIds, i;
CobolSymbol * src = NULL, * dest = NULL;
StackEntry ** prms, * SrcEntry = NULL, * CurEntry = NULL, * DestEntry = NULL;
char SrcPrefix[80], DestPrefix[80];
char ErrorBuffer[80];
BOOL ZeroDivCheck = FALSE;
typedef StackEntry * StackEntryPtr;

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

    prms = new StackEntryPtr [nIds];
    for (i = 0; i < nIds; ++ i)
       prms[i] = SemStack.Pop();

    if (SourceFirst)
       SrcEntry = SemStack.Pop();

    // First get the value (or identifier) of source
    if (SrcEntry->kind == SE_Identifier) {
       if ((src = SymTable.Lookup (SrcEntry->ident)) == NULL) {
	  sprintf (ErrorBuffer, "Bad source operand %s to %s", 
	  		SrcEntry->ident, OpName);
	  WriteError (ErrorBuffer);
	  return;
       }
       BuildPrefix (SrcEntry->ident, SrcPrefix);

       // Generate a check for x/0 for divide
       if (OpChar == '/') {
          GenIndent();
          codef << "if (" << SrcPrefix << src->CName << " != 0)\n";
	  ZeroDivCheck = TRUE;
          ++ NestingLevel;
       }
    }

    if (DestEntry->kind == SE_Null) {
       // Perform the operation on every dest parameter
       for (i = 0; i < nIds; ++ i) {
	  CurEntry = prms[i];
	  if ((dest = SymTable.Lookup (CurEntry->ident)) == NULL) {
	     sprintf (ErrorBuffer, "Bad dest operand %s to %s", 
			   CurEntry->ident, OpName);
	     WriteError (ErrorBuffer);
	     return;
	  }
	  BuildPrefix (CurEntry->ident, DestPrefix);
	  delete CurEntry;

	  GenIndent();
	  switch (dest->Picture.Kind) {
	     case PictureType::Integer:
	     case PictureType::Float:
			   codef << DestPrefix << dest->CName;
			   codef << " " << OpChar << "= ";
			   if (SrcEntry->kind == SE_Identifier)
			      codef << SrcPrefix << src->CName;
			   else if (SrcEntry->kind == SE_Integer ||
				    SrcEntry->kind == SE_Float) 
			      PrintConstant (SrcEntry, codef);
			   else {
			      sprintf (ErrorBuffer, "Incompatible types to %s", 
					    OpName);
			      WriteError (ErrorBuffer);
			   }
			   codef << ";\n";
			   break;
	     default:
			   sprintf (ErrorBuffer, "Bad type of operand %s in \
			   		%s", CurEntry->ident, OpName);
			   WriteError (ErrorBuffer);
			   break;
	  }
       }
    }
    else {
       if ((dest = SymTable.Lookup (DestEntry->ident)) == NULL) {
	  sprintf (ErrorBuffer, "Bad dest operand %s to %s", 
			CurEntry->ident, OpName);
	  WriteError (ErrorBuffer);
	  return;
       }
       BuildPrefix (dest->CobolName, DestPrefix);

       GenIndent();
       codef << DestPrefix << dest->CName << " = ";

       if (RoundResult)
	  codef << "_RoundResult (";

       // Perform the operation on every dest parameter
       CurEntry = prms[0];
       if (CurEntry->kind == SE_Identifier) {
	  if ((dest = SymTable.Lookup (CurEntry->ident)) == NULL) {
	     sprintf (ErrorBuffer, "Bad src operand %s to %s", 
			   CurEntry->ident, OpName);
	     WriteError (ErrorBuffer);
	     return;
	  }
	  BuildPrefix (CurEntry->ident, DestPrefix);
       }

       switch (dest->Picture.Kind) {
	  case PictureType::Integer:
	  case PictureType::Float:
	  		if (RoundResult)
			   codef << "(double) ";
			if (CurEntry->kind == SE_Identifier)
			   codef << DestPrefix << dest->CName;
			else
			   PrintConstant (CurEntry, codef);
			break;
	  default:
			sprintf (ErrorBuffer, "Bad type of operand %s in \
				     %s", CurEntry->ident, OpName);
			WriteError (ErrorBuffer);
			break;
       }

       codef << " " << OpChar << " ";

       if (RoundResult)
	  codef << "(double) ";
       if (SrcEntry->kind == SE_Identifier)
	  codef << SrcPrefix << src->CName;
       else
	  PrintConstant (SrcEntry, codef);

       delete CurEntry;

       if (RoundResult)
	  codef << ")";
       codef << ";\n";
    }

    // Close check for zero
    if (ZeroDivCheck)
       -- NestingLevel;

    delete SrcEntry;
    delete [] prms;
    RoundResult = FALSE;
}

void GenAdd (void)
{
    GenericArithmetic ("ADD", TRUE, '+');
}

void GenSubtract (void)
{
    GenericArithmetic ("SUBTRACT", TRUE, '-');
}

void GenMultiply (void)
{
    GenericArithmetic ("MULTIPLY", FALSE, '*');
}

void GenDivide (void)
{
    GenericArithmetic ("DIVIDE", FALSE, '/');
}

void GenCompute (void)
{
int nArgs, i;
StackEntry ** prms;
typedef StackEntry * StackEntryPtr;
CobolSymbol * sym;

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
       codef << ")";
       RoundResult = FALSE;
    }
    codef << ";\n";
    
    for (i = 0; i < nArgs; ++ i)
       delete prms[i];
    delete [] prms;
}

