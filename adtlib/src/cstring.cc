/* cstring.cc
**
**	Named to avoid conflict with ANSI string.h . Stands for class CString.
*/

#include <cstring.h>

CString :: CString (void)
{
    m_MemUsed = 0;
    m_MemAllocated = 1;
    m_Data = new char [1];
    assert (m_Data != NULL);
    m_Data[0] = '\x0';
}

CString :: CString (char NewData)
{
    m_MemUsed = 1;
    m_MemAllocated = 2;
    m_Data = new char [2];
    assert (m_Data != NULL);
    m_Data[0] = NewData;
    m_Data[1] = '\x0';
}

CString :: CString (const char * NewData)
{
    m_MemUsed = strlen (NewData);
    m_MemAllocated = m_MemUsed + 1;
    m_Data = new char [m_MemAllocated];
    assert (m_Data != NULL);
    memcpy (m_Data, NewData, m_MemUsed);
    m_Data [m_MemUsed] = '\x0';
}

CString :: CString (int Size)
{
    m_MemUsed = 0;
    m_MemAllocated = Size + 1;
    m_Data = new char [m_MemAllocated];
    assert (m_Data != NULL);
    memset (m_Data, '\x0', m_MemAllocated);
}

CString :: CString (const CString& NewData)
{
    m_MemUsed = NewData.m_MemUsed;
    m_MemAllocated = m_MemUsed + 1;
    m_Data = new char [m_MemAllocated];
    assert (m_Data != NULL);
    memcpy (m_Data, NewData.m_Data, m_MemUsed);
    m_Data [m_MemUsed] = '\x0';
}

void CString :: Resize (WORD NewSize)
{
char * NewData;
    if (NewSize + 1 > m_MemAllocated) {
	NewData = new char [NewSize + 1];
	assert (m_Data != NULL);
	m_MemAllocated = NewSize + 1;
	m_MemUsed = min (NewSize, m_MemUsed);
	memcpy (NewData, m_Data, m_MemUsed);
	memset (NewData + m_MemUsed, '\x0', m_MemAllocated - m_MemUsed);
	delete [] m_Data;
	m_Data = NewData;
    }
    else {
	m_MemUsed = min (NewSize, m_MemUsed);
	memset (m_Data + m_MemUsed, '\x0', m_MemAllocated - m_MemUsed);
    }
}

CString& CString :: Left (WORD Count) const
{
CString * NewCString;
    assert (Count < m_MemUsed);
    NewCString = new CString ((int) Count);
    memcpy (NewCString->m_Data, m_Data, Count);
    NewCString->m_MemUsed = Count;
    return (*NewCString);
}

CString& CString :: Middle (WORD Start, WORD Count) const
{
CString * NewCString;
    assert (Start + Count < m_MemUsed);
    NewCString = new CString ((int) Count);
    assert (NewCString != NULL);
    memcpy (NewCString->m_Data, m_Data + Start, Count);
    NewCString->m_MemUsed = Count;
    return (*NewCString);
}

CString& CString :: Right (WORD Count) const
{
CString * NewCString;
    assert (Count < m_MemUsed);
    // Here the constructor can see the end terminator, so
    //	no intervention is necessary
    NewCString = new CString (m_Data + (m_MemUsed - Count));
    assert (NewCString != NULL);
    return (*NewCString);
}

char CString :: GetAt (WORD idx) const
{
    assert (idx < m_MemUsed);
    return (m_Data[idx]);
}

void CString :: SetAt (WORD idx, char Value)
{
    assert (idx < m_MemUsed);
    m_Data[idx] = Value;
}

void CString :: InsertAt (WORD idx, char ToInsert)
{
    Resize (m_MemUsed + 1);
    assert (idx <= m_MemUsed);
    memmove (m_Data + idx + 1, m_Data + idx, m_MemUsed - idx);
    m_Data[idx] = ToInsert;
    ++ m_MemUsed;
}

void CString :: InsertAt (WORD idx, const char * ToInsert)
{
int islen;
    islen = strlen (ToInsert);
    Resize (m_MemUsed + islen);
    assert (idx <= m_MemUsed);
    memmove (m_Data + idx + islen, m_Data + idx, m_MemUsed - idx);
    memcpy (m_Data + idx, ToInsert, islen);
    m_MemUsed += islen;
}

void CString :: InsertAt (WORD idx, const CString& ToInsert)
{
int islen;
    islen = ToInsert.Size();
    Resize (m_MemUsed + islen);
    assert (idx <= m_MemUsed);
    memmove (m_Data + idx + islen, m_Data + idx, m_MemUsed - idx);
    memcpy (m_Data + idx, ToInsert.m_Data, islen);
    m_MemUsed += islen;
}

void CString :: DeleteAt (WORD idx, WORD Count = 1)
{
    assert (idx + Count < m_MemUsed);
    Resize (m_MemUsed - Count);
    memmove (m_Data + idx, m_Data + idx + Count, m_MemUsed - (idx + Count));
    m_MemUsed -= Count;
}

int CString :: Index (const char ToFind) const
{
char * sptr;
    if ((sptr = (char*) memchr (m_Data, ToFind, m_MemUsed)) == NULL)
	return (-1);
    else
	return (m_MemUsed - strlen(sptr));
}

int CString :: Index (const char * ToFind) const
{
char * sptr;
    if ((sptr = strstr (m_Data, ToFind)) == NULL)
	return (-1);
    else
	return (m_MemUsed - strlen(sptr));
}

int CString :: Index (const CString& ToFind) const
{
char * sptr;
    if ((sptr = strstr (m_Data, ToFind.m_Data)) == NULL)
	return (-1);
    else
	return (m_MemUsed - strlen(sptr));
}

Array<CString> CString :: Split (const CString& delim) const
{
WORD i, nPieces = 0, SplitStart, SplitEnd;
Array<CString> Pieces;

    for (i = 0; i < m_MemUsed; ++ i)
	if (delim.Contains (m_Data[i]))
	    ++ nPieces;

    Pieces.Resize (nPieces);
    Pieces [0] = *this;

    for (i = 1; i < nPieces; ++ i) {
	SplitStart = 0;
	SplitEnd = 0;
	while (SplitStart < Pieces [i - 1].m_MemUsed && 
	    	!delim.Contains (Pieces[i - 1].m_Data[SplitStart]))
	    ++ SplitStart;
	SplitEnd = SplitStart;
	while (SplitEnd < Pieces [i - 1].m_MemUsed && 
	    	delim.Contains (Pieces[i - 1].m_Data[SplitEnd]))
	    ++ SplitEnd;
	-- SplitEnd;

	Pieces [i] = Pieces [i - 1].Right (m_MemUsed - SplitEnd);
	Pieces [i - 1] = Pieces [i - 1].Left (SplitStart);
    }

    return (Pieces);
}

void CString :: Append (WORD Num)
{
char numbuf [15];
    sprintf (numbuf, "%ld", Num);
    Resize (m_MemUsed + strlen (numbuf));
    memcpy (m_Data + m_MemUsed, numbuf, strlen (numbuf));
}

void CString :: Append (const char * NewData, WORD DataSize)
{
    Resize (m_MemUsed + DataSize);
    memcpy (m_Data + m_MemUsed, NewData, DataSize);
}

CString& CString :: operator= (const CString& ToBe)
{
    Resize (ToBe.Size());
    memcpy (m_Data, ToBe.m_Data, m_MemUsed);
    return (*this);
}

CString& CString :: operator= (const char ToBe)
{
    Resize (1);
    m_Data[0] = ToBe;
    return (*this);
}

CString& CString :: operator= (const char * ToBe)
{
    Resize (strlen (ToBe));
    memcpy (m_Data, ToBe, m_MemUsed);
    return (*this);
}

CString& CString :: operator+ (const CString& ToAdd)
{
    Resize (m_MemUsed + ToAdd.Size());
    memcpy (m_Data + m_MemUsed, ToAdd.m_Data, ToAdd.m_MemUsed);
    m_MemUsed += ToAdd.Size();
    return (*this);
}

CString& CString :: operator+ (const char ToAdd)
{
    Resize (m_MemUsed + 1);
    m_Data [m_MemUsed] = ToAdd;
    ++ m_MemUsed;
    return (*this);
}

CString& CString :: operator+ (const char * ToAdd)
{
int AddLength;

    AddLength = strlen (ToAdd);
    Resize (m_MemUsed + AddLength);
    memcpy (m_Data + m_MemUsed, ToAdd, strlen(ToAdd));
    m_MemUsed += AddLength;
    return (*this);
}

void CString :: ReadTextStream (istream& is)
{
    is >> m_Data;
    m_MemUsed = strlen (m_Data);
    assert (m_MemUsed < m_MemAllocated);
    Resize (m_MemUsed);
}

void CString :: ReadBinaryStream (istream& is)
{
    ReadWord (is, &m_MemUsed);
    Resize (m_MemUsed);
    ReadString (is, m_Data, m_MemUsed);
}

void CString :: WriteTextStream (ostream& os)
{
    os << m_Data;
}

void CString :: WriteBinaryStream (ostream& os)
{
    WriteWord (os, &m_MemUsed);
    WriteString (os, m_Data, m_MemUsed);
}

WORD CString :: StreamSize (void) const
{
    return (sizeof(WORD) + m_MemUsed);
}

CString :: ~CString (void)
{
    assert (m_Data != NULL);
    delete [] m_Data;
}

