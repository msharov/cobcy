/* semconio.h
**
*/

#ifndef __SEMCONIO_H
#define __SEMCONIO_H

typedef enum {
    AS_Date,
    AS_Day,
    AS_Weekday,
    AS_Time,
    AS_Console
} AcceptSourceType;

/*----------------------------------------------------*/
  void 	GenAccept (void);
  void 	GenDisplay (void);
  void 	SetDisplayOutput (void);
  void	SetAcceptSource (AcceptSourceType NewSrc);
/*----------------------------------------------------*/

#endif

