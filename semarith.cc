/* semarith.cc
**
**	Implements semantic arithmetic for COBOL compiler.
*/

#include "semextern.h"
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif

BOOL RoundResult = FALSE;

void GenMove (void)
{
WORD nIds, i;
CobolSymbol * src, * dest;
StackEntry ** prms, * CurEntry;
char SrcPrefix[80], DestPrefix[80];
typedef StackEntry *	StackEntryPtr;

    // Pop all stack entries
    nIds = CountIdentifiers();
    prms = new StackEntryPtr [nIds + 1];
    for (i = 0; i < nIds; ++ i)		// the move to
       prms[i + 1] = SemStack.Pop();
    prms[0] = SemStack.Pop();		// the move from

    // First get the value (or identifier) to assign to everything
    CurEntry = prms[0];
    if (CurEntry->kind == SE_Identifier) {
       if ((src = SymTable.Lookup (CurEntry->ident)) == NULL) {
	  WriteError ("Bad operand 1 to MOVE");
	  return;
       }
       BuildPrefix (CurEntry->ident, SrcPrefix);
    }

    for (i = 0; i < nIds; ++ i) {
       CurEntry = prms[i + 1];
       if ((dest = SymTable.Lookup (CurEntry->ident)) == NULL) {
	  WriteError ("Bad dest operand to MOVE");
	  return;
       }
       BuildPrefix (CurEntry->ident, DestPrefix);
       delete CurEntry;

       GenIndent();
       switch (dest->Picture.Kind) {
	  case PictureType::String:
	  		outfile << "_AssignVarString (";
			outfile << DestPrefix << dest->CName;
			if (prms[0]->kind == SE_Identifier) {
			   outfile << ", " << SrcPrefix << src->CName;
			   outfile << ", " << dest->Picture.Size;
			   outfile << ", " << src->Picture.Size;
		        }
			else if (prms[0]->kind == SE_String) {
			   outfile << ", ";
			   PrintConstant (prms[0], outfile);
			   outfile << ", " << dest->Picture.Size;
			   outfile << ", 0";
			}
			else {
			   WriteError ("Incompatible types for MOVE");
			   break;
		        }
			outfile << ");\n";
			break;
	  case PictureType::Integer:
	  case PictureType::Float:
			outfile << DestPrefix << dest->CName;
			outfile << " = ";
			if (prms[0]->kind == SE_Identifier)
			   outfile << SrcPrefix << src->CName;
			else if (prms[0]->kind == SE_Integer ||
			 	 prms[0]->kind == SE_Float) 
			   PrintConstant (prms[0], outfile);
			else {
			   WriteError ("Incompatible types for MOVE");
			   break;
		        }
			outfile << ";\n";
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
CobolSymbol * src, * dest;
StackEntry ** prms, * SrcEntry, * CurEntry, * DestEntry;
char SrcPrefix[80], DestPrefix[80];
char ErrorBuffer[80];
typedef StackEntry *	StackEntryPtr;

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
    }

    // Generate a check for x/0 for divide
    if (OpChar == '/') {
       GenIndent();
       outfile << "if (" << SrcPrefix << src->CName << " != 0)\n";
       ++ NestingLevel;
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
			   outfile << DestPrefix << dest->CName;
			   outfile << " " << OpChar << "= ";
			   if (SrcEntry->kind == SE_Identifier)
			      outfile << SrcPrefix << src->CName;
			   else if (SrcEntry->kind == SE_Integer ||
				    SrcEntry->kind == SE_Float) 
			      PrintConstant (SrcEntry, outfile);
			   else {
			      sprintf (ErrorBuffer, "Incompatible types to %s", 
					    OpName);
			      WriteError (ErrorBuffer);
			   }
			   outfile << ";\n";
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
       outfile << DestPrefix << dest->CName << " = ";

       if (RoundResult)
	  outfile << "_RoundResult (";

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
			   outfile << "(double) ";
			if (CurEntry->kind == SE_Identifier)
			   outfile << DestPrefix << dest->CName;
			else
			   PrintConstant (CurEntry, outfile);
			break;
	  default:
			sprintf (ErrorBuffer, "Bad type of operand %s in \
				     %s", CurEntry->ident, OpName);
			WriteError (ErrorBuffer);
			break;
       }

       outfile << " " << OpChar << " ";

       if (RoundResult)
	  outfile << "(double) ";
       if (SrcEntry->kind == SE_Identifier)
	  outfile << SrcPrefix << src->CName;
       else
	  PrintConstant (SrcEntry, outfile);

       delete CurEntry;

       if (RoundResult)
	  outfile << ")";
       outfile << ";\n";
    }

    // Close check for zero
    if (OpChar == '/')
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

