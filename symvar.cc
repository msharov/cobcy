/* symvar.cc
**
**	Variable class
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include "symvar.h"
#include "symconst.h"
#include "semcontrol.h"

/*-----------------------------------------------*/
  extern BOOL RoundResult;
/*-----------------------------------------------*/

CobolVar :: CobolVar (void)
{
}

void CobolVar :: SetPicture (char * NewPicture)
{
    CSize = Picture.Set (NewPicture);
}

CobolSymbolType CobolVar :: Kind (void)
{
    return (CS_Variable);
}

void CobolVar :: GenDeclare (ofstream& os)
{
    GenIndent();
    Picture.GenTypePrefix (os);
    os << " " << GetCName();
    Picture.GenTypeSuffix (os);
    os << ";\n";
}

void CobolVar :: GenRead (ofstream& os, char * stream)
{
#if DEBUG
    cout << "\t\tReading variable " << *this;
    cout << " from stream " << stream << "\n";
#endif
    GenIndent();
    Picture.GenReadFunction (os);
    os << " (" << stream << ", " << *this << ", ";
    os << "\"" << Picture << "\");\n";
}

void CobolVar :: GenWrite (ofstream& os, char * stream)
{
#if DEBUG
    cout << "\t\tWriting variable " << *this;
    cout << " to stream " << stream << "\n";
#endif
    GenIndent();
    Picture.GenWriteFunction (os);
    os << " (" << stream << ", " << *this << ", ";
    os << "\"" << Picture << "\");\n";
}

// Move for two variables
void CobolVar :: GenMove (ofstream& os, CobolVar * data)
{
    // Indent, as usual
    GenIndent();

    if (Picture.IsNumeric()) {
       os << *this << " = ";
       if (Picture.GenCastFunction (os, data->Picture))
	  os << " (" << *data << ")";
       else
	  os << *data;
       os << ";\n";
    }
    else if (!Picture.IsNumeric()) {
       os << "_AssignVarString (" << *this << ", ";
       if (Picture.GenCastFunction (os, data->Picture))
	  os << " (" << *data << ")";
       else
	  os << *data;
       os << ", " << Picture.GetSize();
       os << ", " << data->Picture.GetSize() << ");\n";
    }
    else
       WriteError ("Incompatible types for assignment");
}

void CobolVar :: GenMove (ofstream& os, CobolConstant& data)
{
    if (Picture.IsNumeric() && !data.IsNumeric()) {
       WriteError ("cannot cast constants (yet)");
       return;
    }

    GenIndent();
    if (Picture.IsNumeric())
       os << *this << " = " << data << ";\n";
    else {
       os << "_AssignVarString (" << *this << ", " << data;
       os << ", " << Picture.GetSize() << ", 0" << ");\n";
    }
}

// This should get both constants and variables
void CobolVar :: GenArith (ofstream& os, Streamable * op1, 
			   Streamable * op2, char opc)
{
    if (Picture.IsNumeric()) {
       // Generate a division by zero check
       if (opc == '/') {
	  GenIndent();
	  os << "if (" << *op2 << " != 0)\n";
	  ++ NestingLevel;
       }

       GenIndent();
       os << *this << " = ";

       // If result is to be rounded, call the function :)
       if (RoundResult)
	  os << "_RoundResult ((double) ";
       os << *op1 << " " << opc << " ";

       // And, of course, rounding is no good if the number inside is an int
       if (RoundResult)
	  os << "(double) ";
       os << *op2;
       if (RoundResult)
	  os << ", " << Picture << ")";
       os << ";\n";

       if (opc == '/')
	  -- NestingLevel;
    }
    else
       WriteError ("arithmetic is for numbers");
}

CobolVar :: ~CobolVar (void)
{
    // Keep it for now.
}

