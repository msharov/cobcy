// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMFILE_H_21AF9B80775D0334054DEA8C2F74C4B1
#define SYMFILE_H_21AF9B80775D0334054DEA8C2F74C4B1

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
    void			GenRecordSignature (ostringstream& os);
    void			GenKeySignature (ostringstream& os);
    void			WriteIndexCName (ostringstream& os);
    void			WriteOpenMode (ostringstream& os, OpenModeType mode);

public:
				CobolFile (void);
				~CobolFile (void);
    CobolSymbolType		Kind (void);
    void			SetFilename (const char* filename);
    void			SetAccessMode (AccessModeType mode);
    void			SetOrganization (OrganizationType org);
    void			SetKey (const char* keyname);
    void			SetStatusVar (const char* varname);
    void			SetRecord (const char* recname);
    void			SetFlushCommand (const char* NewCommand);
    void			SetNewlineFlag (bool NewFlag);
    void			SetUnlinkOnClose (bool NewFlag);
    void			AssociateRecord (void);
    virtual void		text_write (ostringstream& os) const;

    void			GenDeclare (ostringstream& os);
    void			GenOpen (ostringstream& os, OpenModeType mode);
    void			GenFlush (ostringstream& os);
    void			GenSeek (ostringstream& os);
    void			GenClose (ostringstream& os);
    void			GenEOFCheck (ostringstream& os);
    void			GenWriteData (ostringstream& os, CobolData* data = NULL);
    void			GenReadData (ostringstream& os, CobolData* data = NULL);
    void			GenWriteEnd (ostringstream& os);
    void			GenReadEnd (ostringstream& os);
    void			GenSetupForAppend (ostringstream& os);
private:
    char			DataFileName [PATH_MAX];
    char			IndexFileName [PATH_MAX];
    char			RecordName [MAX_SYMBOL_LENGTH];
    char			StatusVar [MAX_SYMBOL_LENGTH];
    char			RecordKey [MAX_SYMBOL_LENGTH];

    char			FlushCommand [PATH_MAX];
    bool			NewlineFlag;
    AccessModeType		AccessMode;
    OrganizationType		Organization;

    bool			Changed;
    bool			Open;
    OpenModeType		OpenMode;
    bool			UnlinkOnClose;

    bool			IsDBF;		// To avoid 'if Organization ==
    bool			IsFormatted;
};

#endif
