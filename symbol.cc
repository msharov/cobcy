/* symbol.cc
**
**	Constructor and destructor for CobolSymbol
*/

#include "semextern.h"

CobolSymbol :: CobolSymbol (void)
{
    nChildren = 0;
    Picture.Text = NULL;
    Picture.CSize = 0;
    ParentCName[0] = '\x0';
    RecordCSize = 0;
    CobolName[0] = '\x0';
    CName[0] = '\x0';
    FileName[0] = '\x0';
    FileRecordName[0] = '\x0';
    DeclLevel = 0;
    Kind = Undefined;
    Undeclared = FALSE;
}

void CobolSymbol :: CobolToCName (char * str)
{
int i;
    for (i = 0; i < strlen(str); ++ i)
       if (str[i] == '-')
	  str[i] = '_';
}

// The purpose of this function is to convert X(3) to XXX
void CobolSymbol :: ExpandPicture (char * APic, char * Expanded)
{
int i, j, k;
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

void CobolSymbol :: SetPicture (char * NewPic)
{
char Expanded [500];
int i;
int j = 0;
int length;
int oldlength;

    ExpandPicture (NewPic, Expanded);

    for (i=0; i < strlen (Expanded); i++)
if (Expanded[i] != '.' && Expanded[i] != ',' && Expanded[i] != '/' && Expanded[i] != 'b' && Expanded[i] != '0') 
       j++;

    length = j;

    oldlength = strlen (Expanded); 
    Picture.Size = length;
    Picture.Kind = PictureType::Undefined;

    if (Picture.Text != NULL)
       delete [] Picture.Text;
    Picture.Text = new char [oldlength + 1];
    if (Picture.Text == NULL)
       WriteError ("Out of memory allocating picture text storage");

    strcpy (Picture.Text, Expanded);

    for (i = 0; i < oldlength; ++ i) {
       if (Expanded[i] == 'x')
	  Picture.Kind = PictureType::String;
       else if (Expanded[i] == 'a')
	  Picture.Kind = PictureType::String;
       else if (Expanded[i] == '9') {
	  if (Picture.Kind == PictureType::Undefined)
	   		   Picture.Kind = PictureType::Integer;
       }
       else if (Expanded[i] == 's')
	  Picture.Sign = TRUE;
       else if (Expanded[i] == 'p')
	  ++ Picture.nFillerZeroes;
       else if (Expanded[i] == 'v') {
	  Picture.Kind = PictureType::Float;
	  // nDigitsBDP is i unless a sign was present
	  Picture.nDigitsBDP = i - (Picture.Sign ? 1 : 0); 
	  // Subtract DBDP from total and subtract this V
	  Picture.nDigitsADP = oldlength - 
			       Picture.nDigitsBDP - 1;
       }
    }

    switch (Picture.Kind) {
	case PictureType::String:	
/*		Picture.CSize = strlen (Expanded) + 1; */
		Picture.CSize = length + 1;
		break; 
	case PictureType::Integer:	
		Picture.CSize = sizeof(long int);
		break; 
	case PictureType::Float:	
		Picture.CSize = sizeof(double);
		break; 
	default:
       		WriteError ("Cannot determine picture type");
    }
}

void CobolSymbol :: SetName (char * NewName)
{
    strcpy (CobolName, NewName);
    strcpy (CName, NewName);
    CobolToCName (CName); 
}

void CobolSymbol :: AddChild (CobolSymbol * NewChild)
{
    ++ nChildren;
    ChildList.Tail();
    ChildList.InsertAfter (NewChild);

    if (NewChild->Kind == CobolSymbol::Record)
       RecordCSize += NewChild->RecordCSize;
    else
       RecordCSize += NewChild->Picture.CSize;
}

CobolSymbol :: ~CobolSymbol (void)
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
    if (Picture.Text != NULL)
       delete [] Picture.Text;
}

