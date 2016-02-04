// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "semtypes.h"

void DeclareRecordLevel (void);
void CloseScopeLevels (uint32_t LastLevel);
void InitializeVariables (void);
void DeclareSpecialName (void);
