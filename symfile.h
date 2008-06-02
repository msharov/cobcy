/* symfile.h
**
**	Defines a file descriptor symbol
*/

#ifndef __SYMFILE_H
#define __SYMFILE_H

#include "symbase.h"
#include "symdata.h"
#include "semtypes.h"

class CobolFile : public CobolSymbol {
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

private:
    void			GenRecordSignature (ostream& os);
    void			GenKeySignature (ostream& os);
    void			WriteIndexCName (ostream& os);
    void			WriteOpenMode (ostream& os, OpenModeType mode);

public:
				CobolFile (void);
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
    virtual void		WriteTextStream (ostream& os);

    void			GenDeclare (ostream& os);
    void			GenOpen (ostream& os, OpenModeType mode);
    void			GenFlush (ostream& os);
    void			GenSeek (ostream& os);
    void			GenClose (ostream& os);
    void			GenEOFCheck (ostream& os);
    void			GenWriteData (ostream& os, 
    					      CobolData * data = NULL);
    void			GenReadData (ostream& os, 
    					     CobolData * data = NULL);
    void			GenWriteEnd (ostream& os);
    void			GenReadEnd (ostream& os);
    void			GenSetupForAppend (ostream& os);

    			       ~CobolFile (void);
};

#endif

