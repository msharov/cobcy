// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "sympic.h"
#include "semutil.h"

PictureType::PictureType (void)
: Size (0)
, CSize (0)
,_kind (Undefined)
, Sign (NoSign)
, SignSeparate (false)
, nDigitsBDP (0)
, nDigitsADP (0)
, nFillerZeroes (0)
,_text()
{
}

// The purpose of this function is to convert X(3) to XXX
/*static*/ void PictureType::Expand (const char* pic, string& expanded)
{
    expanded.clear();
    for (char pc = 'x'; *pic; pc = *pic++) {
	if (*pic == '(') {	// Handle x(5) type things
	    char* epic = const_cast<char*>(++pic);
	    auto n = strtoul (epic, &epic, 10);
	    pic = epic;
	    expanded.append (n, pc);
	} else if (*pic != ')')
	    expanded += *pic;
    }
}

uint32_t PictureType::Set (const char* NewPicture)
{
    Expand (NewPicture, _text);
    Size = 0; // Size will be the number of substitutable spots length(XX.X.B909) = 5
    for (auto c : _text)
	if (!IsInSet (c, ".,/b0"))
	    ++Size;
    _kind = PictureType::Undefined;
    bool afterDP = false;
    for (auto c : _text) {
	if (c == 'x' || c == 'a')
	    _kind = String;
	else if (IsInSet (c, "90z*")) {
	    if (_kind == Undefined)
		_kind = Integer;
	    if (afterDP)
		++ nDigitsADP;
	    else
		++ nDigitsBDP;
	} else if (c == 's')
	    Sign = TrailingSign;
	else if (c == 'p')
	    ++ nFillerZeroes;
	else if (c == 'v') {
	    _kind = Float;
	    afterDP = true;
	}
    }
    switch (_kind) {
	case PictureType::String:	CSize = Size + 1; break;
	case PictureType::Integer:	CSize = sizeof(long int); break;
	case PictureType::Float:	CSize = sizeof(double); break;
	default: WriteError ("Cannot determine picture type"); break;
    }
    return CSize;
}

void PictureType::GenTypePrefix (ostringstream& os) const
{
    if (Sign != NoSign)
	os << "unsigned ";
    switch (_kind) {
	case String:	os << "char"; break;
	case Integer:	os << "long"; break;
	case Float:	os << "double"; break;
	default:	WriteError ("bad picture type for declaration"); break;
    }
}

void PictureType::GenTypeSuffix (ostringstream& os) const
{
    // Only strings need an array suffix.
    if (_kind == PictureType::String)
	os << " [" << CSize << "]";
}

void PictureType::GenReadFunction (ostringstream& os) const
{
    switch (_kind) {
	case String:	os << "_ReadStringVar"; break;
	case Integer:	os << "_ReadIntegerVar"; break;
	case Float:	os << "_ReadFloatVar"; break;
	default:	WriteError ("Invalid picture type"); break;
    }
}

void PictureType::GenWriteFunction (ostringstream& os) const
{
    switch (_kind) {
	case String:	os << "_WriteStringVar"; break;
	case Integer:	os << "_WriteIntegerVar"; break;
	case Float:	os << "_WriteFloatVar"; break;
	default:	WriteError ("Invalid picture type"); break;
    }
}

// Returns true if a cast is needed, so that (...) can be written
bool PictureType::GenCastFrom (ostringstream& os, const PictureType& pic) const
{
    if (_kind == pic._kind)
	return false;
    if (IsNumeric() == pic.IsNumeric())
	return false;
    if (pic.IsNumeric()) {
	if (pic._kind == PictureType::Integer)
	    os << "_StringToInteger";
	else
	    os << "_StringToFloat";
    } else {
	if (pic._kind == PictureType::Integer)
	    os << "_IntegerToString";
	else
	    os << "_FloatToString";
    }
    return true;
}

// Returns true if a cast is needed, so that (...) can be written
bool PictureType::GenCastTo (ostringstream& os, const PictureType& pic) const
{
    if (_kind == pic._kind)
	return false;
    if (IsNumeric() == pic.IsNumeric())
	return false;
    if (IsNumeric()) {
	if (_kind == PictureType::Integer)
	    os << "_IntegerToString";
	else
	    os << "_FloatToString";
    } else {
	if (_kind == PictureType::Integer)
	    os << "_StringToInteger";
	else
	    os << "_StringToFloat";
    }
    return true;
}

// Writes type character, size of field, and number of digits after d.p.
//	Used for creation of DBF files.
void PictureType::GenSignature (ostringstream& os) const
{
    os << " ";
    if (IsNumeric())
	os << "N ";
    else
	os << "C ";
    // Fields are written in the file with all editing characters, so
    //	the size of the field in a file is the length of the whole picture.
    os << _text.size() << " " << nDigitsADP;
}

void PictureType::text_write (ostringstream& os) const
{
    os << _text;
}
