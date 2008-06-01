/* display.c
**
**	Display routines for the DBF editor
*/

#include <string.h>
#include "display.h"

void ViewDBF (DBF_FILE * df)
{
WORD Field = 0, Row = 0, TopRow = 0;
WORD maxx, maxy, MaxVisRecords;
WORD key = 0, i;
const char UsageString[81] = " Use arrows, pgup/pgdn, home/end to move; 'q' to quit                            ";

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
	  	if (Row < df->Header.nRecords - 1 - MaxVisRecords)
	  	   Row += MaxVisRecords;
		else
		   Row = df->Header.nRecords - 1;
	  	if (TopRow < df->Header.nRecords - 1 - MaxVisRecords)
	  	   TopRow += MaxVisRecords;
		else
		   TopRow = df->Header.nRecords - 1 - MaxVisRecords;
		break;
	  case KEY_HOME:
	        Row = 0;
		TopRow = 0;
		break;
	  case KEY_END:
	        Row = df->Header.nRecords - 1;
		TopRow = Row - MaxVisRecords;
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
       SetAttrHeader (stdscr);
       mvwaddstr (stdscr, maxy - 1, 0, UsageString);
       for (i = 0; i < MaxVisRecords; ++ i){
	  if (i + TopRow == Row)
	     SetAttrHighlight (stdscr);
	  else
	     SetAttrNormal (stdscr);
	  DisplayDBFRecord (stdscr, i + 1, Field, df, i + TopRow);
       }
       refresh();

       key = 0;
       key = getch();
    }
    while (key != 'q');
}

void DisplayOpen (void)
{
    initscr();
    noecho();
    cbreak();
    leaveok (stdscr, TRUE);
    keypad (stdscr, TRUE);
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
    keypad (stdscr, FALSE);
    leaveok (stdscr, FALSE);
    attrset (A_NORMAL);
    clear();
    refresh();
    endwin();
}

void SetAttrNormal (WINDOW * win)
{
    if (has_colors())
       wattrset (win, A_NORMAL | COLOR_PAIR(1));
    else
       wattrset (win, A_NORMAL);
}

void SetAttrHighlight (WINDOW * win)
{
    if (has_colors())
       wattrset (win, A_BOLD | COLOR_PAIR(2));
    else
       wattrset (win, A_BOLD);
}

void SetAttrHeader (WINDOW * win)
{
    if (has_colors())
       wattrset (win, COLOR_PAIR(3));
    else
       wattrset (win, A_BOLD);
}

void ClearCursesWindow (WINDOW * win)
{
int maxx, maxy;
int x, y;

    getmaxyx (win, maxy, maxx);
    for (y = 0; y < maxy; ++ y)
       for (x = 0; x < maxx; ++ x)
          mvwaddch (win, y, x, ' ');
}

void DisplayDBFHeader (WINDOW * win, WORD row, WORD field, DBF_FILE * df)
{
int i, j, x = 0, maxy, maxx;
char OutputBuffer[128], dummy[5];
int NameLength, FieldLength;

    getmaxyx (win, maxy, maxx);
    memset (OutputBuffer, 0, 128);

    for (i = field; i < df->nFields && x < maxx; ++ i) {
       NameLength = min (maxx - x, strlen (df->Fields[i].Name));
       FieldLength = min (maxx - x, df->Fields[i].FieldLength);

       strcat (OutputBuffer, " ");
       strncat (OutputBuffer, df->Fields[i].Name, NameLength);
       for (j = 0; j < FieldLength - NameLength; ++ j)
	  strcat (OutputBuffer, " ");
       strcat (OutputBuffer, " ");
       dummy[0] = '|';
       strncat (OutputBuffer, dummy, 1);

       x += 3 + max (FieldLength, NameLength);
    }
    if (x >= maxx)
       memset (&OutputBuffer[maxx], 0, 128 - maxx);

    mvwaddstr (win, row, 0, OutputBuffer);
}

void DisplayDBFRecord (WINDOW * win, WORD row, WORD field, 
			DBF_FILE * df, WORD record)
{
int i, j, ri, x = 0;
char OutputBuffer[128], dummy[5];
int NameLength, FieldLength, DataLength;
int maxx, maxy;
char * RecordBuffer;

    getmaxyx (win, maxy, maxx);
    memset (OutputBuffer, 0, 128);

    DBF_SeekToRecord (df, record);
    RecordBuffer = (char*) malloc (df->Header.RecordLength);
    DBF_ReadRecord (df, RecordBuffer);

    ri = 0;
    for (i = 0; i < field; ++ i)
       ri += df->Fields[i].FieldLength;
    for (i = field; i < df->nFields && x < maxx; ++ i) {
       NameLength = min (maxx - x, strlen (df->Fields[i].Name));
       FieldLength = min (maxx - x, df->Fields[i].FieldLength);
       DataLength = min (strlen (&RecordBuffer[ri]), FieldLength);
       DataLength = min (maxx - x, DataLength);

       strcat (OutputBuffer, " ");
       strncat (OutputBuffer, &RecordBuffer[ri], DataLength);
       for (j = 0; j < FieldLength - DataLength; ++ j)
	  strncat (OutputBuffer, " ", 1);
       for (j = 0; j < NameLength - FieldLength; ++ j)
	  strncat (OutputBuffer, " ", 1);
       strncat (OutputBuffer, " ", 1);
       dummy[0] = '|';
       strncat (OutputBuffer, dummy, 1);

       x += 3 + max (FieldLength, NameLength);
       ri += FieldLength;
    }
    if (x >= maxx)
       memset (&OutputBuffer[maxx], 0, 128 - maxx);

    mvwaddstr (win, row, 0, OutputBuffer);

    free (RecordBuffer);
}

