// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "display.h"

int main (int argc, const char* const* argv)
{
    if (argc != 2) {
	printf ("Usage: %s <file.dbf>\n", argv[0]);
	return (0);
    }
    DBF_FILE* df = DBF_Open (argv[1], "rb");
    DisplayOpen();
    ViewDBF (df);
    DisplayClose();
    DBF_Close (df);
    return (0);
}

