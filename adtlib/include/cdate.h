/* cdate.h
**
**	C++ encapsulation of date and time functions.
*/

#ifndef __CDATE_H
#define __CDATE_H

#include <mdefs.h>
#include <streamab.h>

class CDate : public Streamable {
public:
    typedef enum {
	Sun = 0, Mon, Tue, Wed, Thu, Fri, Sat
    }		WeekdayEnum;
    typedef enum {
	Jan = 0, Feb, Mar, Apr, May, Jun, Jul, Aug, Sep, Oct, Nov, Dec
    }		MonthEnum;

protected:
    WORD		tm_sec;
    WORD		tm_min;
    WORD		tm_hour;
    WORD		tm_mday;
    WORD		tm_mon;
    WORD		tm_year;
    WORD		tm_wday;
    WORD		tm_yday;

public:
			CDate (void);
			CDate (const struct tm * ToBe);
			CDate (const CDate& ToBe);
    void		Update (void);
    void		GetTime (WORD * hour, WORD * min, WORD * sec) const;
    void		SetTime (WORD hour, WORD min, WORD sec);
    void		GetDate (WORD * year, WORD * month, WORD * day) const;
    void		SetDate (WORD year, WORD month, WORD day);
    WORD		GetWeekday (void) const;
    WORD		GetYearDay (void) const;
    BOOL		IsValid (void) const;

    CDate&		operator= (const CDate& ToBe);
    BOOL		operator== (const CDate& ToCompare);
    BOOL		operator> (const CDate& ToCompare);
    BOOL		operator< (const CDate& ToCompare);
    CDate&		operator+ (const CDate& ToAdd);
    CDate&		operator- (const CDate& ToSub);

    virtual void	WriteTextStream (ostream& is);
    virtual void	ReadBinaryStream (istream& is);
    virtual void	WriteBinaryStream (ostream& os);
};

#endif

