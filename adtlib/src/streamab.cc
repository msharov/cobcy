/* streamab.cc
**
**	Base class for file-active class.
*/

#include <streamab.h>
#include <endian.h>

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

void Streamable :: ReadBinaryStream (istream& is)
{
    if (is.eof())
	cerr << "ERROR: Default ReadBinaryStream activated at end of file!";
    else
	cerr << "WARNING: Default ReadBinaryStream activated!";
}

void Streamable :: WriteBinaryStream (ostream& os)
{
    if (os.eof())
	cerr << "ERROR: Default WriteBinaryStream activated at end of file!";
    else
	cerr << "WARNING: Default WriteBinaryStream activated!";
}

void Streamable :: ReadTextStream (istream& is)
{
    if (is.eof())
	cerr << "ERROR: Default ReadTextStream activated at end of file!";
    else
	cerr << "WARNING: Default ReadTextStream activated!";
}

void Streamable :: WriteTextStream (ostream& os)
{
    if (os.eof())
	cerr << "ERROR: Default WriteTextStream activated at end of file!";
    else
	cerr << "WARNING: Default WriteTextStream activated!";
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

