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

/*---------------------| Globals |------------------------------*/
  char 				DisplayOutput [80];
  AcceptSourceType		AcceptSource;
/*--------------------------------------------------------------*/

void GenAccept (void)
{
StackEntry * entry;
CobolSymbol * attr;
char ErrorBuffer [80];
int i, nIds;

    nIds = CountIdentifiers();

    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();
       if (entry->kind == SE_Identifier) {
	  attr = SymTable.Lookup (entry->ident);
	  if (attr == NULL) {
	     sprintf (ErrorBuffer, "Undeclared identifier %s in ACCEPT", 
	     		entry->ident);
	     WriteError (ErrorBuffer);
	     return;
	  }

	  if (attr->Kind == CobolSymbol::Record && AcceptSource != AS_Console) {
	     WriteError ("cannot accept time variables into records");
	     return;
	  }

	  switch (AcceptSource) {
	     case AS_Console:
		if (attr->Kind == CobolSymbol::Record)
		   ReadRecord (attr, codef, "stdin", FALSE);
		else
		   ReadVariable (attr, codef, "stdin", FALSE);
		break;
	     case AS_Date:
	        GenIndent();
	        codef << attr->Prefix << attr->CName;
		codef << " = ";
		codef << "_GetDate();\n";
		break;
	     case AS_Day:
	        GenIndent();
	        codef << attr->Prefix << attr->CName;
		codef << " = ";
		codef << "_GetDay();\n";
		break;
	     case AS_Weekday:
	        NIY ("Weekdays");
	     	break;
	     case AS_Time:
	        GenIndent();
	        codef << attr->Prefix << attr->CName;
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
CobolSymbol * attr;
char ErrorBuffer [80];
int i, nIds;

    nIds = CountIdentifiers();
    ReverseIdentifiers (nIds);

    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();

       if (entry->kind == SE_Integer) {
          GenIndent();
	  codef << "fprintf (" << DisplayOutput;
	  codef << ", \"" << entry->ival << "\");\n";
       }
       else if (entry->kind == SE_Float) {
          GenIndent();
	  codef << "fprintf (" << DisplayOutput;
	  codef << ", \"" << entry->fval << "\");\n";
       }
       else if (entry->kind == SE_Quote) {
          GenIndent();
          codef << "fprintf (" << DisplayOutput;
          codef << ", \"\\\"\");\n";
       }
       else if (entry->kind == SE_String) {
          GenIndent();
	  codef << "fprintf (" << DisplayOutput;
	  codef << ", \"" << entry->ident << "\");\n";
       }
       else if (entry->kind == SE_Identifier) {
	  attr = SymTable.Lookup (entry->ident);
	  if (attr == NULL) {
	     sprintf (ErrorBuffer, "Undeclared identifier %s in DISPLAY", 
	     			entry->ident);
	     WriteError (ErrorBuffer);
	     return;
	  }

          
	  if (attr->Kind == CobolSymbol::Record)
	     PrintRecord (attr, codef, DisplayOutput, FALSE);
	  else
	     PrintVariable (attr, codef, DisplayOutput, FALSE);
       }
       delete entry;
    }

    GenIndent();
    codef << "fprintf (" << DisplayOutput << ", \"\\n\");\n";
}

void SetDisplayOutput (void)
{
StackEntry * OutputStream;
CobolSymbol * OutStrSym;
    
    OutputStream = SemStack.Pop();
    OutStrSym = LookupIdentifier (OutputStream->ident);
    if (OutStrSym != NULL)
       strcpy (DisplayOutput, OutStrSym->CName);
    delete OutputStream;
}

