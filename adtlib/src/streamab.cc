/* streamab.cc
**
**	Base class for file-active class.
*/

#include <streamab.h>
#include <cendian.h>

void ReadByte (istream& is, BYTE * buffer)
{
    is.read (buffer, sizeof(BYTE));
}

void WriteByte (ostream& os, const BYTE * buffer) 
{
    os.write (buffer, sizeof(BYTE));
}

void ReadRaw (istream& is, void * buffer, WORD size)
{
    is.read ((BYTE*) buffer, size);
}

void WriteRaw (ostream& os, const void * buffer, WORD size) 
{
    os.write ((BYTE*) buffer, size);
}

void ReadWord (istream& is, WORD * buffer)
{
    is.read (buffer, sizeof(WORD)); 
    *buffer = LittleEndianToLong (*buffer);
}

void WriteWord (ostream& os, const WORD * buffer) 
{
WORD TempBuf;
    TempBuf = LongToLittleEndian (*buffer);
    os.write (&TempBuf, sizeof(WORD)); 
}

void ReadString (istream& is, char * buffer, WORD s)
{
    is.read (buffer, sizeof(char) * s);
}

void WriteString (ostream& os, const char * buffer, WORD s) 
{
    if (s == 0)
	os.write (buffer, sizeof(char) * strlen(buffer));
    else
	os.write (buffer, sizeof(BYTE) * s);
}

void Streamable :: Load (char * filename)
{
#ifdef __MSDOS__
ifstream is (filename, ios::bin | ios::nocreate);
#else
ifstream is (filename, ios::bin);
#endif

    ReadBinaryStream (is);
}

void Streamable :: Save (char * filename)
{
#ifdef __MSDOS__
ofstream os (filename, ios::bin);
#else
ofstream os (filename, ios::bin);
#endif

    WriteBinaryStream (os);
}

WORD Streamable :: StreamSize (void) const
{
    cerr << "WARNING: Default GetStreamSize activated!\n";
    return (0);
}

void Streamable :: ReadBinaryStream (istream& is)
{
    if (is.eof())
	cerr << "ERROR: Default ReadBinaryStream activated at end of file!\n";
    else
	cerr << "WARNING: Default ReadBinaryStream activated!\n";
}

void Streamable :: WriteBinaryStream (ostream& os)
{
    if (os.eof())
	cerr << "ERROR: Default WriteBinaryStream activated at end of file!\n";
    else
	cerr << "WARNING: Default WriteBinaryStream activated!\n";
}

void Streamable :: ReadTextStream (istream& is)
{
    if (is.eof())
	cerr << "ERROR: Default ReadTextStream activated at end of file!\n";
    else
	cerr << "WARNING: Default ReadTextStream activated!\n";
}

void Streamable :: WriteTextStream (ostream& os)
{
    if (os.eof())
	cerr << "ERROR: Default WriteTextStream activated at end of file!\n";
    else
	cerr << "WARNING: Default WriteTextStream activated!\n";
}

void Streamable :: ReadBinaryStringStream (const char * StringBuf, WORD StrSize)
{
istrstream is (StringBuf, StrSize);
    ReadBinaryStream (is);
}

void Streamable :: WriteBinaryStringStream (char * StringBuf, WORD StrSize)
{
ostrstream os (StringBuf, StrSize);
    WriteBinaryStream (os);
    memcpy (StringBuf, os.rdbuf()->str(), StrSize);
}

void Streamable :: ReadTextStringStream (const char * StringBuf, WORD StrSize)
{
istrstream is (StringBuf, StrSize);
    ReadTextStream (is);
}

void Streamable :: WriteTextStringStream (char * StringBuf, WORD StrSize)
{
ostrstream os (StringBuf, StrSize);
    WriteTextStream (os);
    memcpy (StringBuf, os.rdbuf()->str(), StrSize);
}

istream& operator>> (istream& is, Streamable& obj)
{
    obj.ReadTextStream (is);
    return (is);
};

ostream& operator<< (ostream& is, Streamable& obj)
{
    obj.WriteTextStream (is);
    return (is);
};

