/* sympic.cc
**
**	Data-type dependent functions.
*/

#include "sympic.h"
#include "semutil.h"

PictureType :: PictureType (void)
{
    Text = NULL;
    Size = 0;
    CSize = 0;
    Sign = PictureType::NoSign;
    SignSeparate = FALSE;
    nDigitsBDP = 0;
    nDigitsADP = 0;
    nFillerZeroes = 0;
}

// The purpose of this function is to convert X(3) to XXX
void PictureType :: Expand (char * APic, char * Expanded)
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

WORD PictureType :: Set (char * NewPicture)
{
char Expanded [500];
unsigned int i;
int j = 0;
int length;
unsigned int oldlength;
BOOL FoundPoint = FALSE;

    Expand (NewPicture, Expanded);

    for (i = 0; i < strlen (Expanded); i++)
       if (!IsInSet (Expanded[i], ".,/b0")) 
          j++;

    // length will be the number of substitutable spots length(XX.X.B909) = 5
    length = j;

    oldlength = strlen (Expanded); 
    Size = length;
    Kind = PictureType::Undefined;

    if (Text != NULL)
       delete [] Text;
    Text = new char [oldlength + 1];
    if (Text == NULL)
       WriteError ("Out of memory allocating picture text storage");

    strcpy (Text, Expanded);

    for (i = 0; i < oldlength; ++ i) {
       if (Expanded[i] == 'x')
	  Kind = PictureType::String;
       else if (Expanded[i] == 'a')
	  Kind = PictureType::String;
       else if (IsInSet (Expanded[i], "90z*")) {
	  if (Kind == PictureType::Undefined)
	     Kind = PictureType::Integer;

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
	  Kind = PictureType::Float;
	  FoundPoint = TRUE;
       }
    }

    switch (Kind) {
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

void PictureType :: GenTypePrefix (ofstream& os)
{
    if (Sign != PictureType::NoSign)
       os << "unsigned ";

    switch (Kind) {
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

void PictureType :: GenTypeSuffix (ofstream& os)
{
    // Only strings need an array suffix.
    if (Kind == PictureType::String)
       os << " [" << Size + 1 << "]";
}

void PictureType :: GenReadFunction (ofstream& os)
{
    switch (Kind) {
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

void PictureType :: GenWriteFunction (ofstream& os)
{
    switch (Kind) {
       case PictureType::String: 
			  os << "_WriteStringVar";
			  break;
       case PictureType::Integer: 
			  os << "_WriteIntegerVar";
			  break;
       case PictureType::Float: 
			  os << "_WriteFloatVar";
			  break;
       case PictureType::Undefined:	
			  WriteError ("Undefined picture type");
			  break;
       default:		  WriteError ("Invalid picture type");
			  break;
    }
}

// Returns TRUE if a cast is needed, so that (...) can be written
BOOL PictureType :: GenCastFunction (ofstream& os, PictureType& pic)
{
    if (Kind == pic.Kind)
       return (FALSE);

    if (IsNumeric() == pic.IsNumeric())
       return (FALSE);

    if (IsNumeric()) {
       if (Kind == PictureType::Integer)
	  os << "StringToInteger";
       else
	  os << "StringToFloat";
    }
    else {
       if (Kind == PictureType::Integer)
	  os << "IntegerToString";
       else
	  os << "FloatToString";
    }
    return (TRUE);
}

void PictureType :: Write (ostream& os)
{
    os << Text;
}

void PictureType :: Write (ofstream& os)
{
    os << Text;
}

PictureType :: ~PictureType (void)
{
    if (Text != NULL)
       delete [] Text;
}


