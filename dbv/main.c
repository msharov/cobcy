/* main.c
**
**	Test program for dbf library
*/

#include "display.h"

int main (int argc, char ** argv)
{
DBF_FILE * df;

    if (argc != 2) {
       printf ("Usage: %s <file.dbf>\n", argv[0]);
       return (0);
    }

    df = DBF_Open (argv[1], "rb");
    DisplayOpen();
    ViewDBF (df);
    DisplayClose();
    DBF_Close (df);
    return (0);
}

