/* semcontrol.cc
**
**	Control statement semantics.
*/

#include "semextern.h"
#include "semcontrol.h"
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif

/*-----------------------| Globals |--------------------------*/
  Queue<char*> 	ParagraphList;
  char		CurParName [32];
/*------------------------------------------------------------*/

void GenParagraph (void)
{
CobolSymbol * NewSymbol;
StackEntry * CurEntry;
char ErrorBuffer [80];
char ** NewParName;

    CurEntry = SemStack.Pop();

    if ((NewSymbol = SymTable.Lookup (CurEntry->ident)) == NULL) {
       // Allocate new symbol table entry
       NewSymbol = new CobolSymbol;
       NewSymbol->SetName (CurEntry->ident);
       // The type will be label
       NewSymbol->Kind = CobolSymbol::Label;
       // Enter it in the table
       SymTable.Insert (CurEntry->ident, NewSymbol);
    }
    else {
       if (NewSymbol->Kind == CobolSymbol::Label && NewSymbol->Undeclared)
	  NewSymbol->Undeclared = FALSE;
       else {
	  sprintf (ErrorBuffer, "Paragraph name %s is already used", 
	  		NewSymbol->CobolName);
	  WriteError (ErrorBuffer);
	  delete CurEntry;
	  return;
       }
    }
    delete (CurEntry);

    // A paragraph finishes previous proc and starts a new one.
    GenIndent();
    outfile << "return;\n";
    GenEndProc();

    // Then declare a new function
    GenIndent();
    outfile << "void " << NewSymbol->CName << " (void)\n";
    BeginCompound();
    outfile << NewSymbol->CName << ":\n";

    NewParName = new (char*);
    *NewParName = new char [30];
    strcpy (*NewParName, NewSymbol->CName);
    strcpy (CurParName, NewSymbol->CName);

    // Setup for insertion in main proc
    ParagraphList.Append (NewParName);
}

void GenGoto (void)
{
CobolSymbol * DestLabel;
StackEntry * CurEntry;

    CurEntry = SemStack.Pop();

    if ((DestLabel = SymTable.Lookup (CurEntry->ident)) == NULL) {
       DestLabel = new CobolSymbol;
       DestLabel->SetName (CurEntry->ident);
       DestLabel->Kind = CobolSymbol::Label;
       DestLabel->Undeclared = TRUE;
       SymTable.Insert (CurEntry->ident, DestLabel);
    }
    delete (CurEntry);

    GenIndent();
    // If in jumping to the top of the current paragraph, use goto.
    if (strcmp (DestLabel->CName, CurParName) == 0)
       outfile << "goto " << DestLabel->CName << ";\n";
    else
       outfile << DestLabel->CName << " ();\n";
}

void GenPerform (void)
{
StackEntry * Proc, * Count;
CobolSymbol * ProcAttr;
char ErrorBuffer [80];
char ProcName [80];

    Count = SemStack.Pop();
    Proc = SemStack.Pop();

    if ((ProcAttr = SymTable.Lookup (Proc->ident)) == NULL) {
       ProcAttr = new CobolSymbol;
       ProcAttr->SetName (Proc->ident);
       ProcAttr->Kind = CobolSymbol::Label;
       ProcAttr->Undeclared = TRUE;
       SymTable.Insert (Proc->ident, ProcAttr);
       strcpy (ProcName, ProcAttr->CName);
    }
    else
       strcpy (ProcName, ProcAttr->CName);

    if (Count->ival > 1) {
       GenIndent();
       outfile << "for (_index = 0; _index < " << Count->ival;
       outfile << "; ++ _index)\n";
       BeginCompound();
    }

    GenIndent();
    outfile << ProcName << " ();\n";

    if (Count->ival > 1)
       EndCompound();

    delete Count;
    delete Proc;
}

// Unlike pragraphs, procedures are always called, with no fall-through
void GenEndProc (void)
{
    GenIndent();
    outfile << "exit (0);\n";
    -- NestingLevel;
    GenIndent();
    outfile << "}\n\n";
}

void BeginCompound (void)
{
    GenIndent();
    outfile << "{\n";
    ++ NestingLevel;
}

void EndCompound (void)
{
    -- NestingLevel;
    GenIndent();
    outfile << "}\n";
}

void GenStartProc (void)
{
StackEntry * CurEntry;
CobolSymbol * NewProc;
char ErrorBuffer [80];

    CurEntry = SemStack.Pop();

    if ((NewProc = SymTable.Lookup (CurEntry->ident)) == NULL) {
       // Allocate new symbol table entry
       NewProc = new CobolSymbol;
       NewProc->SetName (CurEntry->ident);
       // The type will be label
       NewProc->Kind = CobolSymbol::Label;
       // Enter it in the table
       SymTable.Insert (CurEntry->ident, NewProc);
    }
    else {
       if (NewProc->Kind == CobolSymbol::Label && NewProc->Undeclared)
	  NewProc->Undeclared = FALSE;
       else {
	  sprintf (ErrorBuffer, "Procedure name %s is already used", 
	  		NewProc->CobolName);
	  WriteError (ErrorBuffer);
	  delete CurEntry;
	  return;
       }
    }

    GenIndent();
    outfile << "void " << NewProc->CName << " (void)\n";
    BeginCompound();

    delete CurEntry;
}

void GenStartIf (void)
{
    GenIndent();
    outfile << "if (";
}

void GenStartElsif (void)
{
    GenIndent();
    outfile << "else if (";
}

void GenEndIf (void)
{
    outfile << ")\n";
}

void GenElse (void)
{
    GenIndent();
    outfile << "else\n";
}

void GenBool (void)
{
StackEntry *entry[3];
CobolSymbol *attrs[2];
char prefix[80][2];
int i;

    for (i = 0; i < 2; ++ i)
      entry[i] = SemStack.Pop(); 
    
    if ( !(strcmp(entry[0]->ident,"Alphabetic")) ||
         !(strcmp(entry[0]->ident,"Alphabetic-Upper")) ||
         !(strcmp(entry[0]->ident,"Alphabetic-Lower")) )
     {  

       if ((attrs[0] = SymTable.Lookup (entry[1]->ident)) == NULL) {
	  WriteError ("Unknown identifier");
	  return;
       }

       if (attrs[0]->Picture.Kind != PictureType::String) {
          WriteError ("Can't do alphabetic tests on non-alphabetic fields");
          return;
       }

       if (!strcmp(entry[0]->ident,"Alphabetic"))
         outfile << "(_alphabetic(" << entry[1]->ident << "))";
       if (!strcmp(entry[0]->ident,"Alphabetic-Upper"))
         outfile << "(_alphab_up_low(" << entry[1]->ident << ",2))";
       if (!strcmp(entry[0]->ident,"Alphabetic-Lower"))
         outfile << "(_alphab_up_low(" << entry[1]->ident << ",1))";

       delete entry[0]; delete entry[1];
     }
    else
  {
    entry[2] = SemStack.Pop();
      
    if (entry[0]->kind == SE_Identifier) {
       if ((attrs[0] = SymTable.Lookup (entry[0]->ident)) == NULL) {
	  WriteError ("Unknown identifier");
	  return;
       }
       BuildPrefix (entry[0]->ident, prefix[0]);
    }
    if (entry[2]->kind == SE_Identifier) {
       if ((attrs[1] = SymTable.Lookup (entry[2]->ident)) == NULL) {
	  WriteError ("Unknown identifier");
	  return;
       }
       BuildPrefix (entry[2]->ident, prefix[1]);
    }

    outfile << "(";
    if (entry[2]->kind == SE_Identifier)
       outfile << prefix[1] << attrs[1]->CName; 
    else
       PrintConstant (entry[2], outfile); 
    outfile << " " << entry[1]->ident << " ";
    if (entry[0]->kind == SE_Identifier)
       outfile << prefix[0] << attrs[0]->CName;
    else
       PrintConstant (entry[0], outfile);
    outfile << ")";

    for (i = 0; i < 3; ++ i)
       delete entry[i];
  } 
}

void GenConnect (void)
{
StackEntry * CurEntry;

    CurEntry = SemStack.Pop();
    outfile << " " << CurEntry->ident << " ";
    delete (CurEntry);
}

void GenStopRun (void)
{
    GenIndent();
    outfile << "exit (0);\n";
}

void GenParagraphCalls (void)
{
char ** CurName;

    outfile << "\n";
    outfile << "int main ()\n";
    outfile << "{\n";
    ++ NestingLevel;
    GenIndent();
    outfile << "_FirstParagraph ();\n";
    while (!ParagraphList.IsEmpty()) {
       CurName = ParagraphList.Serve();
       GenIndent();
       outfile << *CurName << " ();\n";
       delete [] *CurName;
       delete CurName;
    }
    outfile << "}\n";
}

