/* symbol.cc
**
**	Constructor and destructor for CobolSymbol
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif

CobolSymbol :: CobolSymbol (void)
{
    Picture.Text = NULL;
    Picture.Size = 0;
    Picture.CSize = 0;
    Picture.Sign = PictureType::NoSign;
    Picture.SignSeparate = FALSE;
    Picture.nDigitsBDP = 0;
    Picture.nDigitsADP = 0;
    Picture.nFillerZeroes = 0;

    nChildren = 0;
    ParentCobolName[0] = '\x0';
    RecordCSize = 0;
    CobolName[0] = '\x0';
    CName[0] = '\x0';
    Prefix[0] = '\x0';
    FileName[0] = '\x0';
    FileRecordName[0] = '\x0';
    DeclLevel = 0;
    Kind = Undefined;
    Undeclared = FALSE;
}

void CobolSymbol :: CobolToCName (char * str)
{
unsigned int i;
    for (i = 0; i < strlen(str); ++ i)
       if (str[i] == '-')
	  str[i] = '_';
}

// The purpose of this function is to convert X(3) to XXX
void CobolSymbol :: ExpandPicture (char * APic, char * Expanded)
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

void CobolSymbol :: SetPicture (char * NewPic)
{
char Expanded [500];
unsigned int i;
int j = 0;
int length;
unsigned int oldlength;
BOOL FoundPoint = FALSE;

    ExpandPicture (NewPic, Expanded);

    for (i = 0; i < strlen (Expanded); i++)
       if (!IsInSet (Expanded[i], ".,/b0")) 
          j++;

    // length will be the number of substitutable spots length(XX.X.B909) = 5
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
       else if (IsInSet (Expanded[i], "90z*")) {
	  if (Picture.Kind == PictureType::Undefined)
	     Picture.Kind = PictureType::Integer;

	  if (FoundPoint)
	     ++ Picture.nDigitsADP;
	  else
	     ++ Picture.nDigitsBDP;
       }
       else if (Expanded[i] == 's')
	  Picture.Sign = PictureType::TrailingSign;
       else if (Expanded[i] == 'p')
	  ++ Picture.nFillerZeroes;
       else if (Expanded[i] == 'v') {
	  Picture.Kind = PictureType::Float;
	  FoundPoint = TRUE;
       }
    }

    switch (Picture.Kind) {
	case PictureType::String:	
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

void CobolSymbol :: SetParent (char * NewParent)
{
CobolSymbol * pattr;
char ErrorBuffer [80];

    cerr << "Setting parent of " << CobolName << " to " << NewParent << "\n";
    if (NewParent == NULL) {
       memset (ParentCobolName, MAX_SYMBOL_LENGTH, 0);
       memset (Prefix, MAX_PREFIX_LENGTH, 0);
    }
    else {
       if ((pattr = SymTable.Lookup (NewParent)) == NULL) {
	  sprintf (ErrorBuffer, 
	  	   "cannot find parent %s; wanted for child support; reward", 
		   NewParent);
	  WriteError (ErrorBuffer);
	  return;
       }

       memset (Prefix, 0, MAX_PREFIX_LENGTH);
       strcat (Prefix, pattr->Prefix);
       strcat (Prefix, pattr->CName);
       strcat (Prefix, ".");
    }
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

