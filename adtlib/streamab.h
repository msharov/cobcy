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

#include <iostream>
#include <fstream>
#include <sstream>
#include "mdefs.h"
using namespace std;

class Streamable {
public:
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

istream& operator>> (istream& is, Streamable& obj);
ostream& operator<< (ostream& is, Streamable& obj);

#endif

