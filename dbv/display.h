// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef DISPLAY_H_49D1162E7437501B457F396B695060D0
#define DISPLAY_H_49D1162E7437501B457F396B695060D0

#include "coblib/dbf.h"
#include <ncurses.h>

//----------------------------------------------------------------------

void ViewDBF (DBF_FILE* df);
void DisplayOpen (void);
void DisplayClose (void);

//----------------------------------------------------------------------

#endif

