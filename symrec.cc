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
    CSize += NewChild->GetSize();
}

CobolSymbolType CobolRecord :: Kind (void)
{
    return (CS_Record);
}

void CobolRecord :: GenDeclareBegin (ofstream& os)
{
    GenIndent();
    os << "struct {\n";
}

void CobolRecord :: GenDeclareEnd (ofstream& os)
{
    GenIndent();
    os << "} " << GetCName() << ";\n";
}

void CobolRecord :: GenRead (ofstream& os, char * stream)
{
unsigned int i;
CobolData * child;

#if DEBUG
    cout << "\tReading record " << *this;
    cout << " from stream " << stream << "\n";
#endif
    ChildList.Head();
    for (i = 0; i < nChildren; ++ i) {
       child = ChildList.LookAt();
#if DEBUG
       cout << "\t\tReading child " << *child << "\n";
#endif
       child->GenRead (os, stream);
       ChildList.Next();
    }
}

void CobolRecord :: GenWrite (ofstream& os, char * stream)
{
unsigned int i;
CobolData * child;

#if DEBUG
    cout << "\tWriting record " << *this;
    cout << " to stream " << stream << "\n";
#endif
    ChildList.Head();
    for (i = 0; i < nChildren; ++ i) {
       child = ChildList.LookAt();
#if DEBUG
       cout << "\t\tWriting child " << *child << "\n";
#endif
       child->GenWrite (os, stream);
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

