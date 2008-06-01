/* semscreen.h
**
**	Defines screen semantic actions for COBOL compiler.
*/

#ifndef __SEMSCREEN_H
#define __SEMSCREEN_H

#include <mdefs.h>
#include "semtypes.h"

/*-------------------------------------------*/
  void  BeginScreenField (void);
  void  EndScreenField (void);
  void	CloseScreenSection (void);
  void  SetScreenFieldName (void);
  void  SetScreenFieldLine (void);
  void  SetScreenFieldColumn (void);
  void  SetScreenForeColor (void);
  void  SetScreenBackColor (void);
  void  SetScreenDefaultValue (void);
  void  SetScreenFieldPicture (void);
  void  SetScreenFieldSource (void);
  void  SetScreenFieldAttr  (SCRAttr newAttr);
  void  SetScreenBlankFlag  (BOOL newValue);
/*-------------------------------------------*/

#endif

