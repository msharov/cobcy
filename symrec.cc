/* symrec.cc
**
**	Functions for CobolRecord
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include "symrec.h"

CobolRecord :: CobolRecord (void)
{
    nChildren = 0;
}

void CobolRecord :: AddChild (CobolData * NewChild)
{
    ++ nChildren;
    ChildList.Tail();
    ChildList.InsertAfter (NewChild);
    Size += NewChild->GetSize();
}

CobolSymbolType CobolRecord :: Kind (void)
{
    return (CS_Record);
}

void CobolRecord :: GenDeclareBegin (ostream& os)
{
    GenIndent();
    os << "struct {\n";
}

void CobolRecord :: GenDeclareEnd (ostream& os)
{
    GenIndent();
    os << "} " << GetCName();
    if (ArraySize > 1)
	os << " [" << ArraySize << "]";
    os << ";\n";
}

void CobolRecord :: GenRead (ostream& os, const char * stream)
{
unsigned int i;
CobolData * child;

#ifndef NDEBUG
    cout << "\tReading record " << *this;
    cout << " from stream " << stream << "\n";
#endif
    ChildList.Head();
    for (i = 0; i < nChildren; ++ i) {
       child = ChildList.LookAt();
#ifndef NDEBUG
       cout << "\t\tReading child " << *child << "\n";
#endif
       child->GenRead (os, stream);
       ChildList.Next();
    }
}

void CobolRecord :: GenWrite (ostream& os, const char * stream)
{
unsigned int i;
CobolData * child;

#ifndef NDEBUG
    cout << "\tWriting record " << *this;
    cout << " to stream " << stream << "\n";
#endif
    ChildList.Head();
    for (i = 0; i < nChildren; ++ i) {
       child = ChildList.LookAt();
#ifndef NDEBUG
       cout << "\t\tWriting child " << *child << "\n";
#endif
       child->GenWrite (os, stream);
       ChildList.Next();
    }
}

void CobolRecord :: GenSignature (ostream& os)
{
unsigned int i;
CobolData * child;

#ifndef NDEBUG
    cout << "\tSigning record " << *this << "\n";
#endif
    ChildList.Head();
    for (i = 0; i < nChildren; ++ i) {
       child = ChildList.LookAt();
#ifndef NDEBUG
       cout << "\t\tSigning child " << *child << "\n";
#endif
       child->GenSignature (os);
       ChildList.Next();
    }
}

CobolRecord :: ~CobolRecord (void)
{
    while (!ChildList.IsEmpty()) {
        ChildList.Head();
        ChildList.Remove();
	///////////////////////////////////////////////////////////////////
	// This is important, because ChildList points to existing symbols
	//	in the table and should not be deleted as they would have
	//	been if ~LList was used. All symbols are deleted only by
	//	the HashTable destructor.
	///////////////////////////////////////////////////////////////////
    }
}

