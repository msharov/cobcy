/* semconio.h
**
**	Console IO for the COBOL compiler.
*/

#ifdef __MSDOS__
#include "semexter.h"
#else
#include "semextern.h"
#endif
#include "semconio.h"
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif
#include "symdata.h"
#include "symconst.h"
#include "symfile.h"
#include "semcontrol.h"

/*---------------------| Globals |--------------------------------*/
  char 				DisplayOutput [MAX_SYMBOL_LENGTH];
  AcceptSourceType		AcceptSource;
/*----------------------------------------------------------------*/

void GenAccept (void)
{
StackEntry * entry;
CobolData * attr;
int i, nIds;

#ifndef NDEBUG
    cout << "\tIn GenAccept\n";
#endif

    nIds = CountIdentifiers();

    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();
       if (entry->kind == SE_Identifier) {
	  if ((attr = (CobolData*) LookupIdentifier (entry->ident)) == NULL)
	     return;

	  switch (AcceptSource) {
	     case AS_Console:
	        attr->GenRead (codef, "stdin");
		GenEmptyClause();
		break;
	     case AS_Date:
	        GenIndent();
	        attr->WriteTextStream (codef);
		codef << " = ";
		codef << "_GetDate();\n";
		break;
	     case AS_Day:
	        GenIndent();
	        attr->WriteTextStream (codef);
		codef << " = ";
		codef << "_GetDay();\n";
		break;
	     case AS_Weekday:
	        NIY ("Weekdays");
	     	break;
	     case AS_Time:
	        GenIndent();
	        attr->WriteTextStream (codef);
		codef << " = ";
		codef << "_GetTime();\n";
		break;
	  }
       }
       delete entry;
    }
}

void SetAcceptSource (AcceptSourceType NewSrc)
{
    AcceptSource = NewSrc;
}

void GenDisplay (void)
{
StackEntry * entry;
CobolData * attr;
CobolConstant cattr;
int i, nIds;

#ifndef NDEBUG
    cout << "\tIn GenDisplay\n";
#endif

    nIds = CountIdentifiers();
    ReverseIdentifiers (nIds);

#ifndef NDEBUG
    cout << "\t\t" << nIds << " things on stack\n";
#endif

    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();

       if (entry->kind == SE_Identifier) {
	  if ((attr = (CobolData*) LookupIdentifier (entry->ident)) == NULL)
	     return;
          attr->GenWrite (codef, DisplayOutput);   
       }
       else {
	  cattr = entry;
	  cattr.GenWrite (codef, DisplayOutput);
       }
    }

    GenIndent();
    codef << "fprintf (" << DisplayOutput << ", \"\\n\");\n";
}

void SetDisplayOutput (void)
{
StackEntry * OutputStream;
CobolFile * OutStrSym;
    
    OutputStream = SemStack.Pop();
    OutStrSym = (CobolFile*) LookupIdentifier (OutputStream->ident);
    if (OutStrSym != NULL)
       strcpy (DisplayOutput, OutStrSym->GetFullCName());

#ifndef NDEBUG
    cout << "\tDISPLAY output set to " << DisplayOutput << "\n";
#endif

    delete OutputStream;
}

