/* display.h
**
**	Prototypes of display routines for the DBF viewer.
*/

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <mdefs.h>
#include <ncurses.h>

/*---------------------------------------------------------------------------*/
  void 	ViewBin (FILE *df);
  void 	DisplayOpen (void);
  void 	DisplayClose (void);
  void 	SetAttrNormal (WINDOW *win);
  void 	SetAttrHighlight (WINDOW *win);
  void 	SetAttrHeader (WINDOW *win);
  void 	ClearCursesWindow (WINDOW *win);
  void 	DisplayHeader (WINDOW * win, WORD row, FILE * df);
  void 	DisplayRecord (WINDOW * win, WORD row, char FileBuffer[16], WORD offset);
/*---------------------------------------------------------------------------*/

#endif

