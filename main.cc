/* main.cc
**
**       Defines the main module of the COBOL compiler.
*/

#include "adtlib/mdefs.h"
#include <unistd.h>
#include "lyextern.h"
#include "semextern.h"

//----------------------------------------------------------------------

extern FILE* yyin;
CobcyConfigType CobcyConfig;

//----------------------------------------------------------------------

static void Usage (const char* progname);
static int ProcessFlags (int argc, const char* const* argv);
static void SetInitialConfiguration (void);

//----------------------------------------------------------------------

int main (int argc, const char* const* argv)
{
    SetInitialConfiguration();
    if (ProcessFlags (argc, argv) == S_ERROR) {
	Usage (argv[0]);
	return (2);
    }
    
    if ((yyin = fopen (CobcyConfig.SourceFile, "r")) == NULL) {
	cerr << "FATAL: Could not open '" << CobcyConfig.SourceFile << "'!\n";
	return (1);
    }
    else
	cout << "Compiling " << CobcyConfig.SourceFile << " ...\n";
    
    yyparse();
    fclose (yyin);

    if (ErrorOccured()) {
	unlink (CobcyConfig.CodeFile);
	unlink (CobcyConfig.DeclFile);
    }

    return (0);
}

static void Usage (const char* progname)
{
    cout << "\n";
    cout << "Cobol to C compiler v0.1, Copyright (c) Mike Sharov, 1996\n";
    cout << "Usage:\n";
    cout << "\t" << progname << " [options] <file.cob>\n";
    cout << "\n";
    cout << "\tOptions:\n";
    cout << "\t-g\t\tGenerate compiler debugging info (_cpi trace)\n";
    cout << "\t-o <file.c>\tFile where to put the C code\n";
    cout << "\t\t\t<file.c>.h will also be made with decls.\n";
    cout << "\t\t\tIf this is not specified, Cobcy uses\n";
    cout << "\t\t\t<file.cob>.c and <file.cob>.h\n";
    cout << "\n";
}

static int ProcessFlags (int argc, const char* const* argv)
{
int i, NameLength;
enum {
    FlagMode,
    SourceFileMode,	/* This will be the default */
    OutputFileMode
} mode = FlagMode;
BOOL SourceSet = FALSE, OutputSet = FALSE;

    mode = SourceFileMode;
    for (i = 1; i < argc; ++ i) {
	if (argv[i][0] == '-')
	    mode = FlagMode;

	switch (mode) {
	    case FlagMode:
		if (strcmp (argv[i], "-o") == 0)
		    mode = OutputFileMode;
		else if (strcmp (argv[i], "-g") == 0)
		    CobcyConfig.GenDebug = TRUE;

		// If not -o, revert to looking for the source file
		if (mode != OutputFileMode)
		    mode = SourceFileMode;
		break;
	    case SourceFileMode:
		if (!SourceSet) {
		    strcpy (CobcyConfig.SourceFile, argv[i]);
		    if (!OutputSet) {
			strcpy (CobcyConfig.CodeFile, CobcyConfig.SourceFile);
			strcat (CobcyConfig.CodeFile, ".c");
			strcpy (CobcyConfig.DeclFile, CobcyConfig.SourceFile);
			strcat (CobcyConfig.DeclFile, ".h");
		    }
		    SourceSet = TRUE;
		}
		break;
	    case OutputFileMode:
		if (!OutputSet) {
		    strcpy (CobcyConfig.CodeFile, argv[i]);
		    strcpy (CobcyConfig.DeclFile, argv[i]);
		    /* If possible, remove the ending .c extension 	*/
		    /* Do this by replacing the dot with EOS 		*/
		    NameLength = strlen (CobcyConfig.DeclFile);
		    if (CobcyConfig.DeclFile [NameLength - 1] == 'c' &&
		        CobcyConfig.DeclFile [NameLength - 2] == '.')
		        CobcyConfig.DeclFile [NameLength - 2] = '\x0';

		    strcat (CobcyConfig.DeclFile, ".h");
		    mode = SourceFileMode;
		    OutputSet = TRUE;
		}
		break;
	}
    }

#ifndef NDEBUG
    cerr << "Reading from " << CobcyConfig.SourceFile << "\n";
    cerr << "Compiling into " << CobcyConfig.CodeFile << " and ";
    cerr << CobcyConfig.DeclFile << "\n";
#endif

    if (!SourceSet) {
	cerr << "No cobol file specified\n";
	return (S_ERROR);
    }
    else
	return (S_OK);
}

void SetInitialConfiguration (void)
{
    memset (CobcyConfig.SourceFile, 0, MAX_FILENAME);
    memset (CobcyConfig.CodeFile, 0, MAX_FILENAME);
    memset (CobcyConfig.DeclFile, 0, MAX_FILENAME);
    CobcyConfig.GenDebug = FALSE;
}

