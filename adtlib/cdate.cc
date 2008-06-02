/* cdate.cc
**
**	Implementation of a date encapsulation.
*/

#include <time.h>
#include "cdate.h"
#include "bitarray.h"

const char WeekdayNames[7][4]= {"Sun", "Mon", "Tue", "Wed", 
				"Thu", "Fri", "Sat"};
const char MonthNames[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

CDate :: CDate (void)
{
    tm_sec = 0;
    tm_min = 0;
    tm_hour = 0;
    tm_mday = 0;
    tm_mon = Jan;
    tm_year = 0;
    tm_wday = Sun;
    tm_yday = 0;
}

CDate :: CDate (const struct tm * ToBe)
{
    tm_sec = ToBe->tm_sec;
    tm_min = ToBe->tm_min;
    tm_hour = ToBe->tm_hour;
    tm_mday = ToBe->tm_mday;
    tm_mon = ToBe->tm_mon;
    tm_year = ToBe->tm_year + 1900;
    tm_wday = ToBe->tm_wday;
    tm_yday = ToBe->tm_yday;
}

CDate :: CDate (const CDate& ToBe)
{
    tm_sec = ToBe.tm_sec;
    tm_min = ToBe.tm_min;
    tm_hour = ToBe.tm_hour;
    tm_mday = ToBe.tm_mday;
    tm_mon = ToBe.tm_mon;
    tm_year = ToBe.tm_year;
    tm_wday = ToBe.tm_wday;
    tm_yday = ToBe.tm_yday;
}

void CDate :: Update (void)
{
time_t RawCurTime;
struct tm * CurTime;

    time (&RawCurTime);
    CurTime = localtime (&RawCurTime);
    tm_sec = CurTime->tm_sec;
    tm_min = CurTime->tm_min;
    tm_hour = CurTime->tm_hour;
    tm_mday = CurTime->tm_mday;
    tm_mon = CurTime->tm_mon;
    tm_year = CurTime->tm_year + 1900;
    tm_wday = CurTime->tm_wday;
    tm_yday = CurTime->tm_yday;
}

void CDate :: GetTime (WORD * hour, WORD * min, WORD * sec) const
{
    *hour = tm_hour;
    *min = tm_min;
    *sec = tm_sec;
}

void CDate :: SetTime (WORD hour, WORD min, WORD sec)
{
    tm_hour = hour;
    tm_min = min;
    tm_sec = sec;
}

void CDate :: GetDate (WORD * year, WORD * month, WORD * day) const
{
    *year = tm_year;
    *month = tm_mon;
    *day = tm_mday;
}

void CDate :: SetDate (WORD year, WORD month, WORD day)
{
    tm_year = year;
    tm_mon = month;
    tm_mday = day;
}

WORD CDate :: GetWeekday (void) const
{
    return (tm_wday);
}

WORD CDate :: GetYearDay (void) const
{
    return (tm_yday);
}

bool CDate :: IsValid (void) const
{
    if (tm_sec <= 61 && tm_min <= 61 && tm_hour <= 24 &&
	tm_mday >= 1 && tm_mday <= 31 && tm_mon <= 61 &&
	tm_year >= 1900)
	return (true);
    else
	return (false);
}

CDate& CDate :: operator= (const CDate& ToBe)
{
    tm_sec = ToBe.tm_sec;
    tm_min = ToBe.tm_min;
    tm_hour = ToBe.tm_hour;
    tm_mday = ToBe.tm_mday;
    tm_mon = ToBe.tm_mon;
    tm_year = ToBe.tm_year;
    tm_wday = ToBe.tm_wday;
    tm_yday = ToBe.tm_yday;
    return (*this);
}

bool CDate :: operator== (const CDate& ToCompare)
{
    if (tm_sec == ToCompare.tm_sec && tm_min == ToCompare.tm_min &&
	tm_hour == ToCompare.tm_hour && tm_mday == ToCompare.tm_mday &&
	tm_mon == ToCompare.tm_mon && tm_year == ToCompare.tm_year)
	return (true);
    else
	return (false);
}

bool CDate :: operator> (const CDate& ToCompare)
{
    if (tm_sec > ToCompare.tm_sec && tm_min >= ToCompare.tm_min &&
	tm_hour >= ToCompare.tm_hour && tm_mday >= ToCompare.tm_mday &&
	tm_mon >= ToCompare.tm_mon && tm_year >= ToCompare.tm_year)
	return (true);
    else
	return (false);
}

bool CDate :: operator< (const CDate& ToCompare)
{
    if (tm_sec < ToCompare.tm_sec && tm_min <= ToCompare.tm_min &&
	tm_hour <= ToCompare.tm_hour && tm_mday <= ToCompare.tm_mday &&
	tm_mon <= ToCompare.tm_mon && tm_year <= ToCompare.tm_year)
	return (true);
    else
	return (false);
}

bool CDate :: operator>= (const CDate& ToCompare)
{
    if (tm_sec >= ToCompare.tm_sec && tm_min >= ToCompare.tm_min &&
	tm_hour >= ToCompare.tm_hour && tm_mday >= ToCompare.tm_mday &&
	tm_mon >= ToCompare.tm_mon && tm_year >= ToCompare.tm_year)
	return (true);
    else
	return (false);
}

bool CDate :: operator<= (const CDate& ToCompare)
{
    if (tm_sec <= ToCompare.tm_sec && tm_min <= ToCompare.tm_min &&
	tm_hour <= ToCompare.tm_hour && tm_mday <= ToCompare.tm_mday &&
	tm_mon <= ToCompare.tm_mon && tm_year <= ToCompare.tm_year)
	return (true);
    else
	return (false);
}

CDate& CDate :: operator+ (const CDate& ToAdd)
{
    tm_sec += ToAdd.tm_sec;
    tm_min += ToAdd.tm_min;
    tm_hour += ToAdd.tm_hour;
    tm_mday += ToAdd.tm_mday;
    tm_mon = (tm_mon + ToAdd.tm_mon) % 12;
    tm_year += ToAdd.tm_year;
    tm_wday = (tm_wday + ToAdd.tm_wday) % 7;
    tm_yday += ToAdd.tm_yday;
    return (*this);
}

CDate& CDate :: operator- (const CDate& ToSub)
{
    tm_sec -= ToSub.tm_sec;
    tm_min -= ToSub.tm_min;
    tm_hour -= ToSub.tm_hour;
    tm_mday -= ToSub.tm_mday;
    tm_mon = (tm_mon - ToSub.tm_mon) % 12;
    tm_year -= ToSub.tm_year;
    tm_wday = (tm_wday - ToSub.tm_wday) % 7;
    tm_yday -= ToSub.tm_yday;
    return (*this);
}

void CDate :: WriteTextStream (ostream& os)
{
    os << WeekdayNames [tm_wday] << " " << MonthNames [tm_mon] << " ";
    if (tm_mday < 10)
	os << " ";
    os << tm_mday << " ";
    os << tm_hour << ":" << tm_min << ":" << tm_sec << " " << tm_year;
    os << "\n";
}

void CDate :: ReadBinaryStream (istream& is)
{
BitArray CompactDate (50);

    CompactDate.ReadBinaryStream (is);
    assert (CompactDate.Size() == 50);
    tm_sec = CompactDate.GetAt (0, 6);
    tm_min = CompactDate.GetAt (6, 12);
    tm_hour = CompactDate.GetAt (12, 17);
    tm_mday = CompactDate.GetAt (17, 22);
    tm_mon = CompactDate.GetAt (22, 26);
    tm_year = CompactDate.GetAt (26, 38);
    tm_wday = CompactDate.GetAt (38, 41);
    tm_yday = CompactDate.GetAt (41, 50);
}

void CDate :: WriteBinaryStream (ostream& os)
{
BitArray CompactDate (50);

    CompactDate.SetAt (0, 6, tm_sec);
    CompactDate.SetAt (6, 12, tm_min);
    CompactDate.SetAt (12, 17, tm_hour);
    CompactDate.SetAt (17, 22, tm_mday);
    CompactDate.SetAt (22, 26, tm_mon);
    CompactDate.SetAt (26, 38, tm_year);
    CompactDate.SetAt (38, 41, tm_wday);
    CompactDate.SetAt (41, 50, tm_yday);
    CompactDate.WriteBinaryStream (os);
}

WORD CDate :: StreamSize (void) const
{
BitArray CompactDate (50);

    return (CompactDate.StreamSize());
}

