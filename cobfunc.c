/* cobfunc.c
**
**	Defines cobol function library.
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdlib.h>
#include "cobfunc.h"

#ifndef min
#   define min(a,b)	((a) < (b) ? (a) : (b))
#endif

int _alphabetic (char * str) 
{
int i;
    for (i = 0; i < strlen(str); i++)
       if (!((str[i] >= 'A' && str[i] <= 'Z') 
	     || (str[i] >= 'a' && str[i] <= 'z') ||
	     (str[i] == ' ')
	    ))
	  return (0);
    return (1);
}

/*
** what = 1  : lowercase test
** what = 2  : uppercase test 
*/
int _alphab_up_low(char * str, int what)
{
int i;

    if (_alphabetic(str)) {
       for (i = 0; i < strlen(str); i++) {
	  if (what == 2) {
	     if (islower (str[i]))
		return (0);
	  }
	  else {
	     if (isupper (str[i]))
		return(0);
	  }
       }
    }
    else 
       return(0);
    return(1);
}

void _RuntimeError (char * message)
{
    printf ("\nRuntime error: %s!\n\n", message);
    exit (1);
}

void _ReadStringVar (FILE * stream, char * var, char * pic)
{
int i, sl;
char c;

    sl = strlen (pic);
    memset (var, ' ', sl);
    for (i = 0; i < sl; ++ i) {
       c = fgetc (stream);
       if (c == '\n' || c == '\x0' || feof(stream))
	  break;
       var[i] = c;
    }
}

void _ReadIntegerVar (FILE * stream, long int var, char * pic)
{
int i;
char buffer[20];
    memset (buffer, ' ', strlen (pic));
    for (i = 0; i < strlen (pic); ++ i)
       buffer[i] = fgetc (stream);
    var = atol (buffer);
}

void _ReadFloatVar (FILE * stream, double var, char * pic)
{
int i;
char buffer[20];
    memset (buffer, ' ', strlen (pic));
    for (i = 0; i < strlen (pic); ++ i)
       buffer[i] = fgetc (stream);
    var = atof (buffer);
}

void _WriteStringVar (FILE * stream, char * var, char * pic)
{
int i;
int j;
    for (i = 0, j = 0; i < strlen(pic); ++ i) {
       switch (pic[i]) {
          case 'x': fprintf (stream, "%c", var[j++]); break;
          case 'b': fprintf (stream, " "); break;
          case '/': fprintf (stream, "/"); break;
          case '0': fprintf (stream, "0"); break;
          default: fprintf (stream, "%c", pic[i]); 
       }
    } 
}

void _WriteIntegerVar (FILE * stream, long int var, char * pic)
{
char wiv_buf [128];
    _IntegerToString (wiv_buf, var, pic);
    fprintf (stream, "%s", wiv_buf);
}

void _IntegerToString (char * string, long int var, char * pic)
{
char buffer[20];
int i, j = 0, nl, pl, fl = 0;
int sp = 0;

    sprintf (buffer, "%lu", var);
    nl = strlen (buffer);
    pl = strlen (pic);

    /* Calculate number of filler zeroes on the left */
    for (i = 0; i < pl; ++ i)
       if (pic[i] == '9' || pic[i] == 'z' || pic[i] == '*')
	  ++ fl;

    /* If the number does not fit, cut leftmost digits */
    if (fl < nl)
       j = nl - fl;

    for (i = 0; i < pl; ++ i) {
       if (pic[i] == '9') {
	  if (fl <= nl)
             string[sp++] = buffer[j++]; 
	  else
             string[sp++] = ' ';
	  -- fl;
       }
       else if (pic[i] == '.') string[sp++] = '.';
       else if (pic[i] == ',') string[sp++] = ',';
       else if (pic[i] == '/') string[sp++] = '/';
       else if (pic[i] == 'b') string[sp++] = ' ';
       else if (pic[i] == '0') string[sp++] = '0';
       else if (pic[i] == '+' || pic[i] == 's' || pic[i] == '-') {
          if (var > 0 && pic[i] == '-')
	     string[sp++] = '+';
	  else if (var < 0)
	     string[sp++] = '-';
	  else
	     string[sp++] = ' ';
       }
       else if (pic[i] == 'z' || pic[i] == '*')
	  string[sp++] = '0';
       else if (pic[i] == '$')
	  string[sp++] = '$';
       else
          string[sp++] = (fl - 1 <= nl) ? pic[i] : ' ';
    }
    string[sp] = '\x0';
}

void _WriteFloatVar (FILE * stream, double var, char * pic)
{
char wfv_buf [128];
    _FloatToString (wfv_buf, var, pic);
    fprintf (stream, "%s", wfv_buf);
}

void _FloatToString (char * string, double var, char * pic)
{
char format_s [10];
char buffer [20];
int fbp = 0, fap = 0, pl;
int i, j = 0;
int FoundPoint = 0;
int sp = 0;

    pl = strlen (pic);

    /* Calculate number of filler zeroes on the left */
    for (i = 0; i < pl; ++ i) {
       if (pic[i] == '9' || pic[i] == '0' || pic[i] == 'z' || pic[i] == '*') {
	  if (FoundPoint)
	     ++ fap;
	  else
	     ++ fbp;
       }
       else if (pic[i] == 'v')
	  FoundPoint = 1;
    }

    sprintf (format_s, "%%%d.%df", fbp, fap);
    sprintf (buffer, format_s, var);
    
    /*
    ** sprintf will print correct number of digits after point,
    ** but only as many as needed before the point.
    ** We thus need to reset the index variable j to the real starting point.
    */
    j = strlen (buffer) - (fbp + 1 + fap);

    for (i = 0; i < pl; ++ i) {
       if (pic[i] == '9') {
	  if (j >= 0)
	     string[sp++] = buffer[j];
	  else
	     string[sp++] = ' ';
	  ++ j;
       }
       else if (pic[i] == '.') string[sp++] = '.';
       else if (pic[i] == ',') string[sp++] = ',';
       else if (pic[i] == '/') string[sp++] = '/';
       else if (pic[i] == 'b') string[sp++] = ' ';
       else if (pic[i] == '0') string[sp++] = '0';
       else if (pic[i] == '+' || pic[i] == 's' || pic[i] == '-') {
          if (var > 0 && pic[i] == '-')
	     string[sp++] = '+';
	  else if (var < 0)
	     string[sp++] = '-';
	  else
	     string[sp++] = ' ';
       }
       else if (pic[i] == 'z' || pic[i] == '*') {
	  string[sp++] = '0';
	  ++ j;
       }
       else if (pic[i] == 'v') {
	  string[sp++] = '.';
	  ++ j;
       }
       else if (pic[i] == '$')
	  string[sp++] = '$';
       else
          string[sp++] = pic[i];
    }
    string[sp] = '\x0';
}

void _AssignVarString (char * var1, char * var2, int p1, int p2)
{
    if (p2 == 0)
       p2 = strlen (var2);

    memset (var1, ' ', p1);
    var1[p1] = '\x0';
    strncpy (var1, var2, min (p1, p2));
    
    if (p1 > p2)
       var1[p2] = ' ';
}

long int _RoundResult (double num)
{
long int tnum;
    tnum = (long int) num;
    if ((num - tnum) < 0.5)
       return (tnum);
    else
       return (tnum + 1);
}

