/* streamable.h
**
**	Defines a base class for classes that use iostream.
** IO to file and std[in,out]. Overloaded << and >> for the whole family.
**
** Warning: do not include <unistd,h>, <stdio.h> or any other C output headers.
**		They may contain conflicts with <iostream.h>
**
** includes:
**	iostream.h	- for <<, >> and streams
**	fstream.h	- for ifstream and ofstream
*/

#ifndef __STREAMABLE_H
#define __STREAMABLE_H		 

#include <iostream.h>
#include <fstream.h>
#ifndef __MSDOS__
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include <mdefs.h>
#include <csocket.h>

class Streamable {
public:
    virtual void	Load (char * filename);
    virtual void	Save (char * filename);
    virtual WORD	StreamSize (void) const;
    virtual void 	ReadBinaryStream (istream& is);
    virtual void 	ReadTextStream (istream& is);
    virtual void 	WriteBinaryStream (ostream& os);
    virtual void 	WriteTextStream (ostream& os);

    void	ReadBinaryStringStream (const char * StreamBuf, WORD StrSize);
    void 	ReadTextStringStream (const char * StreamBuf, WORD StrSize);
    void 	WriteBinaryStringStream (char * StreamBuf, WORD StrSize);
    void 	WriteTextStringStream (char * StreamBuf, WORD StrSize);
};

/*------------------------------------------------------------------------*/

void	ReadByte (istream& is, BYTE * buffer);
void	ReadWord (istream& is, WORD * buffer);
void 	ReadString (istream& is, char * buffer, WORD s);
void	ReadRaw (istream& is, void * buffer, WORD s);
void	WriteByte (ostream& os, const BYTE * buffer);
void	WriteWord (ostream& os, const WORD * buffer);
void 	WriteString (ostream& os, const char * buffer, WORD s = 0);
void	WriteRaw (ostream& os, const void * buffer, WORD s);

/*------------------------------------------------------------------------*/

istream& operator>> (istream& is, Streamable& obj);
ostream& operator<< (ostream& is, Streamable& obj);

#endif

