/* symfile.cc
**
**	Cobol file symbol
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

CobolFile :: CobolFile (void)
{
    memset (FileName, 0, PATH_MAX);
    memset (RecordName, 0, MAX_SYMBOL_LENGTH);
    memset (StatusVar, 0, MAX_SYMBOL_LENGTH);
    memset (RecordKey, 0, MAX_SYMBOL_LENGTH);
    memset (FlushCommand, 0, PATH_MAX);
    AccessMode = AM_Sequential;
    Organization = ORG_Sequential;
    NewlineFlag = TRUE;
    Changed = FALSE;
}

CobolSymbolType CobolFile :: Kind (void)
{
    return (CS_FileDesc);
}

void CobolFile :: SetFilename (char * filename)
{
    strcpy (FileName, filename);
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
    if (Organization == ORG_Sequential)
       AccessMode = AM_Sequential;
    else
       AccessMode = AM_Random;
}

void CobolFile :: SetKey (char * keyname)
{
    strcpy (RecordKey, keyname);
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

void CobolFile :: AssociateRecord (void)
{
CobolRecord * rec;

    if (strlen (RecordName) && (rec = (CobolRecord*) 
     				LookupIdentifier (RecordName)) != NULL)
       rec->AssociateWithStream (this);
    else
       WriteError ("file is not associated with any record");
}

void CobolFile :: GenDeclare (ofstream& os)
{
    GenIndent();
    os << "FILE * " << GetCName();
    if (Organization == ORG_Indexed) {
       os << ", _" << GetCName() << "_i";
    }
    os << ";\n";
}

void CobolFile :: GenOpen (ofstream& os, OpenModeType mode)
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
       os << " (&" << *this << ", ";
       if (Organization == ORG_Indexed) {
	  os << "&_" << *this << ", ";
       }
       os << "\"" << FileName << "\", ";
       switch (mode) {
	  case OM_Input:	os << "\"rb\"";		break;
	  case OM_Output:	os << "\"wb\"";		break;
	  case OM_Extend:	os << "\"ab\"";		break;
	  case OM_InputOutput:	os << "\"w+b\"";	break;
       }
       os << ");\n";
       Open = TRUE;
       if (mode != OM_Input)
	  Changed = TRUE;
    }
}

void CobolFile :: GenFlush (ofstream& os)
{
    GenIndent();
    os << "fflush (" << *this << ");\n";
    if (strlen (FlushCommand) > 0)  {
       GenIndent();
       os << "system (\"" << FlushCommand << " ";
       os << FileName << "\");\n";
    }
}

void CobolFile :: GenSeek (ofstream& os)
{
CobolData * BoundRecord;

    // Outright stop any attempt to seek in sequential mode
    if (AccessMode == AM_Sequential)
       return;

    if (Open) {
       GenIndent();

       if (Organization == ORG_Relative || Organization == ORG_Indexed) {
	  switch (Organization) {
	     case ORG_Relative:	os << "_SeekRelativeFile";	break;
	     case ORG_Indexed:	os << "_SeekIndexedFile";	break;
	     default:	WriteError ("cannot seek in sequential files");
			   break;
	  }
       }

       // Now look up the bound record to determine block size for the file
       if ((BoundRecord = (CobolData*) SymTable.Lookup (RecordName)) == NULL){
	  WriteError ("record bound to file does not exist");
	  return;
       }

       os << " (" << *this << ", ";
       if (Organization == ORG_Indexed)
	  os << "_" << *this << ", ";

       if (Organization == ORG_Relative) {
	  // Here, use offset RecordSize * RecordKey
	  os << BoundRecord->GetSize() << " * ";
       }
       PrintIdentifier (RecordKey, os);
       os << ");\n";
    }
}

void CobolFile :: GenClose (ofstream& os)
{
    if (Open) {
       GenIndent();
       switch (Organization) {
	  case ORG_Line_sequential:
          case ORG_Sequential: 	os << "_CloseSequentialFile ("; break;
          case ORG_Relative: 	os << "_CloseRelativeFile ("; break;
          case ORG_Indexed: 	os << "_CloseIndexedFile ("; break;
       }
       os << *this << ");\n";
       
       if (Changed)
	  GenFlush (os);
       if (UnlinkOnClose) {
	  GenIndent();
	  os << "unlink (\"" << FileName << "\");\n";
       }
    }
}

void CobolFile :: GenEOFCheck (ofstream& os)
{
    GenIndent();
    os << "if (feof (" << *this << "))\n";
}

void CobolFile :: GenWriteData (ofstream& os, CobolData * data)
{
    if (data != NULL)
       data->GenWrite (os, GetFullCName());
    else {
       data = (CobolData*) LookupIdentifier (RecordName);
       data->GenWrite (os, GetFullCName());
    }
    Changed = TRUE;
}

void CobolFile :: GenReadData (ofstream& os, CobolData * data)
{
    if (data != NULL)
       data->GenRead (os, GetFullCName());
    else {
       data = (CobolData*) LookupIdentifier (RecordName);
       data->GenRead (os, GetFullCName());
    }
}

void CobolFile :: GenReadEnd (ofstream& os)
{
    if (NewlineFlag) {
       GenIndent();
       os << "fgetc (" << *this << ");\n";
    }
}

void CobolFile :: GenWriteEnd (ofstream& os)
{
    if (NewlineFlag) {
       GenIndent();
       os << "fprintf (" << *this << ", \"\\n\");\n";
    }
}

CobolFile :: ~CobolFile (void)
{
    // Nothing is needed here yet
}

