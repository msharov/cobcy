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
#include "symrec.h"
#include "symvar.h"
#include "symconst.h"

/*---------------------| Globals |------------------------------*/
  Stack<CobolRecord> 		NestedRecordList;
  Queue<StackEntry> 		VarInit;
  CobolRecord * 		ParentRecord = NULL;
  extern Queue<CobolFile>	SFQueue;
/*--------------------------------------------------------------*/

void DeclareRecordLevel (void)
{
enum {
    RecFirst, 
    RecValue = RecFirst, 
    RecPicture, 
    RecIndexedBy,
    RecTimes,
    RecName, 
    RecLevel, 
    RecLast
};
typedef StackEntry * StackEntryPtr;
StackEntryPtr entry [RecLast];
BOOL HasPicture = TRUE;
CobolRecord * NewRec;
CobolVar * NewVar;
struct {
    char	Name [STACK_IDENT_LENGTH];
    char	Picture [STACK_IDENT_LENGTH];
    char	ArrayIndex [STACK_IDENT_LENGTH]; // Undefined for non-arrays
    int		ArraySize;			 // 1 for non-arrays
    int		DeclLevel;
} NewSymbol;

    // Pop the entries off the stack. There are RecLast of them.
    for (int i = RecFirst; i < RecLast; ++ i)
	entry[i] = SemStack.Pop();

    // Save other variables so that the stack entries can be disposed of ...
    NewSymbol.DeclLevel = entry [RecLevel]->ival;
    strcpy (NewSymbol.Name, entry [RecName]->ident);
    if (entry[RecPicture]->kind != SE_Null)
	strcpy (NewSymbol.Picture, entry [RecPicture]->ident);
    else {
	// Conditional boolean
	if (NewSymbol.DeclLevel == 88) {
	    strcpy (NewSymbol.Picture, "9");
	    entry[RecValue]->kind = SE_Null;
	}
	else
	    HasPicture = FALSE;
    }
    NewSymbol.ArraySize = entry[RecTimes]->ival;
    if (entry[RecIndexedBy]->kind != SE_Null)
       strcpy (NewSymbol.ArrayIndex, entry[RecIndexedBy]->ident);
    else
       NewSymbol.ArrayIndex[0] = 0;

    // If a default value was given, append the name and the value
    //	to the variable initialization queue, which will be processed
    //	in InitializeVariables()
    if (entry [RecValue]->kind != SE_Null) {
       VarInit.Append (entry [RecName]);
       VarInit.Append (entry [RecValue]);
    }

    // ... and dispose of them here. Name and value were appended to 
    //	variable init queue so only delete these
    delete entry[RecPicture];
    delete entry[RecLevel];
    delete entry[RecTimes];
    delete entry[RecIndexedBy];

    // Establish whether the line is a record or a variable:
    //	criteria is the presence of a picture field.
    // DeclLevel 88 is a conditional boolean, a variable.
    if (HasPicture || NewSymbol.DeclLevel == 88)
    {
       NewVar = new CobolVar;
       NewVar->SetName (NewSymbol.Name);
       NewVar->SetDeclLevel (NewSymbol.DeclLevel);
       NewVar->SetPicture (NewSymbol.Picture);
       NewVar->SetArraySize (NewSymbol.ArraySize);
       NewVar->SetArrayIndex (NewSymbol.ArrayIndex);

       // Close all finished parent records
       CloseScopeLevels (NewSymbol.DeclLevel);

       // Generate the actual field declaration
       NewVar->GenDeclare (codef);

       // If parent is not nobody, tell it about it
       if (ParentRecord != NULL) {
	  // Adopt this variable.
	  NewVar->SetParent (ParentRecord->GetName());
          ParentRecord->AddChild (NewVar);
       }
       else {
	  NewVar->SetParent (NULL);
	  codef << "\n";	// Skip a line if level 0 variable
       }

       // Actually place the symbol in the table
#ifndef NDEBUG
       cout << "Declaring variable " << NewSymbol.Name;
       cout << ", dl = " << NewSymbol.DeclLevel << "\n";
#endif
       SymTable.Insert (NewSymbol.Name, NewVar);
    }
    else {
       NewRec = new CobolRecord;
       NewRec->SetName (NewSymbol.Name);
       NewRec->SetDeclLevel (NewSymbol.DeclLevel);
       NewRec->SetArraySize (NewSymbol.ArraySize);
       NewRec->SetArrayIndex (NewSymbol.ArrayIndex);

       // Close all finished parent records
       CloseScopeLevels (NewSymbol.DeclLevel);

       // Try setting the parent now.
       if (ParentRecord != NULL) {
	  // If the parent record is still not NULL, it is a parent of this 
	  // line. To keep the child happy, tell it who the parent will be.
	  NewRec->SetParent (ParentRecord->GetName());
	  // Since this is a record, start a new parent scope
	  NestedRecordList.Push (ParentRecord);
       }
       else {	// No parent = I am parent
	  ParentRecord = NewRec;
	  NewRec->SetParent (NULL);
	  // Skip a line if level 0 record
	  codef << "\n";
       }
       // A record will always open a new scope
       ParentRecord = NewRec;

       // Actually place the symbol in the table
#ifndef NDEBUG
       cout << "Declaring record " << NewSymbol.Name;
       cout << ", dl = " << NewSymbol.DeclLevel << "\n";
#endif
       SymTable.Insert (NewSymbol.Name, NewRec);

       // Generate the actual field declaration
       NewRec->GenDeclareBegin (codef);
       // Record always increases nesting level
       ++ NestingLevel;
    }
}

void CloseScopeLevels (WORD LastLevel)
{
CobolData * ChildRecord;

    // Check if a record before us was finished by checking
    //	the declaration number. If it is less, the record is done.
    while (!NestedRecordList.IsEmpty() && ParentRecord != NULL &&
	   ParentRecord->GetDeclLevel() >= LastLevel) 
    {
#ifndef NDEBUG
       cout << "Closing record " << *ParentRecord;
       cout << ", dl = " << ParentRecord->GetDeclLevel() << "\n";
#endif
       -- NestingLevel;
       ParentRecord->GenDeclareEnd (codef);
       ChildRecord = ParentRecord;
       ParentRecord = NestedRecordList.Pop();
       if (ParentRecord != NULL) {
          ChildRecord->SetParent (ParentRecord->GetName());
          ParentRecord->AddChild (ChildRecord);
       }
    }
    // This happens if the queue is empty, but still not leveled
    //	i.e. the new record is on highest level (0). It will have parent NULL
    if (ParentRecord != NULL && ParentRecord->GetDeclLevel() >= LastLevel)
    {
#ifndef NDEBUG
       cout << "Closing top-level record " << *ParentRecord << "\n";
#endif
       -- NestingLevel;
       ParentRecord->GenDeclareEnd (codef);
       ParentRecord = NULL;
    }
}

static int OpenAncestorLoops (const char* name)
{
    int arrayCountThisLevel = 0;
    while (strlen (name) > 0) {
	CobolData* attr = (CobolData*) SymTable.Lookup (name);
	if (attr->GetArraySize() > 1) {
	    GenIndent();
	    codef << "for (" << attr->GetArrayIndex();
	    codef << " = 0; " << attr->GetArrayIndex();
	    codef << " < " << attr->GetArraySize();
	    codef << "; ++ " << attr->GetArrayIndex() << ")\n";
	    ++ NestingLevel;
	    ++ arrayCountThisLevel;
	}
	name = attr->GetParentName();
    }
    return (arrayCountThisLevel);
}

void InitializeVariables (void)
{
StackEntry * VarName, * VarValue;
CobolVar * attr;
CobolVar * ValueAttr;
CobolConstant ValueConst;

    codef << "static void _SetVarValues (void)\n";
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
	int nLevels = OpenAncestorLoops (VarName->ident);
	attr = (CobolVar*) SymTable.Lookup (VarName->ident);
	if (VarValue->kind == SE_Identifier) {
	    ValueAttr = (CobolVar*) SymTable.Lookup (VarValue->ident);
	    attr->GenMove (codef, ValueAttr);
	    NestingLevel -= nLevels;
	}
	else {
	    ValueConst = VarValue;
	    attr->GenMove (codef, ValueConst);
	}
	NestingLevel -= nLevels;
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
CobolFile * DevStream;

    SpName = SemStack.Pop();
    DevName = SemStack.Pop();

    DevStream = new CobolFile;
    DevStream->SetName (SpName->ident);

    if (strcmp (DevName->ident, "printer") == 0) {
       DevStream->SetFilename (PRINTER_SPOOL_FILE);
       DevStream->SetFlushCommand (PRINTER_COMMAND);
       DevStream->SetOrganization (ORG_Sequential);
       DevStream->SetAccessMode (AM_Sequential);
       DevStream->SetUnlinkOnClose (TRUE);
    }
    else
       DevStream->SetFilename (DevName->ident);
    
    // Declaration is here because it will not be mentioned again
    DevStream->GenDeclare (codef);

#ifndef NDEBUG
    cout << "DBG: Declaring special name " << SpName->ident << "\n";
#endif
    SymTable.Insert (SpName->ident, DevStream);
    SFQueue.Append (DevStream);

    delete DevName;
    delete SpName;
}

