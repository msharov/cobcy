// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "symbase.h"
#include "symdata.h"
#include "semtypes.h"

/// Defines a file descriptor symbol
///
/// Sequential and line sequential files are handled raw,
/// Relative and indexed files are in dBASE IV format. Note that
/// reading and writing is still done with same routines since DBF
/// files store data in all ascii and just like COBOL wants it. Were
/// they developed for COBOL by any chance?
class CobolFile : public CobolSymbol {
private:
    void			GenRecordSignature (ostringstream& os) const;
    void			GenKeySignature (ostringstream& os) const;
    void			WriteIndexCName (ostringstream& os) const;
    void			WriteOpenMode (ostringstream& os, OpenModeType mode) const;

public:
				CobolFile (void);
    virtual CobolSymbolType	Kind (void) const override		{ return CS_FileDesc; }
    inline void			SetFilename (const char* filename)	{ _dataFileName = filename; }
    void			SetAccessMode (AccessModeType mode);
    void			SetOrganization (OrganizationType org);
    void			SetKey (const char* keyname);
    void			SetStatusVar (const char* varname)	{ _statusVar = varname; }
    void			SetRecord (const char* recname)		{ _recordName = recname; }
    void			SetFlushCommand (const char* NewCommand){ _flushCommand = NewCommand; }
    void			SetNewlineFlag (bool NewFlag)		{ NewlineFlag = NewFlag; }
    void			SetUnlinkOnClose (bool NewFlag)		{ UnlinkOnClose = NewFlag; }

    void			AssociateRecord (void) const;
    virtual void		text_write (ostringstream& os) const override;

    void			GenDeclare (ostringstream& os) const;
    void			GenOpen (ostringstream& os, OpenModeType mode);
    void			GenFlush (ostringstream& os) const;
    void			GenSeek (ostringstream& os) const;
    void			GenClose (ostringstream& os);
    void			GenEOFCheck (ostringstream& os) const;
    void			GenWriteData (ostringstream& os, const CobolData* data = nullptr) const;
    void			GenReadData (ostringstream& os, const CobolData* data = nullptr) const;
    void			GenWriteEnd (ostringstream& os) const;
    void			GenReadEnd (ostringstream& os) const;
    void			GenSetupForAppend (ostringstream& os) const;
private:
    string			_dataFileName;
    string			_indexFileName;
    string			_recordName;
    string			_statusVar;
    string			_recordKey;

    string			_flushCommand;
    bool			NewlineFlag;
    AccessModeType		AccessMode;
    OrganizationType		Organization;

    mutable bool		Changed;
    bool			Open;
    OpenModeType		OpenMode;
    bool			UnlinkOnClose;

    bool			IsDBF;		// To avoid 'if Organization ==
    bool			IsFormatted;
};
