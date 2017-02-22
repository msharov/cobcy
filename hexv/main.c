// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "display.h"

int main (int argc, char ** argv)
{
    if (argc != 2) {
	printf ("Usage: %s <file>\n", argv[0]);
	return EXIT_SUCCESS;
    }
    FILE* df = fopen (argv[1], "rb");
    if (!df)
	_RuntimeError ("could not open file %s", argv[1]);
    DisplayOpen();
    ViewBin (df);
    DisplayClose();
    fclose (df);
    return EXIT_SUCCESS;
}
