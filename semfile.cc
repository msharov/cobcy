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
  CobolFile * 			FileToSelect = NULL;
  Queue<CobolFile>		FDInit;
  Queue<CobolFile>		SFQueue;
/*--------------------------------------------------------------*/

void BeginFileDecl (void)
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

    // This used here only
    FileToSelect = NewFD;
}

void EndFileDecl (void)
{
    FileToSelect = NULL;
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
#ifndef NDEBUG
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
#ifndef NDEBUG
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

void GenOpen (OpenModeType mode)
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
	fattr->GenOpen (codef, mode);
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

    DestStream->GenSeek (codef);
    DestStream->GenReadData (codef);
    DestStream->GenReadEnd (codef);

    DestStream->GenEOFCheck (codef);
}

//
// This function does setup for GenWrite and GenRewrite
//
static void FileWriteSetup (CobolFile ** DestStream, CobolData ** ToWriteFrom)
{
StackEntry * FileEntry, * SrcEntry;
CobolSymbol * ToWrite;

    SrcEntry = SemStack.Pop();
    FileEntry = SemStack.Pop();

    if ((ToWrite = LookupIdentifier (FileEntry->ident)) == NULL) {
	delete FileEntry;
	delete SrcEntry;
	return;
    }
    delete FileEntry;

    *ToWriteFrom = (CobolData*) SymTable.Lookup (SrcEntry->ident);
    delete SrcEntry;

#ifndef NDEBUG
    cout << "\tWriting record ";
    if (*ToWriteFrom != NULL)
	cout << **ToWriteFrom;
    else
	cout << *ToWrite;
    cout << " to file found using " << *ToWrite << "\n";
#endif

    // Now, the first argument to write can either be a file descriptor,
    //	or the record associated with that file descriptor
    //	The criteria is the Kind function, overloaded from CobolSymbol
    if (ToWrite->Kind() == CS_Record || ToWrite->Kind() == CS_Variable)
	*DestStream = ((CobolData*) ToWrite)->GetStream();
    else if (ToWrite->Kind() == CS_FileDesc)
	*DestStream = (CobolFile*) ToWrite;
    else
	WriteError ("cannot write that");
}

void GenWrite (void)
{
CobolData * ToWriteFrom;
CobolFile * DestStream;
    
    FileWriteSetup (&DestStream, &ToWriteFrom);
    DestStream->GenSetupForAppend (codef);
    DestStream->GenWriteData (codef, (CobolData*) ToWriteFrom);
    DestStream->GenWriteEnd (codef);
}

void GenRewrite (void)
{
CobolData * ToWriteFrom;
CobolFile * DestStream;
    
    FileWriteSetup (&DestStream, &ToWriteFrom);
    DestStream->GenSeek (codef);
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
    if (FileToSelect != NULL) {
	CurEntry = SemStack.Pop();
	FileToSelect->SetStatusVar (CurEntry->ident);
	delete CurEntry;
    }
    else
	WriteError ("setting status variable for nonexistant file");
}

void SetAccessMode (AccessModeType mode)
{
    if (FileToSelect != NULL)
	FileToSelect->SetAccessMode (mode);
    else
	WriteError ("setting access mode for nonexistant file");
}

void SetOrganization (OrganizationType org)
{
    if (FileToSelect != NULL)
	FileToSelect->SetOrganization (org);
    else
	WriteError ("setting organization for nonexistant file");
}

void SetRelativeKey (void)
{
StackEntry * CurEntry;
    if (FileToSelect != NULL) {
	CurEntry = SemStack.Pop();
	FileToSelect->SetKey (CurEntry->ident);
	FileToSelect = NULL;
	SemStack.Push (CurEntry);
	delete CurEntry;
    }
    else
	WriteError ("setting relative key for nonexistant file");
}

void SetRecordKey (void)
{
StackEntry * CurEntry;
    if (FileToSelect != NULL) {
	CurEntry = SemStack.Pop();
	FileToSelect->SetKey (CurEntry->ident);
	delete CurEntry;
    }
    else
	WriteError ("setting record key for nonexistant file");
}

// Printer spool for example...
void OpenSpecialFiles (void)
{
CobolFile * sf;
Queue<CobolFile> TempQ;

#ifndef NDEBUG
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

#ifndef NDEBUG
    cout << "DBG: Closing special files\n";
#endif

    while (!SFQueue.IsEmpty()) {
	sf = SFQueue.Serve();
	sf->GenClose (codef);
    }
}

