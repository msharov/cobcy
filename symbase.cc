/* symbase.cc
**
**	Functions for CobolSymbol
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include "symbase.h"

CobolSymbol :: CobolSymbol (void)
{
    memset (ParentCobolName, 0, MAX_SYMBOL_LENGTH);
    memset (CobolName, 0, MAX_SYMBOL_LENGTH);
    memset (CName, 0, MAX_SYMBOL_LENGTH);
    memset (Prefix, 0, MAX_PREFIX_LENGTH);
    memset (FullCName, 0, MAX_FULLNAME_LENGTH);
}

void CobolSymbol :: CobolToCName (char * str)
{
unsigned int i;
    for (i = 0; i < strlen(str); ++ i)
       if (str[i] == '-')
	  str[i] = '_';
}

void CobolSymbol :: SetName (char * NewName)
{
    strcpy (CobolName, NewName);
    strcpy (CName, NewName);
    CobolToCName (CName); 
    strcpy (FullCName, Prefix);
    strcat (FullCName, CName);
}

void CobolSymbol :: SetParent (char * NewParent)
{
CobolSymbol * pattr;
char ErrorBuffer [80];

    if (NewParent == NULL) {
       memset (ParentCobolName, MAX_SYMBOL_LENGTH, 0);
       memset (Prefix, MAX_PREFIX_LENGTH, 0);
       strcpy (FullCName, CName);
    }
    else {
       if ((pattr = SymTable.Lookup (NewParent)) == NULL) {
	  sprintf (ErrorBuffer, 
	  	   "cannot find parent %s; wanted for child support; reward", 
		   NewParent);
	  WriteError (ErrorBuffer);
	  return;
       }

       strcpy (Prefix, pattr->Prefix);
       strcat (Prefix, pattr->CName);
       strcat (Prefix, ".");

       strcpy (FullCName, Prefix);
       strcat (FullCName, CName);
    }
}

void CobolSymbol :: WriteBinaryStream (ostream& os)
{
    os << FullCName;
}

void CobolSymbol :: WriteTextStream (ostream& os)
{
    os << FullCName;
}

CobolSymbol :: ~CobolSymbol (void)
{
    // Keep it around just in case...
}

