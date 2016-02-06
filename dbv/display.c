// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include <string.h>
#include <stdlib.h>
#include "display.h"

//----------------------------------------------------------------------

#ifndef min
#define min(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b)	((a) < (b) ? (b) : (a))
#endif

//----------------------------------------------------------------------

static void SetAttrNormal (WINDOW* win);
static void SetAttrHighlight (WINDOW* win);
static void SetAttrHeader (WINDOW* win);
static void DisplayDBFHeader (WINDOW* win, uint32_t row, uint32_t field, DBF_FILE* df);
static void DisplayDBFRecord (WINDOW* win, uint32_t row, uint32_t field, DBF_FILE* df, uint32_t record);

//----------------------------------------------------------------------

void ViewDBF (DBF_FILE * df)
{
    uint32_t Row = 0, TopRow = 0;
    int32_t Field = 0;
    uint32_t maxx UNUSED, maxy, MaxVisRecords = 0;
    uint32_t key = 0, i;
    static const char c_UsageString[81] = " Use arrows, pgup/pgdn, home/end to move; 'q' to quit";

    getmaxyx (stdscr, maxy, maxx);
    SetAttrNormal (stdscr);

    do {
	switch (key) {
	    case KEY_UP:
		if (Row > 0)
		    -- Row;
		break;
	    case KEY_DOWN:
		if (Row < df->Header.nRecords - 1)
		    ++ Row;
		break;
	    case KEY_LEFT:
		if (Field > 0)
		    -- Field;
		break;
	    case KEY_RIGHT:
		if (Field < df->nFields - 1)
		    ++ Field;
		break;
	    case KEY_PPAGE:
		if (Row > MaxVisRecords)
		    Row -= MaxVisRecords;
		else
		    Row = 0;
		if (TopRow > MaxVisRecords)
		    TopRow -= MaxVisRecords;
		else
		    TopRow = 0;
		break;
	    case KEY_NPAGE:
		if (Row < df->Header.nRecords - MaxVisRecords)
		    Row += MaxVisRecords;
		else
		    Row = df->Header.nRecords - 1;
		if (TopRow < df->Header.nRecords - MaxVisRecords)
		    TopRow += MaxVisRecords;
		else
		    TopRow = df->Header.nRecords - MaxVisRecords;
		break;
	    case KEY_HOME:
		Row = 0;
		TopRow = 0;
		break;
	    case KEY_END:
		Row = df->Header.nRecords - 1;
		TopRow = Row - min (Row, MaxVisRecords);
		break;
	}

	while (Row >= TopRow + maxy - 2)
	    ++ TopRow;
	while (Row < TopRow)
	    -- TopRow;
	MaxVisRecords = min (maxy - 2, df->Header.nRecords - TopRow);

	erase();
	SetAttrHeader (stdscr);
	DisplayDBFHeader (stdscr, 0, Field, df);
	for (i = 0; i < MaxVisRecords; ++i){
	    if (i + TopRow == Row)
		SetAttrHighlight (stdscr);
	    else
		SetAttrNormal (stdscr);
	    DisplayDBFRecord (stdscr, i + 1, Field, df, i + TopRow);
	}
	SetAttrNormal (stdscr);
	wmove (stdscr, TopRow+MaxVisRecords+1, 0);
	wclrtobot (stdscr);
	SetAttrHeader (stdscr);
	mvwaddstr (stdscr, maxy-1, 0, c_UsageString);
	wclrtoeol (stdscr);
	refresh();
    } while ((key = getch()) != 'q');
}

void DisplayOpen (void)
{
    initscr();
    noecho();
    cbreak();
    curs_set (0);
    leaveok (stdscr, true);
    keypad (stdscr, true);
    if (has_colors()) {
	start_color();
	init_pair (1, COLOR_WHITE, COLOR_BLUE);
	init_pair (2, COLOR_YELLOW, COLOR_MAGENTA);
	init_pair (3, COLOR_BLUE, COLOR_WHITE);
    }
}

void DisplayClose (void)
{
    echo();
    nocbreak();
    curs_set (1);
    keypad (stdscr, false);
    leaveok (stdscr, false);
    attrset (A_NORMAL);
    clear();
    refresh();
    endwin();
}

static void SetAttrNormal (WINDOW * win)
{
    int attr = A_NORMAL;
    if (has_colors())
	attr |= COLOR_PAIR(1);
    wattrset (win, attr);
    wbkgdset (win, ' '|attr);
}

static void SetAttrHighlight (WINDOW * win)
{
    int attr = A_BOLD;
    if (has_colors())
	attr |= COLOR_PAIR(2);
    wattrset (win, attr);
    wbkgdset (win, ' '|attr);
}

static void SetAttrHeader (WINDOW * win)
{
    int attr = has_colors() ? COLOR_PAIR(3) : A_BOLD;
    wattrset (win, attr);
    wbkgdset (win, ' '|attr);
}

static void DisplayDBFHeader (WINDOW* win, uint32_t row, uint32_t field, DBF_FILE * df)
{
    wmove (win, row, 0);
    wclrtoeol (win);
    unsigned x = 0, maxy UNUSED, maxx;
    getmaxyx (win, maxy, maxx);
    for (int i = field; i < df->nFields && x+4 < maxx; ++i) {
	mvwaddch (win, row, x++, ' ');
	unsigned namelen = strlen (df->Fields[i].Name);
	unsigned fieldlen = df->Fields[i].FieldLength;
	unsigned outsz = min (max (namelen, fieldlen), maxx-x-2);
	waddnstr (win, df->Fields[i].Name, outsz);
	x += outsz;
	mvwaddstr (win, row, x, " |");
	x += 2;
    }
}

static void DisplayDBFRecord (WINDOW* win, uint32_t row, uint32_t field, DBF_FILE* df, uint32_t record)
{
    DBF_SeekToRecord (df, record);
    char* recordBuffer = (char*) malloc (df->Header.RecordLength);
    DBF_ReadRecord (df, recordBuffer);
    const char* ftext = recordBuffer;

    wmove (win, row, 0);
    wclrtoeol (win);
    unsigned x = 0, maxy UNUSED, maxx;
    getmaxyx (win, maxy, maxx);
    for (unsigned i = 0; i < field; ++i)
	ftext += df->Fields[i].FieldLength;
    for (int i = field; i < df->nFields && x+4 < maxx; ++i) {
	mvwaddch (win, row, x++, ' ');
	unsigned fieldlen = df->Fields[i].FieldLength;
	unsigned namelen = strlen (df->Fields[i].Name);
	unsigned outsz = min (max (namelen, fieldlen), maxx-x-2);
	waddnstr (win, ftext, min (outsz, fieldlen));
	ftext += fieldlen;
	x += outsz;
	wclrtoeol (win);
	mvwaddstr (win, row, x, " |");
	x += 2;
    }
    free (recordBuffer);
}
