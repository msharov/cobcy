/* main.cc
**
**       Defines the main module of the COBOL compiler.
*/

#include <iostream.h>
#include <mdefs.h>
#ifdef CAN_HAVE_STDIO
#ifndef __MSDOS__
#include <unistd.h>
#endif
#include <stdio.h>
#endif
#include "lyextern.h"
#ifdef __MSDOS__
#include "semexter.h"	/* I have no intention of shortening file names. */
#else
#include "semextern.h"
#endif

/*---------------------| Globals |------------------------------*/
#ifndef CAN_HAVE_STDIO
  struct FILE;
#endif
  extern FILE * 		yyin;
  char 				SourceFile[30];
  char				OutputFile[30];
  char 				CodeFile[30];
  char 				DeclFile[30];
/*--------------------------------------------------------------*/
  void 	Usage (void);
  void 	ProcessFlags (int argc, char ** argv);
/*--------------------------------------------------------------*/

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

    if (ErrorOccured()) {
       unlink (CodeFile);
       unlink (DeclFile);
    }

    return (0);
}

void Usage (void)
{
    cout << "\n";
    cout << "Cobol compiler v0.1, Copyright (c) Mike Sharov, 1995\n";
    cout << "Usage:\n";
    cout << "\tcobol [-o <codef.c] <file.cob>\n";
    cout << "\n";
}

void ProcessFlags (int argc, char ** argv)
{
int SourceName = 1, OutputName = -1;
int count;

    if (strcmp (argv[1], "-o") == 0) {
       SourceName += 2;
       OutputName = 2;
    }
    strcpy (SourceFile, argv[SourceName]);
    if (OutputName < 0) {
       strcpy (CodeFile, argv[SourceName]);
       strcat (CodeFile, ".c");
       strcpy (DeclFile, argv[SourceName]);
       strcat (DeclFile, ".h");
    }
    else {
       strcpy (CodeFile, argv[OutputName]);
       strcpy (DeclFile, argv[OutputName]);
       strcat (DeclFile, ".h");
    }

}

