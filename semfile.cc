/* semfile.cc
**
**	Implements file semantic actions for COBOL compiler.
*/

#ifdef __MSDOS__
#include "semexter.h"
#include "semcontr.h"		// EOF checking with if()
#else
#include "semextern.h"
#include "semcontrol.h"
#endif
#include "semfile.h"
#include "semdecl.h"
#if CAN_HAVE_STDIO
#ifndef __MSDOS__
#include <unistd.h>
#endif
#endif
#include "symfile.h"
#include "semdecl.h"	// for CloseScopeLevels
#include "symdata.h"

/*---------------------| Globals |------------------------------*/
  CobolFile * 			FileToAssociate = NULL;
  Queue<CobolFile>		FDInit;
  Queue<CobolFile>		SFQueue;
/*--------------------------------------------------------------*/

void FileDecl (void)
{
StackEntry * CurEntry;
CobolFile * NewFD;

    CurEntry = SemStack.Pop();	// Pop file name
    NewFD = new CobolFile;

    // Printer is a special file: spool and print on close
    if (strcmp (CurEntry->ident, "printer") == 0) {
       NewFD->SetFilename (PRINTER_SPOOL_FILE);
       NewFD->SetFlushCommand (PRINTER_COMMAND);
       NewFD->SetOrganization (ORG_Sequential);
       NewFD->SetAccessMode (AM_Sequential);
       NewFD->SetUnlinkOnClose (TRUE);
    }
    else
       NewFD->SetFilename (CurEntry->ident);

    delete (CurEntry);

    // Pop file descriptor name
    CurEntry = SemStack.Pop();
    NewFD->SetName (CurEntry->ident);
    SymTable.Insert (CurEntry->ident, NewFD);
    delete (CurEntry);
}

void GenFileDesc (void)
{
StackEntry * CurEntry, * FileNameEntry;
CobolFile * NewFD;

    FileNameEntry = SemStack.Pop();
    CurEntry = SemStack.Pop();
    if ((NewFD = (CobolFile*) SymTable.Lookup (CurEntry->ident)) == NULL) {
       WriteError ("Cannot create unselected file descriptor");
       delete (CurEntry);
       return;
    }
#if DEBUG
    cout << "Declaring new file descriptor " << CurEntry->ident << "\n";
#endif
    delete (CurEntry);

    if (strlen (FileNameEntry->ident) > 0)
       NewFD->SetFilename (FileNameEntry->ident);
    delete (FileNameEntry);

    FDInit.Append (NewFD);

    if (FileToAssociate != NULL)
       WriteError ("No record found for file descriptor");
    FileToAssociate = NewFD;

    CloseScopeLevels (0);
    codef << "\n";
    NewFD->GenDeclare (codef);
}

void AssociateFileRecord (void)
{
StackEntry * CurEntry;

    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
#if DEBUG
       cout << "\tAssociating record " << CurEntry->ident;
       cout << " to " << *FileToAssociate << "\n";
#endif
       FileToAssociate->SetRecord (CurEntry->ident);
       // Only associate the first record. All the others are not associated.
       FileToAssociate = NULL;
       // Push it back for further declaration. The record is not
       //	fully defined at this point.
       SemStack.Push (CurEntry);
    }
}

void GenOpen (char * mode)
{
StackEntry *entry;
CobolFile *fattr;
OpenModeType nmode;
int nIds, i;

    if (strcmp (mode, "input") == 0)
       nmode = OM_Input;
    else if (strcmp (mode, "output") == 0)
       nmode = OM_Output;
    else if (strcmp (mode, "io") == 0)
       nmode = OM_InputOutput;
    else if (strcmp (mode, "extend") == 0)
       nmode = OM_Extend;
    else {
       WriteError ("internal: bad file mode");
       return;
    }

    nIds = CountIdentifiers();
    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();
       if ((fattr = (CobolFile*) SymTable.Lookup (entry->ident)) == NULL) {
	  WriteError ("Unknown file descriptor");
	  delete entry;
	  return;
       }
       delete entry;
       
#if DEBUG
       cout << "\tOpening file " << *fattr << " for " << mode << "\n";
#endif
       fattr->GenOpen (codef, nmode);
    }
}

void GenClose (void)
{
StackEntry *entry;
CobolFile *fattr;
int nIds, i;

    nIds = CountIdentifiers();
    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();
       if ((fattr = (CobolFile*) SymTable.Lookup (entry->ident)) == NULL) {
	  WriteError ("Unknown file descriptor");
          delete entry;
	  return;
       }
       delete entry;
#if DEBUG
       cout << "\tClosing file " << *fattr << "\n";
#endif
       fattr->GenClose (codef);
    }
}

void GenSeek (CobolFile * RecFile)
{
    RecFile->GenSeek (codef);
}

void GenRead (void)
{
StackEntry * entry;
CobolSymbol * ToRead;
CobolFile * DestStream;

    entry = SemStack.Pop();

    if ((ToRead = LookupIdentifier (entry->ident)) == NULL) {
       delete entry;
       return;
    }
    delete entry;

    if (ToRead->Kind() == CS_Record || ToRead->Kind() == CS_Variable)
       DestStream = ((CobolData*) ToRead)->GetStream();
    else if (ToRead->Kind() == CS_FileDesc)
       DestStream = (CobolFile*) ToRead;
    else
       WriteError ("cannot read into that");

    DestStream->GenReadData (codef);
    DestStream->GenReadEnd (codef);

    DestStream->GenEOFCheck (codef);
}

void GenWrite (void)
{
StackEntry * FileEntry, * SrcEntry;
CobolSymbol * ToWrite;
CobolData * ToWriteFrom;
CobolFile * DestStream;

    SrcEntry = SemStack.Pop();
    FileEntry = SemStack.Pop();

    if ((ToWrite = LookupIdentifier (FileEntry->ident)) == NULL) {
       delete FileEntry;
       delete SrcEntry;
       return;
    }
    delete FileEntry;

    ToWriteFrom = (CobolData*) SymTable.Lookup (SrcEntry->ident);
    delete SrcEntry;

#if DEBUG
    cout << "\tWriting record ";
    if (ToWriteFrom != NULL)
       cout << *ToWriteFrom;
    else
       cout << *ToWrite;
    cout << " to file found using " << *ToWrite << "\n";
#endif

    // Now, the first argument to write can either be a file descriptor,
    //	or the record associated with that file descriptor
    //	The criteria is the Kind function, overloaded from CobolSymbol
    if (ToWrite->Kind() == CS_Record || ToWrite->Kind() == CS_Variable)
       DestStream = ((CobolData*) ToWrite)->GetStream();
    else if (ToWrite->Kind() == CS_FileDesc)
       DestStream = (CobolFile*) ToWrite;
    else
       WriteError ("cannot write that");

    DestStream->GenWriteData (codef, (CobolData*) ToWriteFrom);
    DestStream->GenWriteEnd (codef);
}

void AssociateRecordsWithFD (void)
{
CobolFile * FD;
    while (!FDInit.IsEmpty()) {
       FD = FDInit.Serve();
       FD->AssociateRecord();
    }
}

void SetFileStatus (void)
{
StackEntry * CurEntry;
    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
       FileToAssociate->SetStatusVar (CurEntry->ident);
       delete CurEntry;
    }
}

void SetAccessMode (char * mode)
{
    if (FileToAssociate != NULL) {
       if (strcmp (mode, "sequential") == 0)
          FileToAssociate->SetAccessMode (AM_Sequential);
       else if (strcmp (mode, "random") == 0)
          FileToAssociate->SetAccessMode (AM_Random);
       else if (strcmp (mode, "dynamic") == 0)
          FileToAssociate->SetAccessMode (AM_Dynamic);
       else
	  WriteError ("Invalid access mode");
    }
}

void SetOrganization (char * org)
{
    if (FileToAssociate != NULL) {
       if (strcmp (org, "sequential") == 0)
          FileToAssociate->SetOrganization (ORG_Sequential);
       else if (strcmp (org, "line sequential") == 0)
          FileToAssociate->SetOrganization (ORG_Line_sequential);
       else if (strcmp (org, "relative") == 0)
          FileToAssociate->SetOrganization (ORG_Relative);
       else if (strcmp (org, "indexed") == 0)
          FileToAssociate->SetOrganization (ORG_Indexed);
       else
	  WriteError ("Invalid organization");
    }
}

void SetRelativeKey (void)
{
StackEntry * CurEntry;
    if (FileToAssociate != NULL) {
       CurEntry = SemStack.Pop();
       FileToAssociate->SetKey (CurEntry->ident);
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
       FileToAssociate->SetKey (CurEntry->ident);
       delete CurEntry;
    }
}

// Printer spool for example...
void OpenSpecialFiles (void)
{
CobolFile * sf;
Queue<CobolFile> TempQ;

#if DEBUG
    cout << "DBG: Opening special files\n";
#endif

    while (!SFQueue.IsEmpty()) {
       sf = SFQueue.Serve();
       sf->GenOpen (codef, OM_Output);
       TempQ.Append (sf);
    }
    while (!TempQ.IsEmpty())
       SFQueue.Append (TempQ.Serve());	// For closing
}

void CloseSpecialFiles (void)
{
CobolFile * sf;

#if DEBUG
    cout << "DBG: Closing special files\n";
#endif

    while (!SFQueue.IsEmpty()) {
       sf = SFQueue.Serve();
       sf->GenClose (codef);
    }
}

