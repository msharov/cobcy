/* symlabel.cc
**
**	Defines a label symbol
*/

#include "symlabel.h"
#include "semutil.h"
#include "semcontrol.h"

CobolLabel :: CobolLabel (void)
{
    Undeclared = true;
}

void CobolLabel :: GenDeclare (ostream& os)
{
    os << "int " << *this << " (void)\n";
    BeginCompound();
    os << *this << ":\n";
}

void CobolLabel :: GenJump (ostream& os, CobolLabel * dest)
{
    GenIndent();
    // If in jumping to the top of the current paragraph, use goto.
    if (dest == this) {
       os << "goto " << *this << ";\n";
    }
    else {
       // Return the difference of indeces of the destination paragraph
       //	and the current paragraph. This is done so that when
       //	that paragraph is through, and we return here, we could
       //	tell the main routine how many paragraphs to advance.
       os << "return (";
       os << "_pi_" << *dest << " - ";
       os << "_pi_" << *this << ");\n";
    }
}

CobolLabel :: ~CobolLabel (void)
{
}

