// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef DISPLAY_H_186EF824437607FF5648FA194C2EDA1C
#define DISPLAY_H_186EF824437607FF5648FA194C2EDA1C

#include "../config.h"
#include <ncurses.h>

//----------------------------------------------------------------------

void ViewBin (FILE* df);
void DisplayOpen (void);
void DisplayClose (void);
void SetAttrNormal (WINDOW* win);
void SetAttrHighlight (WINDOW* win);
void SetAttrHeader (WINDOW* win);
void ClearCursesWindow (WINDOW* win);
void DisplayHeader (WINDOW* win, uint32_t row, FILE* df);
void DisplayRecord (WINDOW* win, uint32_t row, char FileBuffer[16], uint32_t offset);

//----------------------------------------------------------------------

#endif

