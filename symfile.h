/* symfile.h
**
**	Defines a file descriptor symbol
*/

#ifndef __SYMFILE_H
#define __SYMFILE_H

#include <mdefs.h>
#include <llist.h>
#include "symbase.h"
#include "symdata.h"

typedef enum {
    AM_Sequential,
    AM_Random,
    AM_Dynamic
} AccessModeType;

typedef enum {
    ORG_Sequential,
    ORG_Line_sequential,
    ORG_Relative,
    ORG_Indexed
} OrganizationType;

typedef enum {
    OM_Input,
    OM_Output,
    OM_Extend,
    OM_InputOutput
} OpenModeType;

class CobolFile : public CobolSymbol {
protected:
    char			FileName [PATH_MAX];
    char			RecordName [MAX_SYMBOL_LENGTH];
    char			StatusVar [MAX_SYMBOL_LENGTH];
    char			RecordKey [MAX_SYMBOL_LENGTH];

    char			FlushCommand [PATH_MAX];
    BOOL			NewlineFlag;
    AccessModeType		AccessMode;
    OrganizationType		Organization;

    BOOL			Changed;
    BOOL			Open;
    OpenModeType		OpenMode;
    BOOL			UnlinkOnClose;

public:
				CobolFile (void);
    CobolSymbolType		Kind (void);
    void			SetFilename (char * filename);
    void			SetAccessMode (AccessModeType mode);
    void			SetOrganization (OrganizationType org);
    void			SetKey (char * keyname);
    void			SetStatusVar (char * varname);
    void			SetRecord (char * recname);
    void			SetFlushCommand (char * NewCommand);
    void			SetNewlineFlag (BOOL NewFlag);
    void			SetUnlinkOnClose (BOOL NewFlag);
    void			AssociateRecord (void);

    void			GenDeclare (ofstream& os);
    void			GenOpen (ofstream& os, OpenModeType mode);
    void			GenFlush (ofstream& os);
    void			GenSeek (ofstream& os);
    void			GenClose (ofstream& os);
    void			GenEOFCheck (ofstream& os);
    void			GenWriteData (ofstream& os, 
    					      CobolData * data = NULL);
    void			GenReadData (ofstream& os, 
    					     CobolData * data = NULL);
    void			GenWriteEnd (ofstream& os);
    void			GenReadEnd (ofstream& os);

    			       ~CobolFile (void);
};

#endif

