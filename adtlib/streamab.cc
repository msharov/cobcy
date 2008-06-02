/* streamab.cc
**
**	Base class for file-active class.
*/

#include "streamab.h"

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

