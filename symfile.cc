/* symfile.cc
**
**	Cobol file symbol.
**	Sequential and line sequential files are handled raw,
** Relative and indexed files are in dBASE IV format. Note that
** reading and writing is still done with same routines since DBF
** files store data in all ascii and just like COBOL wants it. Were
** they developed for COBOL by any chance?
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include "symfile.h"
#include "symrec.h"
#include "symvar.h"

CobolFile :: CobolFile (void)
{
    memset (DataFileName, 0, PATH_MAX);
    memset (IndexFileName, 0, PATH_MAX);
    memset (RecordName, 0, MAX_SYMBOL_LENGTH);
    memset (StatusVar, 0, MAX_SYMBOL_LENGTH);
    memset (RecordKey, 0, MAX_SYMBOL_LENGTH);
    memset (FlushCommand, 0, PATH_MAX);
    AccessMode = AM_Sequential;
    Organization = ORG_Sequential;
    NewlineFlag = TRUE;
    Changed = FALSE;
    IsDBF = FALSE;
}

CobolSymbolType CobolFile :: Kind (void)
{
    return (CS_FileDesc);
}

void CobolFile :: SetFilename (char * filename)
{
    strcpy (DataFileName, filename);
}

void CobolFile :: SetAccessMode (AccessModeType mode)
{
    // Only sequential access files have newline breaks
    NewlineFlag = FALSE;

    // This is not yet complete, since I am not sure what AM_Dynamic should be
    switch (Organization) {
        case ORG_Sequential:
		NewlineFlag = TRUE;
	case ORG_Line_sequential:
       		if (mode != AM_Sequential)
	  	   WriteWarning ("access mode reset to sequential");
		AccessMode = AM_Sequential;
		break;
	case ORG_Relative:
		AccessMode = mode;	// Can be all three
		break;
	case ORG_Indexed:
		if (mode == AM_Sequential)
		   WriteWarning ("access mode reset to random");
		AccessMode = AM_Random;	
		break;
    }
}

void CobolFile :: SetOrganization (OrganizationType org)
{
    Organization = org;
    // No warning needed because organization should always be set
    //	before access mode. This is just to be sure it has reasonable values.
    if (Organization == ORG_Sequential || Organization == ORG_Line_sequential)
       AccessMode = AM_Sequential;
    else
       AccessMode = AM_Random;

    // Set the DBF flag
    if (Organization == ORG_Relative || Organization == ORG_Indexed)
       IsDBF = TRUE;
}

void CobolFile :: SetKey (char * keyname)
{
    strcpy (RecordKey, keyname);
    strcpy (IndexFileName, RecordKey);
    strcat (IndexFileName, ".ndx");
}

void CobolFile :: SetStatusVar (char * varname)
{
    strcpy (StatusVar, varname);
}

void CobolFile :: SetRecord (char * recname)
{
    strcpy (RecordName, recname);
}

void CobolFile :: SetNewlineFlag (BOOL NewFlag)
{
    NewlineFlag = NewFlag;
}

void CobolFile :: SetUnlinkOnClose (BOOL NewFlag)
{
    UnlinkOnClose = NewFlag;
}

void CobolFile :: SetFlushCommand (char * NewCommand)
{
    strcpy (FlushCommand, NewCommand);
}

void CobolFile :: WriteIndexCName (ostream& os)
{
    // The uppercased 'I' will ensure that no conflict with Cobol code
    //	occurs, since Cobcy translates everything to lowercase.
    os << GetCName() << "_Idx";
}

void CobolFile :: WriteTextStream (ostream& os)
{
    CobolSymbol::WriteTextStream (os);
    if (IsDBF)
       os << "->DataDesc";
}

void CobolFile :: WriteOpenMode (ostream& os, OpenModeType mode)
{
    switch (mode) {
       case OM_Input:		os << "\"rb\"";		break;
       case OM_Output:		os << "\"wb\"";		break;
       case OM_Extend:		os << "\"ab\"";		break;
       case OM_InputOutput:	os << "\"w+b\"";	break;
    }
}

void CobolFile :: GenRecordSignature (ostream& os)
{
CobolRecord * rec;

    if (strlen (RecordName) && (rec = (CobolRecord*) 
     				LookupIdentifier (RecordName)) != NULL) {
       os << "\"";
       rec->GenSignature (os);
       os << "\"";
    }
    else
       WriteError ("file is not associated with any record");
}

void CobolFile :: GenKeySignature (ostream& os)
{
CobolVar * var;

    if (strlen (RecordKey) && (var = (CobolVar*) 
     				LookupIdentifier (RecordKey)) != NULL) {
       os << "\"";
       var->GenSignature (os);
       os << "\"";
    }
    else
       WriteError ("file is not associated with any record");
}

void CobolFile :: AssociateRecord (void)
{
CobolRecord * rec;

    if (strlen (RecordName) && (rec = (CobolRecord*) 
     				LookupIdentifier (RecordName)) != NULL)
       rec->AssociateWithStream (this);
    else
       WriteError ("file is not associated with any record");
}

void CobolFile :: GenDeclare (ostream& os)
{
    GenIndent();
    if (IsDBF)
       os << "DBF_FILE * ";
    else
       os << "FILE * ";
    os << GetCName() << ";\n";

    if (Organization == ORG_Indexed) {
       GenIndent();
       os << "NDX_FILE * ";
       WriteIndexCName (os);
       os << ";\n";
    }
}

void CobolFile :: GenOpen (ostream& os, OpenModeType mode)
{
    GenIndent();
    if (!Open) {
       switch (Organization) {
	  case ORG_Line_sequential:
	  case ORG_Sequential:	os << "_OpenSequentialFile";	break;
	  case ORG_Relative:	os << "_OpenRelativeFile";	break;
	  case ORG_Indexed:	os << "_OpenIndexedFile";	break;
       }
       OpenMode = mode;
       os << " (&" << GetFullCName() << ", ";
       os << "\"" << DataFileName << "\"";

       // For DBF files add the index file descriptor and a
       //	record signature for creation of new files and
       //	validation of the old ones.
       if (IsDBF) {
	  os << ", ";
	  GenRecordSignature (os);
       }

       os << ", ";
       WriteOpenMode (os, mode);
       os << ");\n";

       // Index file is separate in dBASE
       if (Organization == ORG_Indexed) {
	  GenIndent();
	  os << "_OpenIndexFile (&";
	  WriteIndexCName (os);
	  os << ", \"" << IndexFileName << "\", ";
	  GenKeySignature (os);
	  os << ", ";
	  WriteOpenMode (os, mode);
	  os << ");\n";
       }

       Open = TRUE;
       if (mode != OM_Input)
	  Changed = TRUE;
    }
}

void CobolFile :: GenFlush (ostream& os)
{
    GenIndent();
    if (IsDBF)
	os << "fflush (" << GetFullCName() << "->DataDesc);\n";
    else
	os << "fflush (" << GetFullCName() << ");\n";

    if (strlen (FlushCommand) > 0)  {
	GenIndent();
	os << "system (\"" << FlushCommand << " ";
	os << DataFileName << "\");\n";
    }
}

void CobolFile :: GenSeek (ostream& os)
{
CobolData * BoundRecord;

    // Outright stop any attempt to seek in sequential mode
    if (AccessMode == AM_Sequential)
	return;

    if (Open) {
	GenIndent();
	switch (Organization) {
	    case ORG_Relative:	
		os << "_SeekRelativeFile";	
		break;
	    case ORG_Indexed:	
		os << "_SeekIndexedFile";	
		break;
	    default:	
		WriteError ("cannot seek in sequential files");
		break;
	}

	// Now look up the bound record to determine block size for the file
	if ((BoundRecord = (CobolData*) SymTable.Lookup (RecordName)) == NULL){
	    WriteError ("record bound to file does not exist");
	    return;
	}

	os << " (" << GetFullCName() << ", ";
	if (Organization == ORG_Indexed) {
	    WriteIndexCName (os);
	    os << ", ";
	}

	CobolVar* keyAttr = (CobolVar*) LookupIdentifier (RecordKey);
	if (keyAttr != NULL)
	    keyAttr->GenCharCast (os);
	else
	    WriteError ("this DBF file has no index variable");
	os << ");\n";
    }
}

void CobolFile :: GenClose (ostream& os)
{
    if (Open) {
	if (Changed)
	    GenFlush (os);

	GenIndent();
	switch (Organization) {
	    case ORG_Line_sequential:
	    case ORG_Sequential: 	os << "_CloseSequentialFile ("; break;
	    case ORG_Relative: 	os << "_CloseRelativeFile ("; break;
	    case ORG_Indexed: 	os << "_CloseIndexedFile ("; break;
	}
	if (IsDBF)	// DBF functions take a double pointer
	    os << "&";
	os << GetFullCName();
	os << ");\n";
	
	// Index file is separate in dBASE
	if (Organization == ORG_Indexed) {
	    GenIndent();
	    os << "_CloseIndexFile (&";
	    WriteIndexCName (os);
	    os << ");\n";
	}

	if (UnlinkOnClose) {
	    GenIndent();
	    os << "unlink (\"" << DataFileName << "\");\n";
	}
    }
}

void CobolFile :: GenEOFCheck (ostream& os)
{
    GenIndent();
    os << "if (feof (" << *this << "))\n";
}

void CobolFile :: GenWriteData (ostream& os, CobolData * data)
{
char StreamName[80];

    strcpy (StreamName, GetFullCName());
    if (IsDBF)
       strcat (StreamName, "->DataDesc");

    if (data != NULL)
       data->GenWrite (os, StreamName);
    else {
       data = (CobolData*) LookupIdentifier (RecordName);
       data->GenWrite (os, StreamName);
    }
    Changed = TRUE;
}

void CobolFile :: GenReadData (ostream& os, CobolData * data)
{
char StreamName[80];

    strcpy (StreamName, GetFullCName());
    if (IsDBF)
	strcat (StreamName, "->DataDesc");

    if (data != NULL)
	data->GenRead (os, StreamName);
    else {
	data = (CobolData*) LookupIdentifier (RecordName);
	data->GenRead (os, StreamName);
    }
}

void CobolFile :: GenReadEnd (ostream& os)
{
    if (NewlineFlag) {
	GenIndent();
	os << "fgetc (" << *this << ");\n";
    }
    if (IsDBF && AccessMode == AM_Sequential) {
	GenIndent();
	os << "DBF_SeekToNext (" << GetFullCName() << ");\n";
    }
}

void CobolFile :: GenWriteEnd (ostream& os)
{
    if (NewlineFlag) {
	GenIndent();
	os << "fprintf (" << *this << ", \"\\n\");\n";
    }
    if (IsDBF && AccessMode == AM_Sequential) {
	GenIndent();
	os << "DBF_SeekToNext (" << GetFullCName() << ");\n";
    }
}

void CobolFile :: GenSetupForAppend (ostream& os)
{
    if (IsDBF) {
	GenIndent();
	os << "DBF_AppendRecord (" << GetFullCName() << ");\n";
    }
    if (Organization == ORG_Indexed) {
	GenIndent();
	os << "NDX_InsertKey (";
	WriteIndexCName (os);
	os << ", ";
	CobolVar* keyAttr = (CobolVar*) LookupIdentifier (RecordKey);
	if (keyAttr != NULL)
	    keyAttr->GenCharCast (os);
	else
	    WriteError ("this DBF file has no index variable");
	os << ", " << GetFullCName() << "->Header.nRecords - 1);\n";
    }
}

CobolFile :: ~CobolFile (void)
{
    // Nothing is needed here yet
}

