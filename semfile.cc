// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "semcontrol.h"
#include "semfile.h"
#include "semdecl.h"
#include "symfile.h"
#include "semdecl.h"	// for CloseScopeLevels
#include "symdata.h"

//----------------------------------------------------------------------

vector<CobolFile*> SFQueue;

static CobolFile* s_FileToAssociate = nullptr;
static CobolFile* s_FileToSelect = nullptr;
static vector<CobolFile*> s_FDInit;

//----------------------------------------------------------------------

void BeginFileDecl (void)
{
    auto filename = PopIdentifier();	// First the file name
    auto fd = PopIdentifier();		// File descriptor name
    auto f = g_Symbols.emplace<CobolFile> (fd.ident);
    f->SetName (fd.ident);
    // Printer is a special file: spool and print on close
    if (strcmp (filename.ident, "printer") == 0) {
	f->SetFilename (PRINTER_SPOOL_FILE);
	f->SetFlushCommand (PRINTER_COMMAND);
	f->SetOrganization (ORG_Sequential);
	f->SetAccessMode (AM_Sequential);
	f->SetUnlinkOnClose (true);
    } else
	f->SetFilename (filename.ident);
    s_FileToSelect = f;
}

void EndFileDecl (void)
{
    s_FileToSelect = nullptr;
}

void GenFileDesc (void)
{
    auto filename = PopIdentifier();
    auto fid = PopIdentifier();
    auto f = g_Symbols.lookup<CobolFile>(fid.ident);
    if (!f)
	return WriteError ("Cannot create unselected file descriptor");
    DTRACE ("Declaring new file descriptor %s\n", fid.ident.c_str());
    if (strlen (filename.ident) > 0)
	f->SetFilename (filename.ident);
    s_FDInit.push_back (f);

    if (s_FileToAssociate)
	WriteError ("No record found for file descriptor");
    s_FileToAssociate = f;

    CloseScopeLevels (0);
    codef << "\n";
    f->GenDeclare (codef);
}

void AssociateFileRecord (void)
{
    if (!s_FileToAssociate || g_Tokens.empty())
	return;
    auto& e = g_Tokens.back();		// The record is not fully defined at this point, so do not pop
    DTRACE ("\tAssociating record %s to %s\n", e.ident.c_str(), s_FileToAssociate->GetFullCName().c_str());
    s_FileToAssociate->SetRecord (e.ident);
    s_FileToAssociate = nullptr;	// Only associate the first record. All the others are not associated.
}

void GenOpen (OpenModeType mode)
{
    auto files = PopStatement();
    for (auto& f : files) {
	auto fattr = g_Symbols.lookup<CobolFile>(f.ident);
	if (!fattr)
	    WriteError ("unknown file %s", f.ident.c_str());
	else
	    fattr->GenOpen (codef, mode);
    }
}

void GenClose (void)
{
    auto files = PopStatement();
    for (auto& f : files) {
	auto fattr = g_Symbols.lookup<CobolFile>(f.ident);
	if (!fattr)
	    WriteError ("unknown file %s", f.ident.c_str());
	else
	    fattr->GenClose (codef);
    }
}

void GenRead (void)
{
    auto file = PopIdentifier();
    auto fd = LookupIdentifier<CobolSymbol> (file.ident);
    if (!fd)
	return WriteError ("unknown file to read");

    auto fstm = dynamic_cast<const CobolFile*>(fd);
    if (fd->Kind() == CS_Record || fd->Kind() == CS_Variable)
	fstm = dynamic_cast<const CobolData*>(fd)->GetStream();
    else if (fd->Kind() != CS_FileDesc)
	return WriteError ("cannot read into that");

    fstm->GenSeek (codef);
    fstm->GenReadData (codef);
    fstm->GenReadEnd (codef);
    fstm->GenEOFCheck (codef);
}

// This function does setup for GenWrite and GenRewrite
static const CobolFile* FileWriteSetup (const CobolData*& toWriteFrom)
{
    auto src = PopIdentifier();
    auto file = PopIdentifier();
    auto toWrite = LookupIdentifier<CobolSymbol> (file.ident);
    toWriteFrom = g_Symbols.lookup<CobolData> (src.ident);
    DTRACE ("\tWriting record %s to file found using %s\n", toWriteFrom ? toWriteFrom->GetFullCName().c_str() : toWrite->GetFullCName().c_str(), toWrite->GetFullCName().c_str());
    auto fstm = dynamic_cast<const CobolFile*>(toWrite);
    // Now, the first argument to write can either be a file descriptor,
    //	or the record associated with that file descriptor
    //	The criteria is the Kind function, overloaded from CobolSymbol
    if (!toWrite)
	WriteError ("unknown destination file");
    else {
	if (toWrite->Kind() == CS_Record || toWrite->Kind() == CS_Variable)
	    fstm = dynamic_cast<const CobolData*>(toWrite)->GetStream();
	else if (toWrite->Kind() != CS_FileDesc)
	    WriteError ("cannot write that");
    }
    return fstm;
}

void GenWrite (void)
{
    const CobolData* var;
    auto f = FileWriteSetup (var);
    if (!f)
	return;
    f->GenSetupForAppend (codef);
    f->GenWriteData (codef, var);
    f->GenWriteEnd (codef);
}

void GenRewrite (void)
{
    const CobolData* var;
    auto f = FileWriteSetup (var);
    if (!f)
	return;
    f->GenSeek (codef);
    f->GenWriteData (codef, var);
    f->GenWriteEnd (codef);
}

void AssociateRecordsWithFD (void)
{
    for (auto& ifd : s_FDInit)
	ifd->AssociateRecord();
    s_FDInit.clear();
}

void SetFileStatus (void)
{
    if (!s_FileToSelect)
	return WriteError ("setting status variable for nonexistant file");
    auto e = PopIdentifier();
    s_FileToSelect->SetStatusVar (e.ident);
}

void SetAccessMode (AccessModeType mode)
{
    if (!s_FileToSelect)
	return WriteError ("setting access mode for nonexistant file");
    s_FileToSelect->SetAccessMode (mode);
}

void SetOrganization (OrganizationType org)
{
    if (!s_FileToSelect)
	return WriteError ("setting organization for nonexistant file");
    s_FileToSelect->SetOrganization (org);
}

void SetRelativeKey (void)
{
    if (!s_FileToSelect)
	return WriteError ("setting relative key for nonexistant file");
    s_FileToSelect->SetKey (g_Tokens.back().ident);
    s_FileToSelect = nullptr;
}

void SetRecordKey (void)
{
    if (!s_FileToSelect)
	return WriteError ("setting record key for nonexistant file");
    auto e = PopIdentifier();
    s_FileToSelect->SetKey (e.ident);
}

// Printer spool for example...
void OpenSpecialFiles (void)
{
    DTRACE ("DBG: Opening special files\n");
    for (auto& sf : SFQueue)
	sf->GenOpen (codef, OM_Output);
}

void CloseSpecialFiles (void)
{
    DTRACE ("DBG: Closing special files\n");
    for (auto& sf : SFQueue)
	sf->GenClose (codef);
    SFQueue.clear();
}
