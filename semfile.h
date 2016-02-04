// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "semtypes.h"

#define PRINTER_COMMAND		"lpr -h"
#define PRINTER_SPOOL_FILE	"/tmp/cobol.prn"

#define PRINTER_STREAM_NAME	"printer"
#define DISPLAY_STREAM_NAME	"stdout"

void BeginFileDecl (void);
void EndFileDecl (void);
void GenFileDesc (void);
void AssociateFileRecord (void);
void GenOpen (OpenModeType mode);
void GenClose (void);
void GenRead (void);
void GenWrite (void);
void GenRewrite (void);
void AssociateRecordsWithFD (void);
void SetFileStatus (void);
void SetAccessMode (AccessModeType mode);
void SetOrganization (OrganizationType org);
void SetRelativeKey (void);
void SetRecordKey (void);
void OpenSpecialFiles (void);
void CloseSpecialFiles (void);
