/* main.cc
**
**       Defines the main module of the COBOL compiler.
*/

#include <iostream.h>
#include <mdefs.h>
#ifdef CAN_HAVE_STDIO
#include <unistd.h>
#include <stdio.h>
#endif
#include "lyextern.h"
#include "semextern.h"

/*---------------------| Globals |------------------------------*/
#ifndef CAN_HAVE_STDIO
  struct FILE;
#endif
  extern FILE * 		yyin;
  char 				SourceFile[30];
  char 				OutputFile[30];
  Stack<StackEntry> 		SemStack;
  int 				NestingLevel = 0;
  HashTable<CobolSymbol> 	SymTable;
  ofstream 			outfile;
/*--------------------------------------------------------------*/

void Usage (void)
{
    cout << "\n";
    cout << "Cobol compiler v0.1, Copyright (c) Mike Sharov, 1995\n";
    cout << "Usage:\n";
    cout << "\tcobol [-o <outfile.c] <file.cob>\n";
    cout << "\n";
}

void ProcessFlags (int argc, char ** argv)
{
int SourceName = 1, OutputName = -1;

    if (strcmp (argv[1], "-o") == 0) {
       SourceName += 2;
       OutputName = 2;
    }
    strcpy (SourceFile, argv[SourceName]);
    if (OutputName < 0) {
       strcpy (OutputFile, argv[SourceName]);
       strcat (OutputFile, ".c");
    }
    else
       strcpy (OutputFile, argv[OutputName]);
}

int main (int argc, char ** argv)
{
    if (argc < 2) {
       Usage();
       return (1);
    }

    ProcessFlags (argc, argv);
    
    if ((yyin = fopen (SourceFile, "r")) == NULL) {
       cerr << "FATAL: Could not open '" << SourceFile << "'!\n";
       return (1);
    }
    else
       cout << "Compiling " << SourceFile << " ...\n";
    
    yyparse();
    fclose (yyin);

    if (ErrorOccured())
       unlink (OutputFile);

    return (0);
}

