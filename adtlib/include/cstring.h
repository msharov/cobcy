/* cstring.h
**
**	Named to avoid conflict with ANSI string.h . Stands for class String.
*/

#ifndef __CSTRING_H
#define __CSTRING_H

#include <string.h>
#include <assert.h>
#include <mdefs.h>
#include <streamab.h>

class String : public Streamable {
private:
    char *		m_Data;
    WORD		m_MemUsed;
    WORD		m_MemAllocated;

public:
    			String (void);
    			String (char NewData);
    			String (const char * NewData);
    			String (WORD Size);
    			String (const String& NewData);

    inline WORD		Size (void) const;
    void		Resize (WORD NewSize);
    String&		Left (WORD Count) const;
    String&		Middle (WORD Start, WORD Count) const;
    String&		Right (WORD Count) const;

    char		GetAt (WORD idx) const;
    void		SetAt (WORD idx, char Value);
    void		InsertAt (WORD idx, char ToInsert);
    void		InsertAt (WORD idx, const char * ToInsert);
    void		InsertAt (WORD idx, const String& ToInsert);
    void		DeleteAt (WORD idx, WORD Count = 1);

    int			Index (const char ToFind) const;
    int			Index (const char * ToFind) const;
    int			Index (const String& ToFind) const;
    inline BOOL		Contains (const char ToFind) const;
    inline BOOL		Contains (const char * ToFind) const;
    inline BOOL		Contains (const String& ToFind) const;

    String&		operator= (const String& ToBe);
    String&		operator= (const char ToBe);
    String&		operator= (const char * ToBe);
    inline BOOL		operator== (const String& ToCompare) const;
    inline char&	operator[] (WORD idx);
    inline const char&	operator[] (WORD idx) const;
    inline		operator const char * (void) const;
    String&		operator+ (const String& ToAdd);
    String&		operator+ (const char ToAdd);
    String&		operator+ (const char * ToAdd);

    virtual	       ~String (void);

    virtual void	ReadTextStream (istream& is);
    virtual void	ReadBinaryStream (istream& is);
    virtual void	WriteTextStream (ostream& os);
    virtual void	WriteBinaryStream (ostream& os);
};

/*-------------------------------------------------------------------------*/

inline WORD String :: Size (void) const
{
    return (m_MemUsed);
}

inline BOOL String :: Contains (const char ToFind) const
{
    return (Index (ToFind) >= 0);
}

inline BOOL String :: Contains (const char * ToFind) const
{
    return (Index (ToFind) >= 0);
}

inline BOOL String :: Contains (const String& ToFind) const
{
    return (Index (ToFind) >= 0);
}

inline BOOL String :: operator== (const String& ToCompare) const
{
    return (strcmp (m_Data, ToCompare.m_Data) == 0);
}

inline char& String :: operator[] (WORD idx)
{
    assert (idx < m_MemUsed);
    return (m_Data[idx]);
}

inline const char& String :: operator[] (WORD idx) const
{
    assert (idx < m_MemUsed);
    return (m_Data[idx]);
}

inline String :: operator const char * (void) const
{
    return (m_Data);
}

#endif

