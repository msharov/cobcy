/* semconio.h
**
**	Console IO for the COBOL compiler.
*/

#include "semextern.h"
#include "semconio.h"
#ifdef CAN_HAVE_STDIO
#include <stdio.h>
#endif

/*---------------------| Globals |------------------------------*/
  char 				DisplayOutput [80];
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

	  if (attr->Kind == CobolSymbol::Record)
	     ReadRecord (attr, outfile, "stdin", FALSE);
	  else
	     ReadVariable (attr, outfile, "stdin", FALSE);
       }
       delete entry;
    }
}

void GenDisplay (void)
{
StackEntry * entry;
CobolSymbol * attr;
char prefix [80];
char ErrorBuffer [80];
int i, nIds;

    nIds = CountIdentifiers();
    ReverseIdentifiers (nIds);

    for (i = 0; i < nIds; ++ i) {
       entry = SemStack.Pop();

       if (entry->kind == SE_Integer) {
          GenIndent();
	  outfile << "fprintf (" << DisplayOutput;
	  outfile << ", \"" << entry->ival << "\");\n";
       }
       else if (entry->kind == SE_Float) {
          GenIndent();
	  outfile << "fprintf (" << DisplayOutput;
	  outfile << ", \"" << entry->fval << "\");\n";
       }
       else if (entry->kind == SE_String) {
          GenIndent();
	  outfile << "fprintf (" << DisplayOutput;
	  outfile << ", \"" << entry->ident << "\");\n";
       }
       else if (entry->kind == SE_Identifier) {
	  attr = SymTable.Lookup (entry->ident);
	  if (attr == NULL) {
	     sprintf (ErrorBuffer, "Undeclared identifier %s in DISPLAY", 
	     			entry->ident);
	     WriteError (ErrorBuffer);
	     return;
	  }

          BuildPrefix (entry->ident, prefix);
          
	  if (attr->Kind == CobolSymbol::Record)
	     PrintRecord (attr, outfile, DisplayOutput, FALSE);
	  else
	     PrintVariable (attr, outfile, DisplayOutput, FALSE);
       }
       delete entry;
    }

    GenIndent();
    outfile << "fprintf (" << DisplayOutput << ", \"\\n\");\n";
}

void SetDisplayOutput (void)
{
StackEntry * OutputStream;
    
    OutputStream = SemStack.Pop();
    strcpy (DisplayOutput, OutputStream->ident);
    delete OutputStream;
}

