/* streamab.cc
**
**	Base class for file-active class.
*/

#include "streamab.h"
#include "cendian.h"

void ReadByte (istream& is, BYTE * buffer)
{
    is.read ((char*) buffer, sizeof(BYTE));
}

void WriteByte (ostream& os, const BYTE * buffer) 
{
    os.write ((const char*) buffer, sizeof(BYTE));
}

void ReadRaw (istream& is, void * buffer, WORD size)
{
    is.read ((char*) buffer, size);
}

void WriteRaw (ostream& os, const void * buffer, WORD size) 
{
    os.write ((const char*) buffer, size);
}

void ReadWord (istream& is, WORD * buffer)
{
    is.read ((char*) buffer, sizeof(WORD)); 
    *buffer = LittleEndianToLong (*buffer);
}

void WriteWord (ostream& os, const WORD * buffer) 
{
    WORD TempBuf = LongToLittleEndian (*buffer);
    os.write ((const char*) &TempBuf, sizeof(WORD)); 
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
    ifstream is (filename, ios::binary);
    ReadBinaryStream (is);
}

void Streamable :: Save (char * filename)
{
    ofstream os (filename, ios::binary);
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
    istringstream is (string (StringBuf, StrSize));
    ReadBinaryStream (is);
}

void Streamable :: WriteBinaryStringStream (char * StringBuf, WORD StrSize)
{
    ostringstream os;
    WriteBinaryStream (os);
    memcpy (StringBuf, os.rdbuf()->str().c_str(), StrSize);
}

void Streamable :: ReadTextStringStream (const char * StringBuf, WORD StrSize)
{
    istringstream is (string (StringBuf, StrSize));
    ReadTextStream (is);
}

void Streamable :: WriteTextStringStream (char * StringBuf, WORD StrSize)
{
    ostringstream os;
    WriteTextStream (os);
    memcpy (StringBuf, os.rdbuf()->str().c_str(), StrSize);
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

