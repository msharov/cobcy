// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "../coblib/cobfunc.h"
#include <curses.h>
#include <ctype.h>

//----------------------------------------------------------------------

typedef char row_t [16];

static void ViewBin (const char* filename);
static void DisplayOpen (void);
static void DeleteWindows (void);
static void CreateWindows (void);

//----------------------------------------------------------------------

static WINDOW* _wframe = NULL;
static WINDOW* _whex = NULL;

//----------------------------------------------------------------------

int main (int argc, const char* const* argv)
{
    if (argc != 2) {
	printf ("Usage: %s <file>\n", argv[0]);
	return EXIT_SUCCESS;
    }
    _InstallSignalHandlers();
    ViewBin (argv[1]);
    return EXIT_SUCCESS;
}

static void ViewBin (const char* filename)
{
    FILE* df = NULL;
    _OpenSequentialFile (&df, filename, "rb");
    fseek (df, 0, SEEK_END);
    size_t fsz = ftell (df);
    unsigned nrows = (fsz+15)/16;
    row_t* filebuf = _AllocateBytes (nrows*sizeof(row_t));
    fseek (df, 0, SEEK_SET);
    fread (filebuf, fsz, 1, df);
    _CloseSequentialFile (df);

    DisplayOpen();
    unsigned sel = 0, top = 0, visrows = 0, key = KEY_RESIZE;
    do {
	switch (key) {
	    case 'k':
	    case KEY_UP:
		if (sel > 0)
		    --sel;
		break;
	    case 'j':
	    case KEY_DOWN:
		if (sel < nrows-1)
		    ++sel;
		break;
	    case KEY_PPAGE:
		if (sel > visrows) {
		    sel -= visrows;
		    top = max (0, top - visrows);
		    break;
		}	// fallthrough
	    case 'K':
	    case KEY_HOME:
		sel = 0;
		break;
	    case ' ':
	    case KEY_NPAGE:
		if (sel < nrows - visrows) {
		    sel += visrows;
		    top = min (top + visrows, nrows - visrows);
		    break;
		}	// fallthrough
	    case 'J':
	    case KEY_END:
		sel = nrows-1;
		break;
	    case KEY_RESIZE:
		DeleteWindows();
		CreateWindows();
		visrows = min ((unsigned) getmaxy(_whex), nrows);
		break;
	}
	if (sel < top)
	    top = sel;
	if (sel >= top + getmaxy(_whex))
	    top = min (sel, nrows-visrows);

	werase (_wframe);
	mvwaddstr (_wframe, 0, 1, filename);
	mvwprintw (_wframe, getmaxy(_wframe)-1, 1, "%08X | Use arrows, pgup/pgdn, home/end to move; 'q' to quit", fsz);
	mvwprintw (_wframe, getmaxy(_wframe)-1, getmaxx(_wframe)-5, "%3u%%", sel*100/nrows);
	wnoutrefresh (_wframe);

	werase (_whex);
	for (unsigned i = 0; i < visrows; ++ i){
	    if (top+i == sel)
		wattrset (_whex, has_colors() ? COLOR_PAIR(2)|A_BOLD : A_REVERSE);
	    else
		wattrset (_whex, has_colors() ? COLOR_PAIR(1) : A_NORMAL);
	    mvwprintw (_whex, i, 0, " %08X:  ", (top+i)*16);
	    const char* line = filebuf[i+top];
	    for (unsigned j = 0; j < 16; ++j)
		wprintw (_whex, "%02hhX%s ", line[j], " "+!!((j+1)%8));
	    for (unsigned j = 0; j < 16; ++j)
		waddch (_whex, isprint(line[j]) ? line[j] : ' ');
	    waddstr (_whex, "  ");
	}
	wrefresh (_whex);
    } while ((key = wgetch (_whex)) != 'q');

    free (filebuf);
}

static void DeleteWindows (void)
{
    if (_wframe)
	delwin (_wframe);
    if (_whex)
	delwin (_whex);
}

static void CreateWindows (void)
{
    unsigned frw = min (COLS, 80);
    _wframe = newwin (LINES, frw, 0, (COLS-frw)/2);
    wbkgdset (_wframe, ' '|(has_colors() ? COLOR_PAIR(3) : A_REVERSE));

    _whex = newwin (LINES-2, frw, 1, getbegx(_wframe));
    leaveok (_whex, true);
    keypad (_whex, true);
}

static void DisplayClose (void)
{
    if (isendwin())
	return;
    DeleteWindows();
    endwin();
}

void DisplayOpen (void)
{
    initscr();
    atexit (DisplayClose);
    noecho();
    cbreak();
    curs_set (0);
    if (has_colors()) {
	start_color();
	use_default_colors();
	init_pair (1, COLOR_WHITE, COLOR_BLUE);
	init_pair (2, COLOR_WHITE, COLOR_MAGENTA);
	init_pair (3, COLOR_BLUE, COLOR_WHITE);
    }
}
