/* cstring.h
**
**	Named to avoid conflict with ANSI string.h . Stands for class CString.
*/

#ifndef __CSTRING_H
#define __CSTRING_H

#include <string.h>
#include <assert.h>
#include <mdefs.h>
#include <streamab.h>
#include <array.h>

class CString : public Streamable {
private:
    char *		m_Data;
    WORD		m_MemUsed;
    WORD		m_MemAllocated;

public:
    			CString (void);
    			CString (char NewData);
    			CString (const char * NewData);
    			CString (int Size);
    			CString (const CString& NewData);

    inline WORD		Size (void) const;
    void		Resize (WORD NewSize);
    CString&		Left (WORD Count) const;
    CString&		Middle (WORD Start, WORD Count) const;
    CString&		Right (WORD Count) const;

    void		Append (WORD Num);
    void		Append (const char * NewData, WORD DataSize);

    char		GetAt (WORD idx) const;
    void		SetAt (WORD idx, char Value);
    void		InsertAt (WORD idx, char ToInsert);
    void		InsertAt (WORD idx, const char * ToInsert);
    void		InsertAt (WORD idx, const CString& ToInsert);
    void		DeleteAt (WORD idx, WORD Count = 1);

    int			Index (const char ToFind) const;
    int			Index (const char * ToFind) const;
    int			Index (const CString& ToFind) const;
    inline BOOL		Contains (const char ToFind) const;
    inline BOOL		Contains (const char * ToFind) const;
    inline BOOL		Contains (const CString& ToFind) const;
    Array<CString>	Split (const CString& delim = " \n\t") const;

    CString&		operator= (const CString& ToBe);
    CString&		operator= (const char ToBe);
    CString&		operator= (const char * ToBe);
    inline BOOL		operator== (const CString& ToCompare) const;
    inline BOOL		operator< (const CString& ToCompare) const;
    inline BOOL		operator> (const CString& ToCompare) const;
    inline char&	operator[] (WORD idx);
    inline const char&	operator[] (WORD idx) const;
    inline		operator const char * (void) const;
    inline		operator char * (void);
    CString&		operator+ (const CString& ToAdd);
    CString&		operator+ (const char ToAdd);
    CString&		operator+ (const char * ToAdd);

    virtual	       ~CString (void);

    virtual void	ReadTextStream (istream& is);
    virtual void	ReadBinaryStream (istream& is);
    virtual void	WriteTextStream (ostream& os);
    virtual void	WriteBinaryStream (ostream& os);
    virtual WORD	StreamSize (void) const;
};

/*-------------------------------------------------------------------------*/

inline WORD CString :: Size (void) const
{
    return (m_MemUsed);
}

inline BOOL CString :: Contains (const char ToFind) const
{
    return (Index (ToFind) >= 0);
}

inline BOOL CString :: Contains (const char * ToFind) const
{
    return (Index (ToFind) >= 0);
}

inline BOOL CString :: Contains (const CString& ToFind) const
{
    return (Index (ToFind) >= 0);
}

inline BOOL CString :: operator== (const CString& ToCompare) const
{
    return (memcmp (m_Data, ToCompare.m_Data, m_MemUsed) == 0);
}

inline BOOL CString :: operator> (const CString& ToCompare) const
{
    return (memcmp (m_Data, ToCompare.m_Data, m_MemUsed) > 0);
}

inline BOOL CString :: operator< (const CString& ToCompare) const
{
    return (memcmp (m_Data, ToCompare.m_Data, m_MemUsed) < 0);
}

inline char& CString :: operator[] (WORD idx)
{
    assert (idx < m_MemUsed);
    return (m_Data[idx]);
}

inline const char& CString :: operator[] (WORD idx) const
{
    assert (idx < m_MemUsed);
    return (m_Data[idx]);
}

inline CString :: operator const char * (void) const
{
    return (m_Data);
}

inline CString :: operator char * (void)
{
    return (m_Data);
}

#endif

