// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include "display.h"

//----------------------------------------------------------------------

#ifndef min
#define min(a,b)	((a) < (b) ? (a) : (b))
#endif

//----------------------------------------------------------------------

static void SetAttrNormal (WINDOW* win);
static void SetAttrHighlight (WINDOW* win);
static void SetAttrHeader (WINDOW* win);
static void DisplayHeader (WINDOW* win, uint32_t row);
static void DisplayRecord (WINDOW* win, uint32_t row, char FileBuffer[16], uint32_t offset);

//----------------------------------------------------------------------

void ViewBin (FILE * df)
{
uint32_t Row = 0, TopRow = 0;
uint32_t maxx __attribute__((unused)), maxy, MaxVisRecords;
uint32_t key = 0, i;
const char UsageString[81] = " Use arrows, pgup/pgdn, home/end to move; 'q' to quit                            ";
uint32_t FileLength;
uint32_t MaxRows;
typedef char	OneRowType [16];
OneRowType * FileBuffer;

    getmaxyx (stdscr, maxy, maxx);
    SetAttrNormal (stdscr);

    fseek (df, 0, SEEK_END);
    FileLength = ftell (df);
    MaxRows = FileLength / 16 + ((FileLength % 16 == 0) ? 0 : 1);
    FileBuffer = (OneRowType*) malloc (MaxRows * sizeof(OneRowType));
    if (FileBuffer == NULL) {
	printf ("Sorry, out of memory.\n");
	DisplayClose();
	exit (1);
    }
    fseek (df, 0, SEEK_SET);
    fread ((char*) FileBuffer, FileLength, 1, df);
    fclose (df);

    do {
	switch (key) {
	    case KEY_UP:
		if (Row > 0)
		    -- Row;
		break;
	    case KEY_DOWN:
		if (Row < MaxRows - 1)
		    ++ Row;
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
		if (Row < MaxRows - 1 - MaxVisRecords)
		    Row += MaxVisRecords;
		else
		    Row = MaxRows - 1;
		if (TopRow < MaxRows - 1 - MaxVisRecords)
		    TopRow += MaxVisRecords;
		else
		    TopRow = MaxRows - 1 - MaxVisRecords;
		break;
	    case KEY_HOME:
		Row = 0;
		TopRow = 0;
		break;
	    case KEY_END:
		Row = MaxRows - 1;
		TopRow = Row - MaxVisRecords;
		break;
	}

	while (Row >= TopRow + maxy - 2)
	    ++ TopRow;
	while (Row < TopRow)
	    -- TopRow;
	MaxVisRecords = min (maxy - 2, MaxRows - TopRow);

	erase();
	SetAttrHeader (stdscr);
	DisplayHeader (stdscr, 0);
	SetAttrHeader (stdscr);
	mvwaddstr (stdscr, maxy - 1, 0, UsageString);
	for (i = 0; i < MaxVisRecords; ++ i){
	    if (i + TopRow == Row)
		SetAttrHighlight (stdscr);
	    else
		SetAttrNormal (stdscr);
	    DisplayRecord (stdscr, i + 1, FileBuffer[i + TopRow], 
		    (i + TopRow) * 16);
	}
	refresh();

	key = 0;
	key = getch();
    }
    while (key != 'q');

    free (FileBuffer);
}

void DisplayOpen (void)
{
    initscr();
    noecho();
    cbreak();
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
    keypad (stdscr, false);
    leaveok (stdscr, false);
    attrset (A_NORMAL);
    clear();
    refresh();
    endwin();
}

static void SetAttrNormal (WINDOW * win)
{
    if (has_colors())
	wattrset (win, A_NORMAL | COLOR_PAIR(1));
    else
	wattrset (win, A_NORMAL);
}

static void SetAttrHighlight (WINDOW * win)
{
    if (has_colors())
	wattrset (win, A_BOLD | COLOR_PAIR(2));
    else
	wattrset (win, A_BOLD);
}

static void SetAttrHeader (WINDOW * win)
{
    if (has_colors())
	wattrset (win, COLOR_PAIR(3));
    else
	wattrset (win, A_BOLD);
}

static void DisplayHeader (WINDOW * win, uint32_t row)
{
    char OutputBuffer [81];
    memset (OutputBuffer, ' ', 80);
    OutputBuffer [80] = '\x0';
    mvwaddstr (win, row, 0, OutputBuffer);
}

static void IntToHex (uint32_t num, int dp, char* strbuf)
{
    static const char HexDigits [16] = "0123456789ABCDEF";
    for (int i = dp - 1; i >= 0; --i) {
	uint32_t Digit = num & 0x0F;
	strbuf[i] = HexDigits [Digit];
	num >>= 4;
    }
}

static void DisplayRecord (WINDOW* win, uint32_t row, char FileBuffer[16], uint32_t offset)
{
    int maxx, maxy __attribute__((unused));
    getmaxyx (win, maxy, maxx);
    if (maxx < 80) {
	printf ("Screen too small!\n");
	DisplayClose();
	exit (1);
    }

    char NumInAscii[10];
    IntToHex (offset, 8, NumInAscii);
    int opp = 0;
    char OutputBuffer[128];
    OutputBuffer[opp++] = ' ';
    strncpy (&OutputBuffer[opp], NumInAscii, 8);
    opp += 8;
    strncpy (&OutputBuffer[opp], ": ", 8);
    opp += 2;

    for (int i = 0; i < 16; ++i) {
	IntToHex ((uint32_t) FileBuffer[i], 2, NumInAscii);
	NumInAscii[2] = ' ';
	strncpy (&OutputBuffer [opp], NumInAscii, 3);
	opp += 3;
	if (i == 7) {
	    OutputBuffer [opp] = ' ';
	    ++ opp;
	}
    }
    OutputBuffer [opp] = ' ';
    ++ opp;
    for (int i = 0; i < 16; ++i) {
	char c = FileBuffer[i];
	c = isprint(c) ? c : ' ';
	OutputBuffer [opp++] = c;
    }

    for (int j = 0; j < 3; ++j)
	OutputBuffer [opp++] = ' ';
    OutputBuffer[opp] = '\x0';

    mvwaddstr (win, row, 0, OutputBuffer);
}
