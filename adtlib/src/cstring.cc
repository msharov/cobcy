/* cstring.cc
**
**	Named to avoid conflict with ANSI string.h . Stands for class String.
*/

#include <cstring.h>

String :: String (void)
{
    m_MemUsed = 0;
    m_MemAllocated = 1;
    m_Data = new char [1];
    m_Data[0] = '\x0';
}

String :: String (char NewData)
{
    m_MemUsed = 1;
    m_MemAllocated = 2;
    m_Data = new char [2];
    m_Data[0] = NewData;
    m_Data[1] = '\x0';
}

String :: String (const char * NewData)
{
    m_MemUsed = strlen (NewData);
    m_MemAllocated = m_MemUsed + 1;
    m_Data = new char [m_MemAllocated];
    strncpy (m_Data, NewData, m_MemUsed);
    m_Data [m_MemUsed] = '\x0';
}

String :: String (WORD Size)
{
    m_MemUsed = 0;
    m_MemAllocated = Size + 1;
    m_Data = new char [m_MemAllocated];
    memset (m_Data, '\x0', m_MemAllocated);
}

String :: String (const String& NewData)
{
    m_MemUsed = NewData.m_MemUsed;
    m_MemAllocated = m_MemUsed + 1;
    m_Data = new char [m_MemAllocated];
    strncpy (m_Data, NewData.m_Data, m_MemUsed);
    m_Data [m_MemUsed] = '\x0';
}

void String :: Resize (WORD NewSize)
{
char * NewData;
    if (NewSize + 1 > m_MemAllocated) {
	NewData = new char [NewSize + 1];
	m_MemAllocated = NewSize + 1;
	m_MemUsed = min (NewSize, m_MemUsed);
	strncpy (NewData, m_Data, m_MemUsed);
	memset (NewData + m_MemUsed, '\x0', m_MemAllocated - m_MemUsed);
	delete [] m_Data;
	m_Data = NewData;
    }
    else {
	m_MemUsed = min (NewSize, m_MemUsed);
	memset (m_Data + m_MemUsed, '\x0', m_MemAllocated - m_MemUsed);
    }
}

String& String :: Left (WORD Count) const
{
String * NewString;
    assert (Count < m_MemUsed);
    NewString = new String (Count);
    strncpy (NewString->m_Data, m_Data, Count);
    NewString->m_MemUsed = Count;
    return (*NewString);
}

String& String :: Middle (WORD Start, WORD Count) const
{
String * NewString;
    assert (Start + Count < m_MemUsed);
    NewString = new String (Count);
    strncpy (NewString->m_Data, m_Data + Start, Count);
    NewString->m_MemUsed = Count;
    return (*NewString);
}

String& String :: Right (WORD Count) const
{
String * NewString;
    assert (Count < m_MemUsed);
    // Here the constructor can see the end terminator, so
    //	no intervention is necessary
    NewString = new String (m_Data + (m_MemUsed - Count));
    return (*NewString);
}

char String :: GetAt (WORD idx) const
{
    assert (idx < m_MemUsed);
    return (m_Data[idx]);
}

void String :: SetAt (WORD idx, char Value)
{
    assert (idx < m_MemUsed);
    m_Data[idx] = Value;
}

void String :: InsertAt (WORD idx, char ToInsert)
{
    Resize (m_MemUsed + 1);
    assert (idx <= m_MemUsed);
    memmove (m_Data + idx + 1, m_Data + idx, m_MemUsed - idx);
    m_Data[idx] = ToInsert;
    ++ m_MemUsed;
}

void String :: InsertAt (WORD idx, const char * ToInsert)
{
int islen;
    islen = strlen (ToInsert);
    Resize (m_MemUsed + islen);
    assert (idx <= m_MemUsed);
    memmove (m_Data + idx + islen, m_Data + idx, m_MemUsed - idx);
    strncpy (m_Data + idx, ToInsert, islen);
    m_MemUsed += islen;
}

void String :: InsertAt (WORD idx, const String& ToInsert)
{
int islen;
    islen = ToInsert.Size();
    Resize (m_MemUsed + islen);
    assert (idx <= m_MemUsed);
    memmove (m_Data + idx + islen, m_Data + idx, m_MemUsed - idx);
    strncpy (m_Data + idx, ToInsert.m_Data, islen);
    m_MemUsed += islen;
}

void String :: DeleteAt (WORD idx, WORD Count = 1)
{
    assert (idx + Count < m_MemUsed);
    Resize (m_MemUsed - Count);
    memmove (m_Data + idx, m_Data + idx + Count, m_MemUsed - (idx + Count));
    m_MemUsed -= Count;
}

int String :: Index (const char ToFind) const
{
char * sptr;
    if ((sptr = strchr (m_Data, ToFind)) == NULL)
	return (-1);
    else
	return (m_MemUsed - strlen(sptr));
}

int String :: Index (const char * ToFind) const
{
char * sptr;
    if ((sptr = strstr (m_Data, ToFind)) == NULL)
	return (-1);
    else
	return (m_MemUsed - strlen(sptr));
}

int String :: Index (const String& ToFind) const
{
char * sptr;
    if ((sptr = strstr (m_Data, ToFind.m_Data)) == NULL)
	return (-1);
    else
	return (m_MemUsed - strlen(sptr));
}

String&	String :: operator= (const String& ToBe)
{
    Resize (ToBe.Size());
    strcpy (m_Data, ToBe.m_Data);
    return (*this);
}

String&	String :: operator= (const char ToBe)
{
    Resize (1);
    m_Data[0] = ToBe;
    return (*this);
}

String&	String :: operator= (const char * ToBe)
{
    Resize (strlen (ToBe));
    strcpy (m_Data, ToBe);
    return (*this);
}

String&	String :: operator+ (const String& ToAdd)
{
    Resize (m_MemUsed + ToAdd.Size());
    strcpy (m_Data + m_MemUsed, ToAdd.m_Data);
    m_MemUsed += ToAdd.Size();
    return (*this);
}

String&	String :: operator+ (const char ToAdd)
{
    Resize (m_MemUsed + 1);
    m_Data [m_MemUsed] = ToAdd;
    ++ m_MemUsed;
    return (*this);
}

String&	String :: operator+ (const char * ToAdd)
{
int AddLength;

    AddLength = strlen (ToAdd);
    Resize (m_MemUsed + AddLength);
    strcpy (m_Data + m_MemUsed, ToAdd);
    m_MemUsed += AddLength;
    return (*this);
}

void String :: ReadTextStream (istream& is)
{
    is >> m_Data;
    m_MemUsed = strlen (m_Data);
    assert (m_MemUsed < m_MemAllocated);
    Resize (m_MemUsed);
}

void String :: ReadBinaryStream (istream& is)
{
    ReadWord (is, &m_MemUsed);
    Resize (m_MemUsed);
    ReadString (is, m_Data, m_MemUsed);
}

void String :: WriteTextStream (ostream& os)
{
    os << m_Data;
}

void String :: WriteBinaryStream (ostream& os)
{
    WriteWord (os, &m_MemUsed);
    WriteString (os, m_Data, m_MemUsed);
}

String :: ~String (void)
{
    assert (m_Data != NULL);
    delete [] m_Data;
}

