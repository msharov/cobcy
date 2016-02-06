// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symvar.h"
#include "symconst.h"
#include "semcontrol.h"

//-----------------------------------------------

extern bool RoundResult;

//-----------------------------------------------

void CobolVar::GenDeclare (ostringstream& os) const
{
    GenIndent();
    Picture.GenTypePrefix (os);
    os << " " << GetCName();
    Picture.GenTypeSuffix (os);
    os << ";\n";
}

void CobolVar::GenRead (ostringstream& os, const char* stream) const
{
    #ifndef NDEBUG
	if (g_Config.GenDebug)
	    cout << "\t\tReading variable " << *this << " from stream " << stream << "\n";
    #endif
    GenIndent();
    Picture.GenReadFunction (os);
    os << " (" << stream << ", ";
    if (IsNumeric())
	os << '&';
    os << *this << ", \"" << Picture << "\");\n";
}

void CobolVar::GenWrite (ostringstream& os, const char* stream) const
{
    #ifndef NDEBUG
	if (g_Config.GenDebug)
	    cout << "\t\tWriting variable " << *this << " to stream " << stream << "\n";
    #endif
    GenIndent();
    Picture.GenWriteFunction (os);
    os << " (" << stream << ", " << *this << ", ";
    os << "\"" << Picture << "\");\n";
}

// Move for two variables
void CobolVar::GenMove (ostringstream& os, const CobolVar* data) const
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

void CobolVar::GenMove (ostringstream& os, const CobolConstant& data) const
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
void CobolVar::GenArith (ostringstream& os, const CobolSymbol* op1, const CobolSymbol* op2, char opc) const
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
	    os << ", \"" << Picture << "\")";
	os << ";\n";

	if (opc == '/')
	    -- NestingLevel;
    }
    else
	WriteError ("arithmetic is for numbers");
}

void CobolVar::GenSignature (ostringstream& os) const
{
    os << " " << GetCName();
    Picture.GenSignature (os);
}

void CobolVar::GenCharCast (ostringstream& os) const
{
    PictureType CharPic;

    CharPic.Set ("x(11)");	// Generates a character picture
    if (IsNumeric()) {
	if (Picture.GenCastTo (os, CharPic))
	    os << " (" << *this << ", \"" << Picture << "\")";
    } else
	os << *this;
}
