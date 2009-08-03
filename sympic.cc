// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "sympic.h"
#include "semutil.h"

PictureType::PictureType (void)
{
    Text = NULL;
    Size = 0;
    CSize = 0;
    Sign = PictureType::NoSign;
    SignSeparate = false;
    nDigitsBDP = 0;
    nDigitsADP = 0;
    nFillerZeroes = 0;
}

// The purpose of this function is to convert X(3) to XXX
void PictureType::Expand (const char* APic, char * Expanded)
{
    unsigned int i, j, k;
    char buffer[5];
    char filler;

    j = 0;
    strcpy (buffer, "    ");

    for (i = 0; i < strlen (APic); ++ i) {
	if (APic[i] == '(') {	// Handle x(5) type things
	    filler = APic[i - 1];
	    k = 0;
	    ++ i;
	    while (APic[i] != ')' && i < strlen (APic)) {
		buffer[k] = APic[i];
		++ k;
		++ i;
	    }
	    k = j + atoi(buffer) - 1;	// Fill j to j + atoi(buffer)
	    //	-1 because filler char counts

	    for (; j < k; ++ j)		//	with filler,
		Expanded [j] = filler;	//	incrementing j
	}
	else {
	    Expanded[j] = APic[i];
	    ++ j;
	}
    }
    Expanded[j] = '\x0';
}

uint32_t PictureType::Set (const char* NewPicture)
{
    char Expanded [500];
    unsigned int i;
    int j = 0;
    int length;
    unsigned int oldlength;
    bool FoundPoint = false;

    Expand (NewPicture, Expanded);

    for (i = 0; i < strlen (Expanded); ++ i)
	if (!IsInSet (Expanded[i], ".,/b0"))
	    j++;

    // length will be the number of substitutable spots length(XX.X.B909) = 5
    length = j;

    oldlength = strlen (Expanded);
    Size = length;
    m_Kind = PictureType::Undefined;

    if (Text != NULL)
	delete [] Text;
    Text = new char [oldlength + 1];
    if (Text == NULL)
	WriteError ("Out of memory allocating picture text storage");

    strcpy (Text, Expanded);

    for (i = 0; i < oldlength; ++ i) {
	if (Expanded[i] == 'x')
	    m_Kind = PictureType::String;
	else if (Expanded[i] == 'a')
	    m_Kind = PictureType::String;
	else if (IsInSet (Expanded[i], "90z*")) {
	    if (m_Kind == PictureType::Undefined)
		m_Kind = PictureType::Integer;

	    if (FoundPoint)
		++ nDigitsADP;
	    else
		++ nDigitsBDP;
	}
	else if (Expanded[i] == 's')
	    Sign = PictureType::TrailingSign;
	else if (Expanded[i] == 'p')
	    ++ nFillerZeroes;
	else if (Expanded[i] == 'v') {
	    m_Kind = PictureType::Float;
	    FoundPoint = true;
	}
    }

    switch (m_Kind) {
	case PictureType::String:
	    CSize = length + 1;
	    break;
	case PictureType::Integer:
	    CSize = sizeof(long int);
	    break;
	case PictureType::Float:
	    CSize = sizeof(double);
	    break;
	default:
	    WriteError ("Cannot determine picture type");
    }

    return (CSize);
}

void PictureType::GenTypePrefix (ostringstream& os)
{
    if (Sign != PictureType::NoSign)
	os << "unsigned ";

    switch (m_Kind) {
	case PictureType::String:
	    os << "char";
	    break;
	case PictureType::Integer:
	    os << "long int";
	    break;
	case PictureType::Float:
	    os << "double";
	    break;
	default:
	    WriteError ("bad picture type for declaration");
	    break;
    }
}

void PictureType::GenTypeSuffix (ostringstream& os)
{
    // Only strings need an array suffix.
    if (m_Kind == PictureType::String)
	os << " [" << Size + 1 << "]";
}

void PictureType::GenReadFunction (ostringstream& os)
{
    switch (m_Kind) {
	case PictureType::String:
	    os << "_ReadStringVar";
	    break;
	case PictureType::Integer:
	    os << "_ReadIntegerVar";
	    break;
	case PictureType::Float:
	    os << "_ReadFloatVar";
	    break;
	case PictureType::Undefined:
	    WriteError ("Undefined picture type");
	    break;
	default:		  WriteError ("Invalid picture type");
				  break;
    }
}

void PictureType::GenWriteFunction (ostringstream& os)
{
    switch (m_Kind) {
	case PictureType::String:	os << "_WriteStringVar"; break;
	case PictureType::Integer:	os << "_WriteIntegerVar"; break;
	case PictureType::Float:	os << "_WriteFloatVar"; break;
	case PictureType::Undefined:	WriteError ("Undefined picture type"); break;
	default:			WriteError ("Invalid picture type"); break;
    }
}

// Returns true if a cast is needed, so that (...) can be written
bool PictureType::GenCastFrom (ostringstream& os, PictureType& pic)
{
    if (m_Kind == pic.m_Kind)
	return (false);

    if (IsNumeric() == pic.IsNumeric())
	return (false);

    if (pic.IsNumeric()) {
	if (pic.m_Kind == PictureType::Integer)
	    os << "_StringToInteger";
	else
	    os << "_StringToFloat";
    }
    else {
	if (pic.m_Kind == PictureType::Integer)
	    os << "_IntegerToString";
	else
	    os << "_FloatToString";
    }
    return (true);
}

// Returns true if a cast is needed, so that (...) can be written
bool PictureType::GenCastTo (ostringstream& os, PictureType& pic)
{
    if (m_Kind == pic.m_Kind)
	return (false);

    if (IsNumeric() == pic.IsNumeric())
	return (false);

    if (IsNumeric()) {
	if (m_Kind == PictureType::Integer)
	    os << "_IntegerToString";
	else
	    os << "_FloatToString";
    }
    else {
	if (m_Kind == PictureType::Integer)
	    os << "_StringToInteger";
	else
	    os << "_StringToFloat";
    }
    return (true);
}

// Writes type character, size of field, and number of digits after d.p.
//	Used for creation of DBF files.
void PictureType::GenSignature (ostringstream& os)
{
    os << " ";
    if (IsNumeric())
	os << "N ";
    else
	os << "C ";

    // Fields are written in the file with all editing characters, so
    //	the size of the field in a file is the length of the whole picture.
    os << strlen(Text) << " " << nDigitsADP;
}

void PictureType::text_write (ostringstream& os) const
{
    os << Text;
}

PictureType::~PictureType (void)
{
    if (Text != NULL)
	delete [] Text;
}
