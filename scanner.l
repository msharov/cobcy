%{
// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include <math.h>
#include <ctype.h>
#include <string.h>
#include "parser.hh"
#include "semutil.h"

#define STRINGBUFSIZE	200

int CurrentLine = 1;
long int ival;
double fval;
char StringBuffer [STRINGBUFSIZE];
static char ErrorBuffer [80];

#ifndef TRUE
#define TRUE	1
#endif

/*-------------------------------------------------*/
  void StringToLowercase (char * string);
/*-------------------------------------------------*/

/* Read everything in lowercase	*/
#define YY_INPUT(buf,result,max_size) 					\
	if ((result = read (fileno(yyin), (char*) buf, max_size)) < 0) 	\
	    YY_FATAL_ERROR ("read() in flex scanner failed");		\
	StringToLowercase (buf);

%}
%s PICTURE_STATE
%s COMMENT_STATE
%%

[\t ]+		;

^[\t ]*"*"	{ BEGIN COMMENT_STATE; }
<COMMENT_STATE>.*\n {
		  ++ CurrentLine;
		  BEGIN INITIAL;
		  yytext [strlen(yytext) - 1] = '\x0';	/* kill the newline */
		  GenComment (yytext);
		}

<PICTURE_STATE>([\.]*[ax9vbpz09s,\*\+\-\$]+(\([0-9]+\))?)+	{ 
		  	  strcpy (StringBuffer, yytext);
		  	  BEGIN INITIAL;
		  	  return (TOK_PIC_TEXT); 
			}

[0-9]+			{ ival = atol (yytext);
			  return (TOK_INTEGER);	}
[0-9]+\.[0-9]+		{ fval = atof (yytext);
			  return (TOK_FLOAT);	}
\"[a-zA-Z0-9\t \-\{\}\[\]\+\-_\*|\\\/().,\:\';=><\!\$\@\#\%]*\"	{
			  strcpy (StringBuffer, (char*)(yytext + 1));
			  StringBuffer [strlen (StringBuffer) - 1] = '\x0';
			  return (TOK_STRING);	}

accept			{ return (TOK_ACCEPT);	}
access			{ return (TOK_ACCESS);	}
add			{ return (TOK_ADD);	}
advancing		{ return (TOK_ADVANCING);	}
after			{ return (TOK_AFTER);	}
all			{ return (TOK_ALL);	}
alphabet		{ return (TOK_ALPHABET);	}
alphabetic		{ return (TOK_ALPHABETIC);	}
alphabetic-lower        { return (TOK_ALPHABETIC_LOWER); }
alphabetic-upper        { return (TOK_ALPHABETIC_UPPER); }
alphanumeric		{ return (TOK_ALPHANUMERIC);	}
alphanumeric-edited	{ return (TOK_ALPHANUMERIC_EDITED);	}
also			{ return (TOK_ALSO);	}
alter			{ return (TOK_ALTER);	}
alternate		{ return (TOK_ALTERNATE);	}
and			{ return (TOK_AND);	}
any			{ return (TOK_ANY);	}
are			{ return (TOK_ARE);	}
area			{ return (TOK_AREA);	}
areas			{ return (TOK_AREAS);	}
ascending		{ return (TOK_ASCENDING);	}
assign			{ return (TOK_ASSIGN);	}
at			{ return (TOK_AT);	}
author			{ return (TOK_AUTHOR);	}
before			{ return (TOK_BEFORE);	}
binary			{ return (TOK_BINARY);  }
blank			{ return (TOK_BLANK);	}
block			{ return (TOK_BLOCK);	}
bottom			{ return (TOK_BOTTOM);	}
by			{ return (TOK_BY);	}
call			{ return (TOK_CALL);	}
cancel			{ return (TOK_CANCEL);	}
cd			{ return (TOK_CD);	}
cf			{ return (TOK_CF);	}
ch			{ return (TOK_CH);	}
character		{ return (TOK_CHARACTER);	}
characters		{ return (TOK_CHARACTERS);	}
class			{ return (TOK_CLASS);		}
clock-units		{ return (TOK_CLOCK_UNITS);	}
close			{ return (TOK_CLOSE);	}
cobol			{ return (TOK_COBOL);	}
code			{ return (TOK_CODE);	}
code-set		{ return (TOK_CODE_SET);	}
collating		{ return (TOK_COLLATING);	}
column			{ return (TOK_COLUMN);	}
comma			{ return (TOK_COMMA);	}
common			{ return (TOK_COMMON);	}
communication		{ return (TOK_COMMUNICATION);	}
comp			{ return (TOK_COMP);	}
computational		{ return (TOK_COMPUTATIONAL);	}
compute			{ return (TOK_COMPUTE);	}
configuration		{ return (TOK_CONFIGURATION);	}
console			{ return (TOK_CONSOLE);	}
contains		{ return (TOK_CONTAINS);	}
content                 { return (TOK_CONTENT);         }
continue		{ return (TOK_CONTINUE);	}
control			{ return (TOK_CONTROL);	}
controls		{ return (TOK_CONTROLS);	}
copy			{ return (TOK_COPY);	}
corr			{ return (TOK_CORR);	}
corresponding		{ return (TOK_CORRESPONDING);	}
count			{ return (TOK_COUNT);	}
currency		{ return (TOK_CURRENCY);	}
data			{ return (TOK_DATA);	}
date			{ return (TOK_DATE);	}
date-compiled		{ return (TOK_DATE_COMPILED);	}
date-written		{ return (TOK_DATE_WRITTEN);	}
day			{ return (TOK_DAY);	}
day-of-week		{ return (TOK_DAY_OF_WEEK);	}
de			{ return (TOK_DE);	}
debug-contents		{ return (TOK_DEBUG_CONTENTS);	}
debug-item		{ return (TOK_DEBUG_ITEM);	}
debug-line		{ return (TOK_DEBUG_LINE);	}
debug-name		{ return (TOK_DEBUG_NAME);	}
debug-sub-1		{ return (TOK_DEBUG_SUB_1);	}
debug-sub-2		{ return (TOK_DEBUG_SUB_2);	}
debug-sub-3		{ return (TOK_DEBUG_SUB_3);	}
debugging		{ return (TOK_DEBUGGING);	}
decimal-point		{ return (TOK_DECIMAL_POINT);	}
declaratives		{ return (TOK_DECLARATIVES);	}
delete			{ return (TOK_DELETE);	}
delimited		{ return (TOK_DELIMITED);	}
delimiter		{ return (TOK_DELIMITER);	}
depending		{ return (TOK_DEPENDING);	}
depth			{ return (TOK_DEPTH);	}
descending		{ return (TOK_DESCENDING);	}
destination		{ return (TOK_DESTINATION);	}
detail			{ return (TOK_DETAIL);	}
disable			{ return (TOK_DISABLE);	}
display			{ return (TOK_DISPLAY);	}
divide			{ return (TOK_DIVIDE);	}
division		{ return (TOK_DIVISION);	}
down			{ return (TOK_DOWN);	}
duplicates		{ return (TOK_DUPLICATES);	}
dynamic			{ return (TOK_DYNAMIC);	}
egi			{ return (TOK_EGI);	}
elsif			{ return (TOK_ELSIF);	}
else			{ return (TOK_ELSE);	}
emi			{ return (TOK_EMI);	}
enable			{ return (TOK_ENABLE);	}
end			{ return (TOK_END);	}
end-add			{ return (TOK_END_ADD);	}
end-call		{ return (TOK_END_CALL); 	}
end-compute		{ return (TOK_END_COMPUTE);	}
end-delete		{ return (TOK_END_DELETE);	}
end-divide		{ return (TOK_END_DIVIDE);	}
end-evaluate		{ return (TOK_END_EVALUATE);	}
end-if			{ return (TOK_END_IF);		}
end-multiply		{ return (TOK_END_MULTIPLY);	}
end-of-page		{ return (TOK_END_OF_PAGE);	}
end-perform		{ return (TOK_END_PERFORM);	}
end-read		{ return (TOK_END_READ);	}
end-receive		{ return (TOK_END_RECEIVE);	}
end-return		{ return (TOK_END_RETURN);	}
end-rewrite		{ return (TOK_END_REWRITE);	}
end-search		{ return (TOK_END_SEARCH);	}
end-start		{ return (TOK_END_START);	}
end-string		{ return (TOK_END_STRING);	}
end-subtract		{ return (TOK_END_SUBTRACT);	}
end-unstring		{ return (TOK_END_UNSTRING);	}
end-write		{ return (TOK_END_WRITE);	}
enter			{ return (TOK_ENTER);	}
environment		{ return (TOK_ENVIRONMENT);	}
eop			{ return (TOK_EOP);	}
equal			{ return (TOK_EQUAL);	}
error			{ return (TOK_ERROR);	}
esi			{ return (TOK_ESI);	}
evaluate		{ return (TOK_EVALUATE);	}
every			{ return (TOK_EVERY);	}
exception		{ return (TOK_EXCEPTION);	}
exit			{ return (TOK_EXIT);	}
extend			{ return (TOK_EXTEND);	}
external		{ return (TOK_EXTERNAL);	}
false			{ return (TOK_FALSE);	}
fd			{ return (TOK_FD);	}
file			{ return (TOK_FILE);	}
file-control		{ return (TOK_FILE_CONTROL);	}
file-id			{ return (TOK_FILE_ID);	}
filler			{ return (TOK_FILLER);	}
final			{ return (TOK_FINAL);	}
first			{ return (TOK_FIRST);	}
footing			{ return (TOK_FOOTING);	}
for			{ return (TOK_FOR);	}
from			{ return (TOK_FROM);	}
generate		{ return (TOK_GENERATE);	}
giving			{ return (TOK_GIVING);	}
global			{ return (TOK_GLOBAL);	}
go			{ return (TOK_GO);	}
greater			{ return (TOK_GREATER);	}
group			{ return (TOK_GROUP);	}
heading			{ return (TOK_HEADING);	}
high-value	|
high-values		{ return (TOK_HIGH_VALUE);	}
i-o			{ return (TOK_I_O);	}
i-o-control		{ return (TOK_I_O_CONTROL);	}
identification		{ return (TOK_IDENTIFICATION);	}
if			{ return (TOK_IF);	}
in			{ return (TOK_IN);	}
index			{ return (TOK_INDEX);	}
indexed			{ return (TOK_INDEXED);	}
indicate		{ return (TOK_INDICATE);	}
initial			{ return (TOK_INITIAL);	}
initialize		{ return (TOK_INITIALIZE);	}
initiate		{ return (TOK_INITIATE);	}
input			{ return (TOK_INPUT);	}
input-output		{ return (TOK_INPUT_OUTPUT);	}
inspect			{ return (TOK_INSPECT);	}
installation		{ return (TOK_INSTALLATION);	}
into			{ return (TOK_INTO);	}
invalid			{ return (TOK_INVALID);	}
is			{ return (TOK_IS);	}
just			{ return (TOK_JUST);	}
justified		{ return (TOK_JUSTIFIED);	}
key			{ return (TOK_KEY);	}
label			{ return (TOK_LABEL);	}
last			{ return (TOK_LAST);	}
leading			{ return (TOK_LEADING);	}
left			{ return (TOK_LEFT);	}
length			{ return (TOK_LENGTH);	}
less			{ return (TOK_LESS);	}
limit			{ return (TOK_LIMIT);	}
limits			{ return (TOK_LIMITS);	}
linage			{ return (TOK_LINAGE);	}
linage-counter		{ return (TOK_LINAGE_COUNTER);	}
line			{ return (TOK_LINE);	}
line-counter		{ return (TOK_LINE_COUNTER);	}
lines			{ return (TOK_LINES);	}
linkage			{ return (TOK_LINKAGE);	}
lock			{ return (TOK_LOCK);	}
low-value	|
low-values		{ return (TOK_LOW_VALUE);	}
memory			{ return (TOK_MEMORY);	}
merge			{ return (TOK_MERGE);	}
message			{ return (TOK_MESSAGE);	}
mode			{ return (TOK_MODE);	}
modules			{ return (TOK_MODULES);	}
move			{ return (TOK_MOVE);	}
multiple		{ return (TOK_MULTIPLE);	}
multiply		{ return (TOK_MULTIPLY);	}
native			{ return (TOK_NATIVE);	}
negative		{ return (TOK_NEGATIVE);	}
next			{ return (TOK_NEXT);	}
no			{ return (TOK_NO);	}
not			{ return (TOK_NOT);	}
number			{ return (TOK_NUMBER);	}
numeric			{ return (TOK_NUMERIC);	}
numeric-edited		{ return (TOK_NUMERIC_EDITED);	}
object-computer		{ return (TOK_OBJECT_COMPUTER);	}
occurs			{ return (TOK_OCCURS);	}
of			{ return (TOK_OF);	}
off			{ return (TOK_OFF);	}
omitted			{ return (TOK_OMITTED);	}
on			{ return (TOK_ON);	}
open			{ return (TOK_OPEN);	}
optional		{ return (TOK_OPTIONAL);	}
or			{ return (TOK_OR);	}
order			{ return (TOK_ORDER);	}
organization		{ return (TOK_ORGANIZATION);	}
other			{ return (TOK_OTHER);	}
output			{ return (TOK_OUTPUT);	}
overflow		{ return (TOK_OVERFLOW);	}
packed-decimal          { return (TOK_PACKED_DECIMAL);	}
padding			{ return (TOK_PADDING);	}
page			{ return (TOK_PAGE);	}
page-counter		{ return (TOK_PAGE_COUNTER);	}
perform			{ return (TOK_PERFORM);	}
pf			{ return (TOK_PF);	}
ph			{ return (TOK_PH);	}
pic	|
picture			{ BEGIN PICTURE_STATE; return (TOK_PICTURE);	}
plus			{ return (TOK_PLUS);	}
pointer			{ return (TOK_POINTER);	}
position		{ return (TOK_POSITION);	}
positive		{ return (TOK_POSITIVE);	}
printing		{ return (TOK_PRINTING);	}
procedure		{ return (TOK_PROCEDURE);	}
procedures		{ return (TOK_PROCEDURES);	}
proceed			{ return (TOK_PROCEED);	}
program			{ return (TOK_PROGRAM);	}
program\-id		{ return (TOK_PROGRAM_ID);	}
purge			{ return (TOK_PURGE);	}
queue			{ return (TOK_QUEUE);	}
quote	|
quotes			{ return (TOK_QUOTE);	}
random			{ return (TOK_RANDOM);	}
rd			{ return (TOK_RD);	}
read			{ return (TOK_READ);	}
receive			{ return (TOK_RECEIVE);	}
records	|
record			{ return (TOK_RECORD);	}
redefines		{ return (TOK_REDEFINES);	}
reel			{ return (TOK_REEL);	}
reference               { return (TOK_REFERENCE); }
references		{ return (TOK_REFERENCES);	}
relative		{ return (TOK_RELATIVE);	}
release			{ return (TOK_RELEASE);	}
remainder		{ return (TOK_REMAINDER);	}
removal			{ return (TOK_REMOVAL);	}
renames			{ return (TOK_RENAMES);	}
relace			{ return (TOK_REPLACE); }
replacing		{ return (TOK_REPLACING);	}
report			{ return (TOK_REPORT);	}
reporting		{ return (TOK_REPORTING);	}
reports			{ return (TOK_REPORTS);	}
rerun			{ return (TOK_RERUN);	}
reserve			{ return (TOK_RESERVE);	}
reset			{ return (TOK_RESET);	}
return			{ return (TOK_RETURN);	}
reversed		{ return (TOK_REVERSED);	}
rewind			{ return (TOK_REWIND);	}
rewrite			{ return (TOK_REWRITE);	}
rf			{ return (TOK_RF);	}
rh			{ return (TOK_RH);	}
right			{ return (TOK_RIGHT);	}
rounded			{ return (TOK_ROUNDED);	}
run			{ return (TOK_RUN);	}
same			{ return (TOK_SAME);	}
sd			{ return (TOK_SD);	}
search			{ return (TOK_SEARCH);	}
section			{ return (TOK_SECTION);	}
security		{ return (TOK_SECURITY);	}
segment			{ return (TOK_SEGMENT);	}
segment-limit		{ return (TOK_SEGMENT_LIMIT);	}
select			{ return (TOK_SELECT);	}
send			{ return (TOK_SEND);	}
sentence		{ return (TOK_SENTENCE);	}
separate		{ return (TOK_SEPARATE);	}
sequence		{ return (TOK_SEQUENCE);	}
sequential		{ return (TOK_SEQUENTIAL);	}
set			{ return (TOK_SET);	}
sign			{ return (TOK_SIGN);	}
size			{ return (TOK_SIZE);	}
sort			{ return (TOK_SORT);	}
sort-merge		{ return (TOK_SORT_MERGE);	}
source			{ return (TOK_SOURCE);	}
source-computer		{ return (TOK_SOURCE_COMPUTER);	}
space	|
spaces			{ strcpy (StringBuffer, "_space_var");
			  return (TOK_IDENTIFIER);}
special-names		{ return (TOK_SPECIAL_NAMES);	}
standard		{ return (TOK_STANDARD);	}
standard-1		{ return (TOK_STANDARD_1);	}
standard-2		{ return (TOK_STANDARD_2);	}
start			{ return (TOK_START);	}
status			{ return (TOK_STATUS);	}
stop			{ return (TOK_STOP);	}
string			{ return (TOK_STRING);	}
sub-queue-1		{ return (TOK_SUB_QUEUE_1);	}
sub-queue-2		{ return (TOK_SUB_QUEUE_2);	}
sub-queue-3		{ return (TOK_SUB_QUEUE_3);	}
subtract		{ return (TOK_SUBTRACT);	}
sum			{ return (TOK_SUM);	}
supress			{ return (TOK_SUPRESS);	}
symbolic		{ return (TOK_SYMBOLIC);	}
sync	|
synchronized		{ return (TOK_SYNC);	}
table			{ return (TOK_TABLE);	}
tallying		{ return (TOK_TALLYING);	}
tape			{ return (TOK_TAPE);	}
terminal		{ return (TOK_TERMINAL);	}
terminate		{ return (TOK_TERMINATE);	}
test			{ return (TOK_TEST);	}
text			{ return (TOK_TEXT);	}
than			{ return (TOK_THAN);	}
then			{ return (TOK_THEN);	}
through			{ return (TOK_THROUGH);	}
thru			{ return (TOK_THRU);	}
time			{ return (TOK_TIME);	}
times			{ return (TOK_TIMES);	}
to			{ return (TOK_TO);	}
top			{ return (TOK_TOP);	}
trailing		{ return (TOK_TRAILING);	}
true			{ return (TOK_TRUE);	}
type			{ return (TOK_TYPE);	}
unit			{ return (TOK_UNIT);	}
unstring		{ return (TOK_UNSTRING);	}
until			{ return (TOK_UNTIL);	}
up			{ return (TOK_UP);	}
upon			{ return (TOK_UPON);	}
usage			{ return (TOK_USAGE);	}
use			{ return (TOK_USE);	}
using			{ return (TOK_USING);	}
value	|
values			{ return (TOK_VALUE);	}
varying			{ return (TOK_VARYING);	}
when			{ return (TOK_WHEN);	}
while			{ return (TOK_WHILE);	}
with			{ return (TOK_WITH);	}
words			{ return (TOK_WORDS);	}
working-storage		{ return (TOK_WORKING_STORAGE);	}
write			{ return (TOK_WRITE);	}
zero	|
zeroes	|
zeros 			{ strcpy (StringBuffer, "_zero_var");
			  return (TOK_IDENTIFIER); }

[a-z]([a-z0-9\-]*[a-z0-9])?	{ strcpy (StringBuffer, yytext);
				  return (TOK_IDENTIFIER); }

"+"			{ return (TOK_PLUS);	}
"-"			{ return (TOK_MINUS);	}
"*"			{ return (TOK_ASTERISK);	}
"/"			{ return (TOK_SLASH);	}
"="			{ return (TOK_EQUAL);	}
"$"			{ return (TOK_DOLLAR);	}
","			{ return (TOK_COMMA);	}
";"			{ return (TOK_SEMICOLON);	}
"."			{ return (TOK_PERIOD);	}
"("			{ return (TOK_LPAREN);	}
")"			{ return (TOK_RPAREN);	}
">="                    { return (TOK_GREATER_EQ); }
"<="                    { return (TOK_LESS_EQ); }
">"			{ return (TOK_GREATER);	}
"<"			{ return (TOK_LESS);	}

\n		{ ++ CurrentLine; }

.		{ sprintf (ErrorBuffer, "Line %d: parse error on |%s|\n", 
		  	   CurrentLine, yytext); 
		  WriteError (ErrorBuffer);
		}

%%

void StringToLowercase (char * string)
{
register int i, StringLength;
int QuotedText = 0;

    StringLength = strlen (string);
    for (i = 0; i < StringLength; ++ i) {
       if (string[i] == '"')
	  QuotedText ^= 1;
       else if (!QuotedText)
          string[i] = tolower (string[i]);
    }
}

