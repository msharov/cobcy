/* semdecl.cc
**
**	Implements declaratory semantic actions for COBOL compiler.
*/

#include "semextern.h"

/*---------------------| Globals |------------------------------*/
  Stack<CobolSymbol> 		NestedRecordList;
  Queue<StackEntry> 		VarInit;
  CobolSymbol * 		ParentRecord = NULL;
/*--------------------------------------------------------------*/

void DeclareRecordLevel (void)
{
enum {RecFirst, RecValue = RecFirst, RecPicture, RecName, RecLevel, RecLast};
typedef StackEntry *	StackEntryPtr;
StackEntryPtr entry[4];
CobolSymbol * NewSymbol;
int i;

    // Pop the entries off the stack. There are 4 of them.
    for (i = RecFirst; i < RecLast; ++ i)
       entry[i] = SemStack.Pop();

    // Allocate new symbol table entry
    NewSymbol = new CobolSymbol;
    NewSymbol->SetName (entry [RecName]->ident);
    // Set declaration level which is important for determining nesting
    NewSymbol->DeclLevel = entry [RecLevel]->ival;

    // Close all finished parent records
    CloseScopeLevels (NewSymbol->DeclLevel);

    // Establish whether the line is a record or a variable:
    //	criteria is the presence of a picture field.
    if (entry [RecPicture]->kind == SE_Null) {
       // Since the record line may require previous record to be closed
       NewSymbol->Kind = CobolSymbol::Record;

       // Generate the actual field declaration
       //	Note that this is done before incrementing nesting level
       GenIndent();
       outfile << "struct {\n";

       // Record always increases nesting level
       ++ NestingLevel;
    }
    else {
       // Variable setup is trivial
       NewSymbol->Kind = CobolSymbol::Variable;
       NewSymbol->SetPicture (entry [RecPicture]->ident);

       // Generate the actual field declaration
       GenIndent();
       if (NewSymbol->Picture.Kind == PictureType::String) {
	  outfile << "char " << NewSymbol->CName;
	  outfile << " [" << NewSymbol->Picture.Size + 1 << "];\n";
       }
       else if (NewSymbol->Picture.Kind == PictureType::Float) {
	  if (NewSymbol->Picture.Sign)
	     outfile << "unsigned ";
	  outfile << "double " << NewSymbol->CName;
	  outfile << ";\n";
       }
       else {
	  if (NewSymbol->Picture.Sign)
	     outfile << "unsigned ";
	  outfile << "long int " << NewSymbol->CName;
	  outfile << ";\n";
       }
    }

    if (ParentRecord != NULL) {
       // If the parent record is still not NULL, it is a parent of this line
       //	Since this is a record, start a new parent scope
       if (NewSymbol->Kind == CobolSymbol::Record) {
	  NestedRecordList.Push (ParentRecord);
	  ParentRecord = NewSymbol;
       }

       // Assign parent name to the symbol if a variable. Records are adopted
       //	in CloseScopeLevels when their size is known
       else {
          strcpy (NewSymbol->ParentCName, ParentRecord->CobolName);
          ParentRecord->AddChild (NewSymbol);
       }
    }
    else {	// No parent
       strcpy (NewSymbol->ParentCName, "");
       // Skip a line if level 0 record
       outfile << "\n";

       // No parent = NewSymbol is the new parent
       if (NewSymbol->Kind == CobolSymbol::Record) 
	  ParentRecord = NewSymbol;
    }

    // Actually place the symbol in the table
    SymTable.Insert (NewSymbol->CobolName, NewSymbol);

    // If a default value was given, append the name and the value
    //	to the variable initialization queue, which will be processed
    //	in InitializeVariables()
    if (entry [RecValue]->kind != SE_Null) {
       VarInit.Append (entry [RecName]);
       VarInit.Append (entry [RecValue]);
    }

    // Deallocate all records
    for (i = RecFirst; i < RecLast; ++ i) {
       if (entry[i] != NULL) {
	  // Except for the name and value that were appended to the queue
	  if ((i == RecName || i == RecValue) && 
	      entry [RecValue]->kind == SE_Null)
	     delete (entry[i]);
       }
    }
}

void CloseScopeLevels (int LastLevel)
{
CobolSymbol * ChildRecord;

    // Check if a record before us was finished by checking
    //	the declaration number. If it is less, the record is done.
    while (!NestedRecordList.IsEmpty() && ParentRecord != NULL &&
	   ParentRecord->DeclLevel >= LastLevel) 
    {
       -- NestingLevel;
       GenIndent();
       outfile << "} " << ParentRecord->CName << ";\n";
       ChildRecord = ParentRecord;
       ParentRecord = NestedRecordList.Pop();
       if (ParentRecord != NULL) {
          strcpy (ChildRecord->ParentCName, ParentRecord->CobolName);
          ParentRecord->AddChild (ChildRecord);
       }
    }
    if (ParentRecord != NULL && ParentRecord->DeclLevel >= LastLevel)
    {
       -- NestingLevel;
       GenIndent();
       outfile << "} " << ParentRecord->CName << ";\n";
       ParentRecord = NULL;
    }
}

void InitializeVariables (void)
{
StackEntry * VarName, * VarValue;
CobolSymbol * attr, * ValueAttr;
char prefix[80], ValuePrefix[80];

    outfile << "void _SetVarValues (void)\n";
    outfile << "{\n";
    ++ NestingLevel;
    // Initialize predefined variables
    GenIndent();
    outfile << "memset (_space_var, ' ', 200);\n";
    GenIndent();
    outfile << "_space_var[200] = 0;\n";

    // Initialize user-defined variables
    while (!VarInit.IsEmpty()) {
       VarName = VarInit.Serve();
       VarValue = VarInit.Serve();
       attr = SymTable.Lookup (VarName->ident);
       BuildPrefix (VarName->ident, prefix);
       if (VarValue->kind == SE_Identifier) {
	  ValueAttr = SymTable.Lookup (VarValue->ident);
	  BuildPrefix (VarValue->ident, ValuePrefix);
       }

       GenIndent();
       if (attr->Picture.Kind == PictureType::String) {
	  outfile << "_AssignVarString (" << prefix << attr->CName << ", ";
	  if (VarValue->kind == SE_Identifier) {
	     outfile << ValuePrefix << ValueAttr->CName;
	     outfile << ", " << attr->Picture.Size;
	     outfile << ", " << ValueAttr->Picture.Size;
	     outfile << ");\n";
	  }
	  else {
	     PrintConstant (VarValue, outfile);
	     outfile << ", " << attr->Picture.Size;
	     outfile << ", 0);\n";
	  }
       }
       else {
	  outfile << prefix << attr->CName << " = ";
	  if (VarValue->kind == SE_Identifier)
	     outfile << ValuePrefix << ValueAttr->CName;
	  else
	     PrintConstant (VarValue, outfile);
	  outfile << ";\n";
       }
       delete VarName;
       delete VarValue;
    }
    -- NestingLevel;
    GenIndent();
    outfile << "}\n\n";
}

