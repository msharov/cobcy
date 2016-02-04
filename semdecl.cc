// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "semdecl.h"
#include "semfile.h"
#include "symrec.h"
#include "symvar.h"
#include "symconst.h"

//----------------------------------------------------------------------

extern vector<CobolFile*> SFQueue;

static vector<CobolRecord*> s_NestedRecordList;
struct SVarInitEntry {
    StackEntry name, value;
    inline SVarInitEntry (const StackEntry& n, const StackEntry& v) : name(n),value(v) {}
};
static vector<SVarInitEntry> s_VarInit;
static CobolRecord* s_ParentRecord = nullptr;

//----------------------------------------------------------------------

void DeclareRecordLevel (void)
{
    // Pop the entries off the stack. There are 4 of them.
    enum { RecValue, RecPicture, RecName, RecLevel, NRecs };
    StackEntry entry [NRecs];
    for (auto i = 0u; i < NRecs; ++ i)
	entry[i] = PopIdentifier();

    // If a default value was given, append the name and the value
    //	to the variable initialization queue, which will be processed
    //	in InitializeVariables()
    if (entry[RecValue].kind != SE_Null)
	s_VarInit.emplace_back (entry[RecName], entry[RecValue]);

    // Save other variables so that the stack entries can be disposed of ...
    struct {
	string	name;
	string	picture;
	int	level;
    } newSymbol;
    bool NoPicture = false;
    newSymbol.level = entry[RecLevel].ival;
    newSymbol.name = entry[RecName].ident;
    if (entry[RecPicture].kind != SE_Null)
	newSymbol.picture = entry[RecPicture].ident;
    else
	NoPicture = true;

    // Establish whether the line is a record or a variable:
    //	criteria is the presence of a picture field.
    if (NoPicture) {
	DTRACE ("Declaring record %s, dl = %d\n", newSymbol.name.c_str(), newSymbol.level);
	auto newRec = g_Symbols.emplace<CobolRecord> (newSymbol.name);
	newRec->SetName (newSymbol.name);
	newRec->SetDeclLevel (newSymbol.level);

	// Close all finished parent records
	CloseScopeLevels (newSymbol.level);

	// Try setting the parent now.
	if (s_ParentRecord) {
	    // If the parent record is still not nullptr, it is a parent of this
	    // line. To keep the child happy, tell it who the parent will be.
	    newRec->SetParent (s_ParentRecord->GetName());
	    // Since this is a record, start a new parent scope
	    s_NestedRecordList.push_back (s_ParentRecord);
	} else { // No parent = I am parent
	    s_ParentRecord = newRec;
	    newRec->SetParent (nullptr);
	    // Skip a line if level 0 record
	    codef << "\n";
	}
	// A record will always open a new scope
	s_ParentRecord = newRec;

	// Generate the field declaration
	newRec->GenDeclareBegin (codef);
	// Record always increases nesting level
	++ NestingLevel;
    } else {
	DTRACE ("Declaring variable %s, dl = %d\n", newSymbol.name.c_str(), newSymbol.level);
	auto newVar = g_Symbols.emplace<CobolVar> (newSymbol.name);
	newVar->SetName (newSymbol.name);
	newVar->SetDeclLevel (newSymbol.level);
	newVar->SetPicture (newSymbol.picture);

	// Close all finished parent records
	CloseScopeLevels (newSymbol.level);

	// Generate the actual field declaration
	newVar->GenDeclare (codef);

	// If parent is not nobody, tell it about it
	if (s_ParentRecord) { // Adopt this variable.
	    newVar->SetParent (s_ParentRecord->GetName());
	    s_ParentRecord->AddChild (newVar);
	} else {
	    newVar->SetParent (nullptr);
	    codef << "\n";	// Skip a line if level 0 variable
	}
    }
}

void CloseScopeLevels (uint32_t LastLevel)
{
    CobolData * ChildRecord;

    // Check if a record before us was finished by checking
    //	the declaration number. If it is less, the record is done.
    while (!s_NestedRecordList.empty() && s_ParentRecord && s_ParentRecord->GetDeclLevel() >= LastLevel) {
	DTRACE ("Closing record %s, dl = %d\n", s_ParentRecord->GetFullCName().c_str(), s_ParentRecord->GetDeclLevel());
	-- NestingLevel;
	s_ParentRecord->GenDeclareEnd (codef);
	ChildRecord = s_ParentRecord;
	s_ParentRecord = s_NestedRecordList.back();
	s_NestedRecordList.pop_back();
	if (s_ParentRecord) {
	    ChildRecord->SetParent (s_ParentRecord->GetName());
	    s_ParentRecord->AddChild (ChildRecord);
	}
    }
    // This happens if the queue is empty, but still not leveled
    //	i.e. the new record is on highest level (0). It will have parent nullptr
    if (s_ParentRecord && s_ParentRecord->GetDeclLevel() >= LastLevel) {
	DTRACE ("Closing top-level record %s\n", s_ParentRecord->GetFullCName().c_str());
	-- NestingLevel;
	s_ParentRecord->GenDeclareEnd (codef);
	s_ParentRecord = nullptr;
    }
}

void InitializeVariables (void)
{
    codef << "void _SetVarValues (void)\n{\n";
    ++ NestingLevel;
    // Initialize predefined variables
    GenIndent(); codef << "memset (_space_var, ' ', sizeof(_space_var));\n";
    GenIndent(); codef << "_space_var[sizeof(_space_var)-1] = 0;\n";

    // Initialize user-defined variables
    for (auto& i : s_VarInit) {
	auto attr = g_Symbols.lookup<CobolVar> (i.name.ident);
	if (!attr)
	    return WriteError ("initializing unknown variable");
	if (i.value.kind == SE_Identifier) {
	    auto valattr = g_Symbols.lookup<CobolVar> (i.value.ident);
	    if (!valattr)
		return WriteError ("unknown initialization value");
	    attr->GenMove (codef, valattr);
	} else {
	    CobolConstant valconst = i.value;
	    attr->GenMove (codef, valconst);
	}
    }
    s_VarInit.clear();

    -- NestingLevel;
    GenIndent();
    codef << "}\n\n";
}

// PRINTER IS PRINTER-DISPLAY
void DeclareSpecialName (void)
{
    auto name = PopIdentifier();
    auto dev = PopIdentifier();

    DTRACE ("DBG: Declaring special name %s = %s\n", name.ident.c_str(), dev.ident.c_str());
    auto devf = g_Symbols.emplace<CobolFile> (name.ident);
    devf->SetName (name.ident);
    SFQueue.push_back (devf);

    if (dev.ident == "printer") {
	devf->SetFilename (PRINTER_SPOOL_FILE);
	devf->SetFlushCommand (PRINTER_COMMAND);
	devf->SetOrganization (ORG_Sequential);
	devf->SetAccessMode (AM_Sequential);
	devf->SetUnlinkOnClose (true);
    } else
	devf->SetFilename (dev.ident);

    // Declaration is here because it will not be mentioned again
    devf->GenDeclare (codef);
}
