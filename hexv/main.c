// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "display.h"

int main (int argc, char ** argv)
{
    if (argc != 2) {
	printf ("Usage: %s <file>\n", argv[0]);
	return (0);
    }
    FILE * df;
    if ((df = fopen (argv[1], "rb")) == NULL) {
	printf ("Could not open file %s!\n", argv[1]);
	return (1);
    }
    DisplayOpen();
    ViewBin (df);
    DisplayClose();
    fclose (df);
    return (0);
}

