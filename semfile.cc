/* semfile.cc
**
**	Implements file semantic actions for COBOL compiler.
*/

#include "semextern.h"
#include "semfile.h"
#include "semcontrol.h"		// EOF checking with if()

/*---------------------| Globals |------------------------------*/
  CobolSymbol * 		FileToAssociate = NULL;
  Queue<CobolSymbol>		FDInit;
/*--------------------------------------------------------------*/

void FileDecl (void)
{
StackEntry * CurEntry;
CobolSymbol * NewFD;

    NewFD = new CobolSymbol;
    CurEntry = SemStack.Pop();	// Pop file name
    strcpy (NewFD->FileName, CurEntry->ident);
    // Printer is a special case: spool and print on close
    if (strcmp (NewFD->FileName, "printer") == 0)
       strcpy (NewFD->FileName, PRINTER_SPOOL_FILE);
    delete (CurEntry);

    CurEntry = SemStack.Pop();	// Pop file descriptor name
    NewFD->Kind = CobolSymbol::FileDesc;
    NewFD->SetName (CurEntry->ident);
    delete (CurEntry);

    SymTable.Insert (NewFD->CobolName, NewFD);
}

void GenFileDesc (void)
{
StackEntry * CurEntry, * FileNameEntry;
CobolSymbol * NewFD;

    FileNameEntry = SemStack.Pop();
    CurEntry = SemStack.Pop();
    if ((NewFD = SymTable.Lookup (CurEntry->ident)) == NULL) {
       WriteError ("Cannot create unselected file descriptor");
       delete (CurEntry);
       return;
    }
    delete (CurEntry);

    if (strlen (FileNameEntry->ident) > 0)
       strcpy (NewFD->FileName, FileNameEntry->ident);
    delete (FileNameEntry);

    FDInit.Append (NewFD);

    if (FileToAssociate == NULL)
       FileToAssociate = NewFD;
    else
       WriteError ("No record found for file descriptor");

    CloseScopeLevels (0);
    outfile << "\n";
    GenIndent();
    outfile << "FILE * " << NewFD->CName << ";\n";
}

void AssociateFileRecord (void)
{
StackEntry * CurEntry;
    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
       strcpy (FileToAssociate->FileRecordName, CurEntry->ident);
       FileToAssociate = NULL;
       SemStack.Push (CurEntry);
    }
}

void GenOpen (char * mode)
{
StackEntry *entry;
CobolSymbol *fattr;
int nIds, i;

    nIds = CountIdentifiers();
    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();
       if ((fattr = SymTable.Lookup (entry->ident)) == NULL) {
	  WriteError ("Unknown file descriptor");
	  delete entry;
	  return;
       }
       delete entry;

       GenIndent();
       outfile << fattr->CName << " = fopen (\"" << fattr->FileName;
       outfile << "\", \"" << mode << "b\");\n";
    }
}

void GenClose (void)
{
StackEntry *entry;
CobolSymbol *fattr;
int nIds, i;

    nIds = CountIdentifiers();
    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();
       if ((fattr = SymTable.Lookup (entry->ident)) == NULL) {
	  WriteError ("Unknown file descriptor");
          delete entry;
	  return;
       }
       delete entry;

       GenIndent();
       outfile << "fclose (" << fattr->CName << ");\n";

       // Flush spool output to printer
       if (strcmp (fattr->FileName, PRINTER_SPOOL_FILE)) {
	  GenIndent();
	  outfile << "system (\"" << PRINTER_COMMAND;
	  outfile << " " << PRINTER_SPOOL_FILE << "\");\n";
	  GenIndent();
	  outfile << "sleep (1);\n";	// Give the daemon a chance to catch up
	  unlink (PRINTER_SPOOL_FILE);
       }
    }
}

void GenRead (void)
{
StackEntry * entry;
CobolSymbol * RecFile, * Rec;

    entry = SemStack.Pop();

    if ((RecFile = SymTable.Lookup (entry->ident)) == NULL) {
       WriteError ("Invalid filename to READ");
       delete entry;
       return;
    }
    delete entry;

    if ((Rec = SymTable.Lookup (RecFile->FileRecordName)) == NULL) {
       WriteError ("The given file is not associated with a record");
       return;
    }

    if (Rec->Kind == CobolSymbol::Record)
       ReadRecord (Rec, outfile, RecFile->CName);
    else
       ReadVariable (Rec, outfile, RecFile->CName);

    GenStartIf();
    outfile << "feof (" << RecFile->CName << ")";
    GenEndIf();
}

void GenWrite (void)
{
StackEntry * entry;
CobolSymbol * RecFile, * DataRec = NULL, * Rec;

    // A null entry signifies omission of the optional source record.
    entry = SemStack.Pop();
    if (strcmp (entry->ident, "") != 0) {
       if ((DataRec = SymTable.Lookup (entry->ident)) == NULL) {
	  WriteError ("Invalid source record to WRITE");
	  delete entry;
	  return;
       }
    }
    delete entry;

    entry = SemStack.Pop();
    if ((Rec = SymTable.Lookup (entry->ident)) == NULL) {
       WriteError ("Invalid file record to WRITE");
       delete entry;
       return;
    }
    delete entry;

    if ((RecFile = SymTable.Lookup (Rec->FileName)) == NULL) {
       WriteError ("The given record is not associated with a file");
       return;
    }

    // If optional source record is not specified, write file record.
    if (DataRec == NULL)
       DataRec = Rec;

    if (DataRec->Kind == CobolSymbol::Record)
       PrintRecord (DataRec, outfile, RecFile->CName);
    else
       PrintVariable (DataRec, outfile, RecFile->CName);
}

void AssociateRecordsWithFD (void)
{
CobolSymbol * FDName, * FDRec;

    while (!FDInit.IsEmpty()) {
       FDName = FDInit.Serve();
       if ((FDRec = SymTable.Lookup (FDName->FileRecordName)) == NULL) {
	  WriteError ("Associated record is invalid");
	  continue;
       }
       strcpy (FDRec->FileName, FDName->CobolName);
    }
}

void SetFileStatus (void)
{
StackEntry * CurEntry;
    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
       strcpy (FileToAssociate->FileStatusVar, CurEntry->ident);
       delete CurEntry;
    }
}

void SetAccessMode (char * mode)
{
    if (FileToAssociate != NULL) {
       if (strcmp (mode, "sequential") == 0)
          FileToAssociate->FileAccessMode = CobolSymbol::AM_Sequential;
       else if (strcmp (mode, "random") == 0)
          FileToAssociate->FileAccessMode = CobolSymbol::AM_Random;
       else if (strcmp (mode, "dynamic") == 0)
          FileToAssociate->FileAccessMode = CobolSymbol::AM_Dynamic;
       else
	  WriteError ("Invalid access mode");
    }
}

void SetOrganization (char * org)
{
    if (FileToAssociate != NULL) {
       if (strcmp (org, "sequential") == 0)
          FileToAssociate->FileOrganization = CobolSymbol::ORG_Sequential;
       else if (strcmp (org, "line sequential") == 0)
          FileToAssociate->FileOrganization = CobolSymbol::ORG_Line_sequential;
       else if (strcmp (org, "relative") == 0)
          FileToAssociate->FileOrganization = CobolSymbol::ORG_Relative;
       else if (strcmp (org, "indexed") == 0)
          FileToAssociate->FileOrganization = CobolSymbol::ORG_Indexed;
       else
	  WriteError ("Invalid organization");
    }
}

void SetRelativeKey (void)
{
StackEntry * CurEntry;
    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
       strcpy (FileToAssociate->FileRecordName, CurEntry->ident);
       FileToAssociate = NULL;
       SemStack.Push (CurEntry);
       delete CurEntry;
    }
}

void SetRecordKey (void)
{
StackEntry * CurEntry;
    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
       strcpy (FileToAssociate->FileRecordName, CurEntry->ident);
       delete CurEntry;
    }
}

