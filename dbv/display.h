/* display.h
**
**	Prototypes of display routines for the DBF viewer.
*/

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <mdefs.h>
#include <ncurses.h>
#include "dbf.h"

/*---------------------------------------------------------------------------*/
  void 	ViewDBF (DBF_FILE *df);
  void 	DisplayOpen (void);
  void 	DisplayClose (void);
  void 	SetAttrNormal (WINDOW *win);
  void 	SetAttrHighlight (WINDOW *win);
  void 	SetAttrHeader (WINDOW *win);
  void 	ClearCursesWindow (WINDOW *win);
  void 	DisplayDBFHeader (WINDOW * win, uint32_t row, uint32_t field, DBF_FILE * df);
  void 	DisplayDBFRecord (WINDOW * win, uint32_t row, uint32_t field, DBF_FILE * df, uint32_t record);
/*---------------------------------------------------------------------------*/

#endif

