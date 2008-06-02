/* symvar.cc
**
**	Variable class
*/

#include "semextern.h"
#include "symvar.h"
#include "symconst.h"
#include "semcontrol.h"

/*-----------------------------------------------*/
  extern BOOL RoundResult;
/*-----------------------------------------------*/

CobolVar :: CobolVar (void)
{
}

void CobolVar :: SetPicture (const char* NewPicture)
{
    CSize = Picture.Set (NewPicture);
}

CobolSymbolType CobolVar :: Kind (void)
{
    return (CS_Variable);
}

void CobolVar :: GenDeclare (ostream& os)
{
    GenIndent();
    Picture.GenTypePrefix (os);
    os << " " << GetCName();
    Picture.GenTypeSuffix (os);
    os << ";\n";
}

void CobolVar :: GenRead (ostream& os, const char* stream)
{
#ifndef NDEBUG
    cout << "\t\tReading variable " << *this;
    cout << " from stream " << stream << "\n";
#endif
    GenIndent();
    Picture.GenReadFunction (os);
    os << " (" << stream << ", &" << *this << ", ";
    os << "\"" << Picture << "\");\n";
}

void CobolVar :: GenWrite (ostream& os, const char* stream)
{
#ifndef NDEBUG
    cout << "\t\tWriting variable " << *this;
    cout << " to stream " << stream << "\n";
#endif
    GenIndent();
    Picture.GenWriteFunction (os);
    os << " (" << stream << ", " << *this << ", ";
    os << "\"" << Picture << "\");\n";
}

// Move for two variables
void CobolVar :: GenMove (ostream& os, CobolVar * data)
{
    // Indent, as usual
    GenIndent();

    if (Picture.IsNumeric()) {
       os << *this << " = ";
       if (Picture.GenCastFrom (os, data->Picture))
	  os << " (" << *data << ")";
       else
	  os << *data;
       os << ";\n";
    }
    else if (!Picture.IsNumeric()) {
       os << "_AssignVarString (" << *this << ", ";
       if (Picture.GenCastFrom (os, data->Picture))
	  os << " (" << *data << ")";
       else
	  os << *data;
       os << ", " << Picture.GetSize();
       os << ", " << data->Picture.GetSize() << ");\n";
    }
    else
       WriteError ("Incompatible types for assignment");
}

void CobolVar :: GenMove (ostream& os, CobolConstant& data)
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
void CobolVar :: GenArith (ostream& os, Streamable * op1, 
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

void CobolVar :: GenSignature (ostream& os)
{
    os << " " << GetCName();
    Picture.GenSignature (os);
}

void CobolVar :: GenCharCast (ostream& os)
{
PictureType CharPic;

    CharPic.Set ("x(11)");	/* Generates a character picture */
    if (IsNumeric()) {
       	if (Picture.GenCastTo (os, CharPic))
	    os << " (" << *this << ", \"" << Picture << "\")";
    }
    else
	os << *this;
}

CobolVar :: ~CobolVar (void)
{
    // Keep it for now.
}

