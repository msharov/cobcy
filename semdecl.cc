/* semdecl.cc
**
**	Implements declaratory semantic actions for COBOL compiler.
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#include "semdecl.h"
#include "semfile.h"

/*---------------------| Globals |------------------------------*/
  Stack<CobolSymbol> 		NestedRecordList;
  Queue<StackEntry> 		VarInit;
  CobolSymbol * 		ParentRecord = NULL;
  extern BOOL			PrinterUsed;
/*--------------------------------------------------------------*/

void DeclareRecordLevel (void)
{
enum {RecFirst, RecValue = RecFirst, RecPicture, RecName, RecLevel, RecLast};
typedef StackEntry * StackEntryPtr;
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
       codef << "struct {\n";

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
	  codef << "char " << NewSymbol->CName;
	  codef << " [" << NewSymbol->Picture.Size + 1 << "];\n";
       }
       else if (NewSymbol->Picture.Kind == PictureType::Float) {
	  if (NewSymbol->Picture.Sign)
	     codef << "unsigned ";
	  codef << "double " << NewSymbol->CName;
	  codef << ";\n";
       }
       else {
	  if (NewSymbol->Picture.Sign)
	     codef << "unsigned ";
	  codef << "long int " << NewSymbol->CName;
	  codef << ";\n";
       }
    }

    if (ParentRecord != NULL) {
       // If the parent record is still not NULL, it is a parent of this line
       // To keep the child happy, tell it who the parent will be
       NewSymbol->SetParent (ParentRecord->CobolName);

       // Since this is a record, start a new parent scope
       if (NewSymbol->Kind == CobolSymbol::Record) {
	  NestedRecordList.Push (ParentRecord);
	  ParentRecord = NewSymbol;
       }
       // Assign child to parent if a variable. Records are adopted
       //	in CloseScopeLevels when their size is known
       else
          ParentRecord->AddChild (NewSymbol);

    }
    else {	// No parent
       NewSymbol->SetParent (NULL);
       // Skip a line if level 0 record
       codef << "\n";

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

void CloseScopeLevels (unsigned int LastLevel)
{
CobolSymbol * ChildRecord;

    // Check if a record before us was finished by checking
    //	the declaration number. If it is less, the record is done.
    while (!NestedRecordList.IsEmpty() && ParentRecord != NULL &&
	   ParentRecord->DeclLevel >= LastLevel) 
    {
       -- NestingLevel;
       GenIndent();
       codef << "} " << ParentRecord->CName << ";\n";
       ChildRecord = ParentRecord;
       ParentRecord = NestedRecordList.Pop();
       if (ParentRecord != NULL) {
          ChildRecord->SetParent (ParentRecord->CobolName);
          ParentRecord->AddChild (ChildRecord);
       }
    }
    if (ParentRecord != NULL && ParentRecord->DeclLevel >= LastLevel)
    {
       -- NestingLevel;
       GenIndent();
       codef << "} " << ParentRecord->CName << ";\n";
       ParentRecord = NULL;
    }
}

void InitializeVariables (void)
{
StackEntry * VarName, * VarValue;
CobolSymbol * attr, * ValueAttr;

    codef << "void _SetVarValues (void)\n";
    codef << "{\n";
    ++ NestingLevel;
    // Initialize predefined variables
    GenIndent();
    codef << "memset (_space_var, ' ', 200);\n";
    GenIndent();
    codef << "_space_var[200] = 0;\n";

    // Initialize user-defined variables
    while (!VarInit.IsEmpty()) {
       VarName = VarInit.Serve();
       VarValue = VarInit.Serve();
       attr = SymTable.Lookup (VarName->ident);
       if (VarValue->kind == SE_Identifier) {
	  ValueAttr = SymTable.Lookup (VarValue->ident);
       }

       GenIndent();
       if (attr->Picture.Kind == PictureType::String) {
	  codef << "_AssignVarString (" << attr->Prefix << attr->CName << ", ";
	  if (VarValue->kind == SE_Identifier) {
	     codef << ValueAttr->Prefix << ValueAttr->CName;
	     codef << ", " << attr->Picture.Size;
	     codef << ", " << ValueAttr->Picture.Size;
	     codef << ");\n";
	  }
	  else {
	     PrintConstant (VarValue, codef);
	     codef << ", " << attr->Picture.Size;
	     codef << ", 0);\n";
	  }
       }
       else {
	  codef << attr->Prefix << attr->CName << " = ";
	  if (VarValue->kind == SE_Identifier)
	     codef << ValueAttr->Prefix << ValueAttr->CName;
	  else
	     PrintConstant (VarValue, codef);
	  codef << ";\n";
       }
       delete VarName;
       delete VarValue;
    }

    -- NestingLevel;
    GenIndent();
    codef << "}\n\n";
}

// PRINTER IS PRINTER-DISPLAY
void DeclareSpecialName (void)
{
StackEntry * SpName, * DevName;
CobolSymbol * DevStream;

    SpName = SemStack.Pop();
    DevName = SemStack.Pop();

    DevStream = new CobolSymbol;
    DevStream->SetName (SpName->ident);
    
    /*
    ** Special names declare devices, as I understand.
    ** Since I don't have any cardreaders or magnetic tape (:)
    ** I just pipe the output to the display.
    ** In the future, I'll try to make device handling easier...
    */
    if (strcmp (DevName->ident, "printer") == 0) {
       strcpy (DevStream->CName, PRINTER_STREAM_NAME);
       PrinterUsed = TRUE;
    }
    else
       strcpy (DevStream->CName, DISPLAY_STREAM_NAME);

    SymTable.Insert (SpName->ident, DevStream);

    delete DevName;
    delete SpName;
}

