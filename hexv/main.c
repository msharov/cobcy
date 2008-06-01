/* main.c
*/

#include "display.h"

int main (int argc, char ** argv)
{
FILE * df;

    if (argc != 2) {
       printf ("Usage: %s <file>\n", argv[0]);
       return (0);
    }

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

