/* symdata.cc
**
**	Defines an abstract data thing.
*/

#include "symdata.h"
#include "symfile.h"

CobolData :: CobolData (void)
{
    CSize = 0;
    DeclLevel = 0;
    AssociatedStream = NULL;
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

CobolData :: ~CobolData (void)
{
}

