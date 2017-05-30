// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symfile.h"
#include "symrec.h"
#include "symvar.h"

CobolFile::CobolFile (void)
:_dataFileName()
,_indexFileName()
,_recordName()
,_statusVar()
,_recordKey()
,_flushCommand()
, NewlineFlag (true)
, AccessMode (AM_Sequential)
, Organization (ORG_Sequential)
, Changed (false)
, Open()
, OpenMode()
, UnlinkOnClose()
, IsDBF (false)
, IsFormatted()
{
}

void CobolFile::SetAccessMode (AccessModeType mode)
{
    // Only sequential access files have newline breaks
    NewlineFlag = false;
    // This is not yet complete, since I am not sure what AM_Dynamic should be
    switch (Organization) {
	case ORG_Sequential:
	    NewlineFlag = true;	// fallthrough
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

void CobolFile::SetOrganization (OrganizationType org)
{
    Organization = org;
    // No warning needed because organization should always be set
    //	before access mode. This is just to be sure it has reasonable values.
    if (Organization == ORG_Sequential)
	AccessMode = AM_Sequential;
    else
	AccessMode = AM_Random;

    // Set the DBF flag
    if (Organization == ORG_Relative || Organization == ORG_Indexed)
	IsDBF = true;
}

void CobolFile::SetKey (const char* keyname)
{
    _recordKey = keyname;
    _indexFileName = _recordKey;
    _indexFileName += ".ndx";
}

void CobolFile::WriteIndexCName (ostringstream& os) const
{
    // The uppercased 'I' will ensure that no conflict with Cobol code
    //	occurs, since Cobcy translates everything to lowercase.
    os << GetCName() << "_Idx";
}

void CobolFile::text_write (ostringstream& os) const
{
    CobolSymbol::text_write (os);
    if (IsDBF)
	os << "->DataDesc";
}

void CobolFile::WriteOpenMode (ostringstream& os, OpenModeType mode) const
{
    switch (mode) {
	case OM_Input:		os << "\"rb\"";		break;
	case OM_Output:		os << "\"wb\"";		break;
	case OM_Extend:		os << "\"ab\"";		break;
	case OM_InputOutput:	os << "\"w+b\"";	break;
    }
}

void CobolFile::GenRecordSignature (ostringstream& os) const
{
    const CobolRecord* rec;
    if (!_recordName.empty() && (rec = LookupIdentifier<CobolRecord> (_recordName))) {
	os << "\"";
	rec->GenSignature (os);
	os << "\"";
    } else
	WriteError ("file %s is not associated with any record signature", GetFullCName().c_str());
}

void CobolFile::GenKeySignature (ostringstream& os) const
{
    const CobolVar* var;
    if (!_recordKey.empty() && (var = LookupIdentifier<CobolVar> (_recordKey))) {
	os << "\"";
	var->GenSignature (os);
	os << "\"";
    } else
	WriteError ("file %s is not associated with any key signature", GetFullCName().c_str());
}

void CobolFile::AssociateRecord (void) const
{
    CobolRecord* rec = g_Symbols.lookup<CobolRecord> (_recordName);
    if (!rec)
	WriteError ("file %s is not currently associated with any record", GetFullCName().c_str());
    else
	rec->AssociateWithStream (this);
}

void CobolFile::GenDeclare (ostringstream& os) const
{
    GenIndent();
    assert (!NestingLevel);
    if (IsDBF)
	os << "static DBF_FILE* ";
    else
	os << "static FILE* ";
    os << GetCName() << " UNUSED;\n";

    if (Organization == ORG_Indexed) {
	GenIndent();
	os << "static NDX_FILE* ";
	WriteIndexCName (os);
	os << " UNUSED;\n";
    }
}

void CobolFile::GenOpen (ostringstream& os, OpenModeType mode)
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
	os << "\"" << _dataFileName << "\"";

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
	    os << ", \"" << _indexFileName << "\", ";
	    GenKeySignature (os);
	    os << ", ";
	    WriteOpenMode (os, mode);
	    os << ");\n";
	}

	Open = true;
	if (mode != OM_Input)
	    Changed = true;
    }
}

void CobolFile::GenFlush (ostringstream& os) const
{
    GenIndent();
    if (IsDBF)
	os << "fflush (" << GetFullCName() << "->DataDesc);\n";
    else
	os << "fflush (" << GetFullCName() << ");\n";

    if (!_flushCommand.empty())  {
	GenIndent();
	os << "system (\"" << _flushCommand << " ";
	os << _dataFileName << "\");\n";
    }
}

void CobolFile::GenSeek (ostringstream& os) const
{
    // Outright stop any attempt to seek in sequential mode
    if (AccessMode == AM_Sequential)
	return;

    if (Open) {
	GenIndent();
	if (IsDBF) {
	    switch (Organization) {
		case ORG_Relative:	os << "_SeekRelativeFile"; break;
		case ORG_Indexed:	os << "_SeekIndexedFile"; break;
		default:	WriteError ("cannot seek in sequential files"); break;
	    }
	}
	// Now look up the bound record to determine block size for the file
	if (!LookupIdentifier<CobolData>(_recordName))
	    return WriteError ("record bound to file does not exist");
	os << " (" << GetFullCName() << ", ";
	if (Organization == ORG_Indexed) {
	    WriteIndexCName (os);
	    os << ", ";
	}
	auto rkey = LookupIdentifier<CobolVar>(_recordKey);
	if (!rkey)
	    WriteError ("unknown record key %s", _recordKey.c_str());
	else
	    rkey->GenCharCast (os);
	os << ");\n";
    }
}

void CobolFile::GenClose (ostringstream& os)
{
    if (Open) {
	if (Changed) {
	    GenFlush (os);
	    Changed = false;
	}

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
	    os << "unlink (\"" << _dataFileName << "\");\n";
	}
	Open = false;
    }
}

void CobolFile::GenEOFCheck (ostringstream& os) const
{
    GenIndent();
    os << "if (feof (" << *this << "))\n";
}

void CobolFile::GenWriteData (ostringstream& os, const CobolData* data) const
{
    string streamName = GetFullCName();
    if (IsDBF)
	streamName += "->DataDesc";
    if (!data)
	data = LookupIdentifier<CobolData> (_recordName);
    if (data)
	data->GenWrite (os, streamName);
    Changed = true;
}

void CobolFile::GenReadData (ostringstream& os, const CobolData* data) const
{
    string streamName = GetFullCName();
    if (IsDBF)
	streamName += "->DataDesc";
    if (!data)
	data = LookupIdentifier<CobolData> (_recordName);
    if (data)
	data->GenRead (os, streamName);
}

void CobolFile::GenReadEnd (ostringstream& os) const
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

void CobolFile::GenWriteEnd (ostringstream& os) const
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

void CobolFile::GenSetupForAppend (ostringstream& os) const
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
	auto rkey = LookupIdentifier<CobolVar>(_recordKey);
	if (!rkey)
	    WriteError ("unknown record key %s", _recordKey.c_str());
	else
	    rkey->GenCharCast (os);
	os << ", " << GetFullCName() << "->Header.nRecords - 1);\n";
    }
}
