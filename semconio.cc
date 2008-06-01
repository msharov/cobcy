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
#include "symscreen.h"
#include "symconst.h"
#include "symfile.h"
#include "semcontrol.h"

/*---------------------| Globals |--------------------------------*/
  char 				DisplayOutput [MAX_SYMBOL_LENGTH];
  AcceptSourceType		AcceptSource;
/*----------------------------------------------------------------*/

void GenAccept (void)
{
#ifndef NDEBUG
    cout << "\tIn GenAccept\n";
#endif
    int nIds = CountIdentifiers();

    StackEntryPtr* destArray = new StackEntryPtr [nIds];
    int i;
    for (i = 0; i < nIds; ++ i)
       destArray[i] = SemStack.Pop();

    // The following checks for an AT x,y clause, bordered by marks
    StackEntryPtr entry = SemStack.Pop();
    int xloc = -1, yloc = -1;
    if (entry->kind != SE_Mark) {
       yloc = entry->ival;
       delete entry;
       entry = SemStack.Pop();
       xloc = entry->ival;
       delete entry;
       entry = SemStack.Pop();
       if (entry->kind != SE_Mark)
	  WriteError ("ACCEPT AT x,y has no second mark");
       SetCursesUsed (TRUE);
    }
    delete entry;

    if (CobcyConfig.UseCurses && xloc >= 0 && yloc >= 0) {
       GenIndent();
       codef << "move (" << xloc << ", " << yloc << ");\n";
    }

    for (i = 0; i < nIds; ++ i) {
       entry = destArray[i];
       if (entry->kind == SE_Identifier) {
	  CobolSymbol* attr = LookupIdentifier (entry->ident);
	  if (attr == NULL) {
	     delete entry;
	     continue;
	  }
	  if (attr->Kind() == CS_ScreenField && AcceptSource != AS_Console) {
	     WriteError ("Can only accept a SCREEN from CONSOLE");
	     delete entry;
	     continue;
	  }

	  switch (AcceptSource) {
	     case AS_Console:
	     	if (attr->Kind() == CS_ScreenField)
		   ((CobolScreenField*) attr)->GenRead (codef, "stdin");
		else
		   ((CobolData*) attr)->GenRead (codef, "stdin");
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
    delete [] destArray;
}

void SetAcceptSource (AcceptSourceType NewSrc)
{
    AcceptSource = NewSrc;
}

void GenDisplay (void)
{
#ifndef NDEBUG
    cout << "\tIn GenDisplay\n";
#endif
    char outputStream [MAX_SYMBOL_LENGTH];
    strcpy (outputStream, DisplayOutput);

    int nIds = CountIdentifiers();
    ReverseIdentifiers (nIds);
#ifndef NDEBUG
    cout << "\t\t" << nIds << " things on stack\n";
#endif
    StackEntryPtr* printables = new StackEntryPtr [nIds];
    int i;
    for (i = 0; i < nIds; ++ i)
       printables[i] = SemStack.Pop();

    // The following checks for an AT x,y clause, bordered by marks
    StackEntryPtr entry = SemStack.Pop();
    int xloc = -1, yloc = -1;
    if (entry->kind != SE_Mark) {
       yloc = entry->ival;
       delete entry;
       entry = SemStack.Pop();
       xloc = entry->ival;
       delete entry;
       entry = SemStack.Pop();
       if (entry->kind != SE_Mark)
	  WriteError ("DISPLAY AT x,y has no second mark");
       if (CobcyConfig.UseCurses)
          strcpy (outputStream, "NULL");
       SetCursesUsed (TRUE);
    }
    delete entry;

    if (CobcyConfig.UseCurses && xloc >= 0 && yloc >= 0) {
       GenIndent();
       codef << "move (" << xloc << ", " << yloc << ");\n";
    }

    BOOL hasScreens = FALSE;
    for (i = 0; i < nIds; ++ i) {
       entry = printables[i];

       if (entry->kind == SE_Identifier) {
	  CobolSymbol* attr = LookupIdentifier (entry->ident);
	  if (attr == NULL) {
	     delete entry;
	     continue;
	  }
	  if (attr->Kind() == CS_ScreenField) {
	     hasScreens = TRUE;
	     ((CobolScreenField*) attr)->GenWrite (codef, outputStream);
	  }
	  else
	     ((CobolData*) attr)->GenWrite (codef, outputStream);
       }
       else {
	  CobolConstant cattr = entry;
	  cattr.GenWrite (codef, outputStream);
       }
       delete entry;
    }
    delete [] printables;

    if (!hasScreens && !(CobcyConfig.UseCurses && xloc >= 0 && yloc >= 0)) {
       GenIndent();
       codef << "fprintf (" << outputStream << ", \"\\n\");\n";
    }
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

