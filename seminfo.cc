/* seminfo.cc
**
**	Implements informational semantic actions for COBOL compiler.
*/

#include "semextern.h"

void SetProgramName (void)
{
StackEntry * entry;
    if ((entry = SemStack.Pop()) != NULL) {
       if (entry->kind == SE_Identifier) {
          outfile << "/* This program is called ";
	  outfile << entry->ident << " */\n";
       }
       else
	  WriteError ("Program name must be one word");
       delete (entry);
    }
    else
       WriteError ("Program name expected");
}

void SetSourceComputer (void)
{
StackEntry * entry;
    if ((entry = SemStack.Pop()) != NULL) {
       if (entry->kind == SE_Identifier)
          outfile << "/* Written on " << entry->ident << " */\n";
       else
	  WriteError ("Source computer name must be one word");
       delete (entry);
    }
    else
       WriteError ("Source computer name expected");
}

void SetObjectComputer (void)
{
StackEntry * entry;
    if ((entry = SemStack.Pop()) != NULL) {
       if (entry->kind == SE_Identifier)
          outfile << "/* Written for " << entry->ident << " */\n\n";
       else
	  WriteError ("Object computer name must be one word");
       delete (entry);
    }
    else
       WriteError ("Object computer name expected");
}

