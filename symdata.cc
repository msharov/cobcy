/* symdata.cc
**
**	Defines an abstract data thing.
*/

#include "symdata.h"
#include "symfile.h"
#include <string.h>

CobolData :: CobolData (void)
{
    Size = 0;
    DeclLevel = 0;
    AssociatedStream = NULL;
    ArraySize = 1;
    IndexVarName[0] = 0;
}

CobolData :: ~CobolData (void)
{
}

CobolFile * CobolData :: GetStream (void)
{
    return (AssociatedStream);
}

void CobolData :: AssociateWithStream (CobolFile * NewStream)
{
    AssociatedStream = NewStream;
}

void CobolData :: GenRead (ostream& os)
{
    GenRead (os, AssociatedStream->GetFullCName());
}

void CobolData :: GenWrite (ostream& os)
{
    GenWrite (os, AssociatedStream->GetFullCName());
}

void CobolData :: CreateFullCName (void)
{
    CobolSymbol::CreateFullCName();
    if (ArraySize > 1) {
	strcat (FullCName, "[");
	strcat (FullCName, IndexVarName);
	strcat (FullCName, "]");
    }
}

