..\cobcy -nc %1.cob
gcc -g -I.. -I../adtlib/include  -L.. -L../adtlib/lib -L../dbf -o %1.exe %1.cob.c -llibcobol -llibdbf