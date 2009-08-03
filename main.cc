// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include <unistd.h>
#include <stdio.h>

//----------------------------------------------------------------------

extern FILE* yyin;
extern int yyparse (void);
CobcyConfigType CobcyConfig;

//----------------------------------------------------------------------

static void Usage (const char* progname);
static int ProcessFlags (int argc, const char* const* argv);
static void SetInitialConfiguration (void);

//----------------------------------------------------------------------

int main (int argc, const char* const* argv)
{
    SetInitialConfiguration();
    if (ProcessFlags (argc, argv) == -1) {
	Usage (argv[0]);
	return (2);
    }

    if ((yyin = fopen (CobcyConfig.SourceFile, "r")) == NULL) {
	cerr << "FATAL: Could not open '" << CobcyConfig.SourceFile << "'!\n";
	return (1);
    }
#ifndef NDEBUG
    else cout << "Compiling " << CobcyConfig.SourceFile << " ...\n";
#endif

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
    cout << "\n"
	    "Cobol to C compiler v0.1, Copyright (c) Mike Sharov, 1996\n"
	    "Usage:\n"
	    "\t" << progname << " [options] <file.cob>\n"
	    "\n"
	    "\tOptions:\n"
	    "\t-g\t\tGenerate compiler debugging info (_cpi trace)\n"
	    "\t-o <file.c>\tFile where to put the C code\n"
	    "\t\t\t<file.c>.h will also be made with decls.\n"
	    "\t\t\tIf this is not specified, Cobcy uses\n"
	    "\t\t\t<file.cob>.c and <file.cob>.h\n"
	    "\n";
}

static int ProcessFlags (int argc, const char* const* argv)
{
    int NameLength;
    enum {
	FlagMode,
	SourceFileMode,	// This will be the default
	OutputFileMode
    } mode = FlagMode;
    bool SourceSet = false, OutputSet = false;

    mode = SourceFileMode;
    for (int i = 1; i < argc; ++ i) {
	if (argv[i][0] == '-')
	    mode = FlagMode;

	switch (mode) {
	    case FlagMode:
		if (strcmp (argv[i], "-o") == 0)
		    mode = OutputFileMode;
		else if (strcmp (argv[i], "-g") == 0)
		    CobcyConfig.GenDebug = true;

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
		    SourceSet = true;
		}
		break;
	    case OutputFileMode:
		if (!OutputSet) {
		    strcpy (CobcyConfig.CodeFile, argv[i]);
		    strcpy (CobcyConfig.DeclFile, argv[i]);
		    // If possible, remove the ending .c extension
		    // Do this by replacing the dot with EOS
		    NameLength = strlen (CobcyConfig.DeclFile);
		    if (CobcyConfig.DeclFile [NameLength - 1] == 'c' &&
		        CobcyConfig.DeclFile [NameLength - 2] == '.')
		        CobcyConfig.DeclFile [NameLength - 2] = '\x0';

		    strcat (CobcyConfig.DeclFile, ".h");
		    mode = SourceFileMode;
		    OutputSet = true;
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
	return (-1);
    }
    return (0);
}

static void SetInitialConfiguration (void)
{
    memset (CobcyConfig.SourceFile, 0, MAX_FILENAME);
    memset (CobcyConfig.CodeFile, 0, MAX_FILENAME);
    memset (CobcyConfig.DeclFile, 0, MAX_FILENAME);
    CobcyConfig.GenDebug = false;
}
