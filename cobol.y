%{
/* cobol.y
**
**	Defines COBOL's grammar.
*/

#include <stdio.h>
#include <string.h>
#ifdef __MSDOS__
#include "semactio.h"
#else
#include "semactions.h"
#endif
#include "lyextern.h"

extern int CurrentLine;
extern long int ival;
extern double fval;
extern char StringBuffer[];
SEOperatorKind opkind;
long int FillerIndex = 0;
#ifndef TRUE
#define TRUE		1
#endif

/*-----------------------------------*/
  int 	yyerror (char * msg);
/*-----------------------------------*/

%}
%token TOK_ACCEPT
%token TOK_ACCESS
%token TOK_ADD
%token TOK_ADVANCING
%token TOK_AFTER
%token TOK_ALL
%token TOK_ALPHABET
%token TOK_ALPHABETIC
%token TOK_ALPHABETIC_LOWER
%token TOK_ALPHABETIC_UPPER
%token TOK_ALPHANUMERIC
%token TOK_ALPHANUMERIC_EDITED
%token TOK_ALSO
%token TOK_ALTER
%token TOK_ALTERNATE
%token TOK_AND
%token TOK_ANY
%token TOK_ARE
%token TOK_AREA
%token TOK_AREAS
%token TOK_ASCENDING
%token TOK_ASSIGN
%token TOK_AT
%token TOK_AUTHOR
%token TOK_BEFORE
%token TOK_BINARY
%token TOK_BLANK
%token TOK_BLOCK
%token TOK_BOTTOM
%token TOK_BY
%token TOK_CALL
%token TOK_CANCEL
%token TOK_CD
%token TOK_CF
%token TOK_CH
%token TOK_CHARACTER
%token TOK_CHARACTERS
%token TOK_CLASS
%token TOK_CLOCK_UNITS
%token TOK_CLOSE
%token TOK_COBOL
%token TOK_CODE
%token TOK_CODE_SET
%token TOK_COLLATING
%token TOK_COLUMN
%token TOK_COMMA
%token TOK_COMMON
%token TOK_COMMUNICATION
%token TOK_COMP
%token TOK_COMPUTATIONAL
%token TOK_COMPUTE
%token TOK_CONFIGURATION
%token TOK_CONSOLE
%token TOK_CONTAINS
%token TOK_CONTENT
%token TOK_CONTINUE
%token TOK_CONTROL
%token TOK_CONTROLS
%token TOK_COPY
%token TOK_CORR
%token TOK_CORRESPONDING
%token TOK_COUNT
%token TOK_CURRENCY
%token TOK_DATA
%token TOK_DATE
%token TOK_DATE_COMPILED
%token TOK_DATE_WRITTEN
%token TOK_DAY
%token TOK_DAY_OF_WEEK
%token TOK_DE
%token TOK_DEBUG_CONTENTS
%token TOK_DEBUG_ITEM
%token TOK_DEBUG_LINE
%token TOK_DEBUG_NAME
%token TOK_DEBUG_SUB_1
%token TOK_DEBUG_SUB_2
%token TOK_DEBUG_SUB_3
%token TOK_DEBUGGING
%token TOK_DECIMAL_POINT
%token TOK_DECLARATIVES
%token TOK_DELETE
%token TOK_DELIMITED
%token TOK_DELIMITER
%token TOK_DEPENDING
%token TOK_DEPTH
%token TOK_DESCENDING
%token TOK_DESTINATION
%token TOK_DETAIL
%token TOK_DISABLE
%token TOK_DISPLAY
%token TOK_DIVIDE
%token TOK_DIVISION
%token TOK_DOWN
%token TOK_DUPLICATES
%token TOK_DYNAMIC
%token TOK_EGI
%token TOK_ELSIF
%token TOK_ELSE
%token TOK_EMI
%token TOK_ENABLE
%token TOK_END
%token TOK_END_ADD
%token TOK_END_CALL
%token TOK_END_COMPUTE
%token TOK_END_DELETE
%token TOK_END_DIVIDE
%token TOK_END_EVALUATE
%token TOK_END_IF
%token TOK_END_MULTIPLY
%token TOK_END_OF_PAGE
%token TOK_END_PERFORM
%token TOK_END_READ
%token TOK_END_RECEIVE
%token TOK_END_RETURN
%token TOK_END_REWRITE
%token TOK_END_SEARCH
%token TOK_END_START
%token TOK_END_STRING
%token TOK_END_SUBTRACT
%token TOK_END_UNSTRING
%token TOK_END_WRITE
%token TOK_ENTER
%token TOK_ENVIRONMENT
%token TOK_EOP
%token TOK_EQUAL
%token TOK_ERROR
%token TOK_ESI
%token TOK_EVALUATE
%token TOK_EVERY
%token TOK_EXCEPTION
%token TOK_EXIT
%token TOK_EXTEND
%token TOK_EXTERNAL
%token TOK_FALSE
%token TOK_FD
%token TOK_FILE
%token TOK_FILE_CONTROL
%token TOK_FILLER
%token TOK_FINAL
%token TOK_FIRST
%token TOK_FOOTING
%token TOK_FOR
%token TOK_FROM
%token TOK_GENERATE
%token TOK_GIVING
%token TOK_GLOBAL
%token TOK_GO
%token TOK_GREATER
%token TOK_GREATER_EQ
%token TOK_GROUP
%token TOK_HEADING
%token TOK_HIGH_VALUE
%token TOK_I_O
%token TOK_I_O_CONTROL
%token TOK_IDENTIFICATION
%token TOK_IF
%token TOK_IN
%token TOK_INDEX
%token TOK_INDEXED
%token TOK_INDICATE
%token TOK_INITIAL
%token TOK_INITIALIZE
%token TOK_INITIATE
%token TOK_INPUT
%token TOK_INPUT_OUTPUT
%token TOK_INSPECT
%token TOK_INSTALLATION
%token TOK_INTO
%token TOK_INVALID
%token TOK_IS
%token TOK_JUST
%token TOK_JUSTIFIED
%token TOK_KEY
%token TOK_LABEL
%token TOK_LAST
%token TOK_LEADING
%token TOK_LEFT
%token TOK_LENGTH
%token TOK_LESS
%token TOK_LESS_EQ
%token TOK_LIMIT
%token TOK_LIMITS
%token TOK_LINAGE
%token TOK_LINAGE_COUNTER
%token TOK_LINE
%token TOK_LINE_COUNTER
%token TOK_LINES
%token TOK_LINKAGE
%token TOK_LOCK
%token TOK_LOW_VALUE
%token TOK_MEMORY
%token TOK_MERGE
%token TOK_MESSAGE
%token TOK_MODE
%token TOK_MODULES
%token TOK_MOVE
%token TOK_MULTIPLE
%token TOK_MULTIPLY
%token TOK_NATIVE
%token TOK_NEGATIVE
%token TOK_NEXT
%token TOK_NO
%token TOK_NOT
%token TOK_NUMBER
%token TOK_NUMERIC
%token TOK_NUMERIC_EDITED
%token TOK_OBJECT_COMPUTER
%token TOK_OCCURS
%token TOK_OF
%token TOK_OFF
%token TOK_OMITTED
%token TOK_ON
%token TOK_OPEN
%token TOK_OPTIONAL
%token TOK_OR
%token TOK_ORDER
%token TOK_ORGANIZATION
%token TOK_OTHER
%token TOK_OUTPUT
%token TOK_OVERFLOW
%token TOK_PACKED_DECIMAL
%token TOK_PADDING
%token TOK_PAGE
%token TOK_PAGE_COUNTER
%token TOK_PERFORM
%token TOK_PF
%token TOK_PH
%token TOK_PIC
%token TOK_PICTURE
%token TOK_PLUS
%token TOK_POINTER
%token TOK_POSITION
%token TOK_POSITIVE
%token TOK_PRINTING
%token TOK_PROCEDURE
%token TOK_PROCEDURES
%token TOK_PROCEED
%token TOK_PROGRAM
%token TOK_PROGRAM_ID
%token TOK_PURGE
%token TOK_QUEUE
%token TOK_QUOTE
%token TOK_RANDOM
%token TOK_RD
%token TOK_READ
%token TOK_RECEIVE
%token TOK_RECORD
%token TOK_RECORDS
%token TOK_REDEFINES
%token TOK_REEL
%token TOK_REFERENCE
%token TOK_REFERENCES
%token TOK_RELATIVE
%token TOK_RELEASE
%token TOK_REMAINDER
%token TOK_REMOVAL
%token TOK_RENAMES
%token TOK_REPLACE
%token TOK_REPLACING
%token TOK_REPORT
%token TOK_REPORTING
%token TOK_REPORTS
%token TOK_RERUN
%token TOK_RESERVE
%token TOK_RESET
%token TOK_RETURN
%token TOK_REVERSED
%token TOK_REWIND
%token TOK_REWRITE
%token TOK_RF
%token TOK_RH
%token TOK_RIGHT
%token TOK_ROUNDED
%token TOK_RUN
%token TOK_SAME
%token TOK_SD
%token TOK_SEARCH
%token TOK_SECTION
%token TOK_SECURITY
%token TOK_SEGMENT
%token TOK_SEGMENT_LIMIT
%token TOK_SELECT
%token TOK_SEND
%token TOK_SENTENCE
%token TOK_SEPARATE
%token TOK_SEQUENCE
%token TOK_SEQUENTIAL
%token TOK_SET
%token TOK_SIGN
%token TOK_SIZE
%token TOK_SORT
%token TOK_SORT_MERGE
%token TOK_SOURCE
%token TOK_SOURCE_COMPUTER
%token TOK_SPACE
%token TOK_SPACES
%token TOK_SPECIAL_NAMES
%token TOK_STANDARD
%token TOK_STANDARD_1
%token TOK_STANDARD_2
%token TOK_START
%token TOK_STATUS
%token TOK_STOP
%token TOK_STRING
%token TOK_SUB_QUEUE_1
%token TOK_SUB_QUEUE_2
%token TOK_SUB_QUEUE_3
%token TOK_SUBTRACT
%token TOK_SUM
%token TOK_SUPRESS
%token TOK_SYMBOLIC
%token TOK_SYNC
%token TOK_TABLE
%token TOK_TALLYING
%token TOK_TAPE
%token TOK_TERMINAL
%token TOK_TERMINATE
%token TOK_TEST
%token TOK_TEXT
%token TOK_THAN
%token TOK_THEN
%token TOK_THROUGH
%token TOK_THRU
%token TOK_TIME
%token TOK_TIMES
%token TOK_TO
%token TOK_TOP
%token TOK_TRAILING
%token TOK_TRUE
%token TOK_TYPE
%token TOK_UNIT
%token TOK_UNSTRING
%token TOK_UNTIL
%token TOK_UP
%token TOK_UPON
%token TOK_USAGE
%token TOK_USE
%token TOK_USING
%token TOK_VALUE
%token TOK_VALUES
%token TOK_VARYING
%token TOK_WHEN
%token TOK_WITH
%token TOK_WHILE
%token TOK_WORDS
%token TOK_WORKING_STORAGE
%token TOK_WRITE
%token TOK_ZERO
%token TOK_IDENTIFIER
%token TOK_MINUS
%token TOK_ASTERISK
%token TOK_SLASH
%token TOK_EQUAL
%token TOK_DOLLAR
%token TOK_COMMA
%token TOK_SEMICOLON
%token TOK_PERIOD
%token TOK_LPAREN
%token TOK_RPAREN
%token TOK_GREATER
%token TOK_LESS
%token TOK_RPAREN
%token TOK_INTEGER
%token TOK_FLOAT
%token TOK_STRING
%token TOK_PIC_TEXT
%%

program:	{ StartProgram(); }
		identification_division
		environment_division
		data_division
		procedure_division
		{ EndProgram(); }
	;

identification_division:	TOK_IDENTIFICATION TOK_DIVISION TOK_PERIOD
		program_name
	;

program_name:	TOK_PROGRAM_ID TOK_PERIOD identifier prg_name_option TOK_PERIOD
		{ SetProgramName(); }
	;

prg_name_option:	optional_is common_initial optional_program
        |
        ;

common_initial:		TOK_COMMON
	|		TOK_INITIAL
	;

environment_division:	TOK_ENVIRONMENT TOK_DIVISION TOK_PERIOD
		configuration_section
		input_output_section
	;

configuration_section:	TOK_CONFIGURATION TOK_SECTION TOK_PERIOD
			source_computer
			object_computer
			special_names
	|
	;

source_computer:	TOK_SOURCE_COMPUTER TOK_PERIOD
			identifier source_debug_option TOK_PERIOD
			{ SetSourceComputer(); }
	|
	;

source_debug_option:	optional_with TOK_DEBUGGING TOK_MODE
	|
	;

object_computer:	TOK_OBJECT_COMPUTER TOK_PERIOD 
			identifier memory_option TOK_PERIOD
			{ SetObjectComputer(); }
	|
	;

memory_option:		TOK_MEMORY optional_size integer memsize_args
                        { GenComment("Memory size is obsolete and ignored"); }
        |
        ;

memsize_args:		TOK_WORDS
	|		TOK_CHARACTERS
	|		TOK_MODULES
        ;

special_names:		TOK_SPECIAL_NAMES TOK_PERIOD special_name_list
	|
	;
special_name_list:	special_name_decl special_name_list
	|
	;
special_name_decl:	identifier TOK_IS identifier 
			{ DeclareSpecialName(); }
			TOK_PERIOD
	;

input_output_section:	TOK_INPUT_OUTPUT TOK_SECTION TOK_PERIOD
			file_control
	|
	;

file_control:	TOK_FILE_CONTROL TOK_PERIOD
			select_block
	|
	;

select_block:	select_statement select_block_pl
	;
select_block_pl:	select_block
	|
	;
select_statement:	TOK_SELECT optional_optional identifier TOK_ASSIGN optional_to identifier
			file_status_option
			access_mode_option
			organization_option
			TOK_PERIOD { FileDecl(); }
	;

file_status_option:	optional_file TOK_STATUS optional_is 
			identifier { SetFileStatus(); }
	|
	;
access_mode_option:	TOK_ACCESS optional_mode optional_is access_mode
	|
	;
access_mode:	TOK_SEQUENTIAL	{ SetAccessMode ("sequential"); }
	|	TOK_RANDOM	{ SetAccessMode ("random"); }
	|	TOK_DYNAMIC	{ SetAccessMode ("dynamic"); }
	;
organization_option:	TOK_ORGANIZATION optional_is organization_kind
	|
	;
organization_kind:	TOK_SEQUENTIAL		
			{ SetOrganization ("sequential"); }
	|		TOK_LINE TOK_SEQUENTIAL
			{ SetOrganization ("line sequential"); }
	|		TOK_RELATIVE 
			{ SetOrganization ("relative"); }
			relative_key_option
	|		TOK_INDEXED 
			{ SetOrganization ("indexed"); }
			record_key_option
	|
	;
relative_key_option:	TOK_RELATIVE optional_key optional_is 
			identifier { SetRelativeKey(); }
	|
	;
record_key_option:	TOK_RECORD TOK_KEY optional_is 
			identifier { SetRecordKey(); }
	|
	;

data_division:	TOK_DATA TOK_DIVISION TOK_PERIOD
		file_section
		working_storage_section
                linkage_section
                communication_section
                report_section
	;

file_section:	TOK_FILE TOK_SECTION TOK_PERIOD
		file_desc_block
	|
	;

file_desc_block:	file_desc_entry file_desc_block_pl
	;
file_desc_block_pl:	file_desc_block
	|
	;

file_desc_entry:	TOK_FD identifier TOK_LABEL TOK_RECORD optional_is
			file_desc_type file_name_option
			{ GenFileDesc(); } TOK_PERIOD
			file_record_desc 
	;
file_desc_type:		TOK_STANDARD
	|		TOK_OMITTED
	;
file_name_option:	TOK_VALUE optional_is
			identifier
	|		{ StringBuffer[0] = 0; 
			  Push (SE_Identifier); 
			}
	;

file_record_desc:	file_record_level file_record_desc_pl
	;
file_record_desc_pl:	file_record_desc
	|
	;
file_record_level:	integer level_name { AssociateFileRecord(); }
			picture TOK_PERIOD 
			{ Push (SE_Null); DeclareRecordLevel(); }

record_level:	integer 
		level_name 
		picture 
		value_entry 
		usage_option 
		sign_option 
		sync_option 
		justified_option 
		TOK_PERIOD
		{ DeclareRecordLevel(); }
	;

usage_option:          TOK_USAGE optional_is pict_usage_args
        |
        ;

sign_option:	optional_sign optional_is sign_args 
		sep_char_option { NIY("Sign is [leading/trailing]"); }
        |
        ;

sign_args:		TOK_LEADING
        |		TOK_TRAILING
	;

sep_char_option:	TOK_SEPARATE optional_character
        |
        ;

justified_option:	justified_just optional_right { NIY("Just/Justified"); }
	|
	;

justified_just:		TOK_JUSTIFIED
	|		TOK_JUST
	;

sync_option:		TOK_SYNC left_right_option 
			{ NIY("Sync/Synchronized"); } 
	|
	;

left_right_option:	left_right
	|
	;

left_right:		TOK_LEFT
	|		TOK_RIGHT
	;

level_name:	identifier
	|	TOK_FILLER	
	 	{ sprintf (StringBuffer, "filler%03ld", FillerIndex);
		  ++ FillerIndex;
		  Push (SE_Identifier);
		}
	;
picture:	TOK_PICTURE optional_is pic_text
	|	{ Push (SE_Null); }
	;

pict_usage_args:    TOK_BINARY
                    { NIY("Usage is binary"); }
        |           TOK_COMPUTATIONAL
                    { NIY("Usage is computational"); }
        |           TOK_COMP
                    { NIY("Usage is comp"); }
        |           TOK_DISPLAY
                    { NIY("Usage is display"); }
        |           TOK_INDEX
                    { NIY("Usage is index"); }
        |           TOK_PACKED_DECIMAL
                    { NIY("Usage is packed-decimal"); }
        ;

value_entry:	TOK_VALUE optional_is value
	|	{ Push (SE_Null); }
	;
value:		integer
	|	float
	|	string
	|	identifier
	;
	
working_storage_section:	TOK_WORKING_STORAGE TOK_SECTION TOK_PERIOD
				record_entry_block
	|
	;
record_entry_block:	record_level record_entry_block_pl
	;
record_entry_block_pl:	record_entry_block
	|
	;

linkage_section:	TOK_LINKAGE TOK_SECTION TOK_PERIOD
				record_entry_block
	|
	;

communication_section:  TOK_COMMUNICATION TOK_SECTION TOK_PERIOD
                          { NIY("Communication Section"); }
        |
        ;

report_section:         TOK_REPORT TOK_SECTION TOK_PERIOD
                          { NIY("Report Section"); }
        |
        ;

procedure_division:	TOK_PROCEDURE TOK_DIVISION using_option TOK_PERIOD
			{ StartCode(); }
			statement_list
			{ GenEndProc(); EndCode(); }
	;

using_option:	TOK_USING using_identifier
	|
	;

statement_list:		statement statement_list_pl
	;
statement_list_pl:	statement_list
	|
	;

statement:	clause TOK_PERIOD
	|	{ GenEndProc(); }
		TOK_PROCEDURE identifier TOK_PERIOD
		{ GenStartProc(); }
	|	TOK_INITIALIZE initialize_args { NIY("Initialize"); }
        |       TOK_REPLACE TOK_OFF { NIY("Replace off"); } TOK_PERIOD
	|	TOK_READ identifier { GenRead(); }
		optional_word_record TOK_AT TOK_END at_end_clause TOK_PERIOD
	|	identifier TOK_PERIOD { GenParagraph(); }
	;

else_list:	TOK_ELSE { GenElse(); } compound_clause
	|	elsif_list
	|
	; 
elsif_list:	TOK_ELSIF elsif_clause elsif_list_pl
	;
elsif_list_pl:	elsif_list
	|
	;
elsif_clause:	{ GenStartElsif(); } boolean_list { GenEndIf(); } 
			compound_clause
	;

boolean_list:	boolean boolean_list_pl
	;

boolean_list_pl:	logic_connector { GenConnect(); } boolean_list
	|
	;

boolean:	optional_lparen expression optional_is boolean2 
		optional_rparen { GenBool(); }
        ;

boolean2:           TOK_ALPHABETIC
                 { strcpy (StringBuffer, "Alphabetic"); Push (SE_Bool); }
        |           TOK_ALPHABETIC_UPPER
                 { strcpy (StringBuffer, "Alphabetic-Upper"); Push (SE_Bool); }
        |           TOK_ALPHABETIC_LOWER
                 { strcpy (StringBuffer, "Alphabetic-Lower"); Push (SE_Bool); }
        |           relational
                      expression
        ;
	
not_boolean_list:	not_boolean not_boolean_list_pl
	;

not_boolean_list_pl:	logic_connector { GenConnect(); } not_boolean_list
	|
	;

not_boolean:	optional_lparen expression optional_is not_boolean2 
		optional_rparen { GenBool(); }
        ;

not_boolean2:	reverse_relational expression
        ;
                    

logic_connector:	TOK_AND
			{ strcpy (StringBuffer, "&&"); Push (SE_Connector); }
	|		TOK_OR
			{ strcpy (StringBuffer, "||"); Push (SE_Connector); }
	;

relational:	TOK_GREATER optional_than
		{ strcpy (StringBuffer, ">"); Push (SE_Bool); }
	|	TOK_LESS optional_than	
		{ strcpy (StringBuffer, "<"); Push (SE_Bool); }
	|	TOK_EQUAL optional_to
		{ strcpy (StringBuffer, "=="); Push (SE_Bool); }
        |       TOK_GREATER_EQ
                { strcpy (StringBuffer, ">="); Push (SE_Bool); }
        |       TOK_LESS_EQ 
                { strcpy (StringBuffer, "<="); Push (SE_Bool); }
	|	TOK_NOT reverse_relational
	;

reverse_relational:	TOK_GREATER optional_than
		{ strcpy (StringBuffer, "<="); Push (SE_Bool); }
	|	TOK_LESS optional_than
		{ strcpy (StringBuffer, ">="); Push (SE_Bool); }
	|	TOK_EQUAL optional_to
		{ strcpy (StringBuffer, "!="); Push (SE_Bool); }
        |       TOK_GREATER_EQ 
                { strcpy (StringBuffer, "<"); Push (SE_Bool); }
        |       TOK_LESS_EQ 
                { strcpy (StringBuffer, ">"); Push (SE_Bool); }
	;

clause:		TOK_ACCEPT id_list accept_option { GenAccept(); }
	|	TOK_DISPLAY display_args upon_option { GenDisplay(); }
	|	TOK_MOVE expression TOK_TO id_list { GenMove(); }
	|	TOK_ADD expression TOK_TO id_list giving_option
		{ GenAdd(); }
	|	TOK_SUBTRACT expression TOK_FROM id_list giving_option
		{ GenSubtract(); }
	|	TOK_MULTIPLY id_list TOK_BY expression giving_option
		{ GenMultiply(); }
	|	TOK_DIVIDE id_list divide_action_word expression 
		giving_option round_option
		{ GenDivide(); }
		size_error_option
	|	TOK_COMPUTE { Push (SE_Mark); } 
		identifier round_option TOK_EQUAL 
		{ opkind = OP_Equal; Push (SE_Operator); }
		compute_expr_list
		{ GenCompute(); }
	|	TOK_GO TOK_TO identifier { GenGoto(); } 
	|	TOK_PERFORM identifier perform_options { GenPerform(); }
	|	TOK_OPEN open_list
	|	TOK_CLOSE id_list close_options { GenClose(); }
	|	TOK_WRITE identifier write_from_clause { GenWrite(); }
        |       TOK_CALL call_list using_options { NIY("CALL"); }
	|	TOK_STOP TOK_RUN { GenStopRun(); }
        |       TOK_EXIT TOK_PROGRAM { GenStopRun(); }
	|	TOK_IF { GenStartIf(); } boolean_list { GenEndIf(); } if_args
	;

at_end_clause:	compound_clause
	|	{ GenEmptyClause(); }
	;

compound_clause:	{ BeginCompound(); }
			clause_list
			{ EndCompound(); }
	;
clause_list:	clause clause_list_pl
	;
clause_list_pl:	clause_list
	|
	;

expression:	identifier
	|	integer
	|	float
	|	string
	;

accept_option:  TOK_FROM from_arg
        | 	{ SetAcceptSource (AS_Console); }
        ;

from_arg:       TOK_DATE { SetAcceptSource (AS_Date); }
        |       TOK_DAY { SetAcceptSource (AS_Day); }
        |       TOK_DAY_OF_WEEK { SetAcceptSource (AS_Weekday); }
        |       TOK_TIME { SetAcceptSource (AS_Time); }
	|	TOK_CONSOLE { SetAcceptSource (AS_Console); }
        ;

display_args:	{ Push (SE_Mark); } optional_all display_args_marked
	;

display_args_marked:	display_arg display_args_pl
	;
display_args_pl: optional_comma	display_args_marked
	|
	;
display_arg:	identifier
	|	string
	|	integer
	|	float
        |       TOK_QUOTE { Push (SE_Quote); }
	;

upon_option:	TOK_UPON identifier { SetDisplayOutput(); }
	|	{ strcpy (StringBuffer, "stdout"); 
		  Push (SE_Identifier);
		  SetDisplayOutput(); 
		}
	;

giving_option:	TOK_GIVING identifier
	|	{ Push (SE_Null); }
	;

round_option:	TOK_ROUNDED { SetResultRounding(); }
	|
	;

size_error_option:	TOK_ON TOK_SIZE TOK_ERROR 
			{ GenElse(); }
			compound_clause
	|
	;

perform_options:	times_option varying_option
	;
times_option:	integer TOK_TIMES
	|	{ ival = 1; 
		  Push (SE_Integer);
		}
	;
varying_option:		TOK_VARYING loop_condition after_list
	|
	;
after_list:	TOK_AFTER loop_condition after_list
	|
	;
loop_condition:	{ GenStartLoop(); }
		identifier 
		TOK_FROM loop_iterator
		{ GenLoopInit(); }
		loop_condition_part2
	;
loop_condition_part2: 	TOK_TO loop_iterator
			{ GenLoopCondition(); }
			by_option
			{ GenLoopIncrement(); }
	|	by_option
		loop_condition_part3
		{ GenLoopIncrement(); }
	;
loop_condition_part3: 	TOK_WHILE boolean_list
	|		TOK_UNTIL not_boolean_list
	;
by_option:	TOK_BY loop_iterator
	|	{ ival = 1; 	/* 1 is the default increment */
		  Push (SE_Integer);
		}
	;
loop_iterator:	identifier
	|	integer
	;

compute_expr_list:	compute_expr compute_expr_list_pl
	|		compute_term
	;
compute_expr_list_pl:	compute_expr_list
	|
	;
compute_expr:	compute_term compute_operator
	|	compute_operator
	;
compute_term:	identifier
	|	integer
	|	float
	;
compute_operator:	TOK_PLUS	
			{ opkind = OP_Addition; Push (SE_Operator); }
	|		TOK_MINUS
			{ opkind = OP_Subtraction; Push (SE_Operator); }
	|		TOK_ASTERISK
			{ opkind = OP_Multiplication; Push (SE_Operator); }
	|		TOK_SLASH
			{ opkind = OP_Division; Push (SE_Operator); }
	|		TOK_LPAREN
			{ opkind = OP_LParen; Push (SE_Operator); }
	|		TOK_RPAREN
			{ opkind = OP_RParen; Push (SE_Operator); }
	;

open_list:	open_entry open_list_pl
	;
open_list_pl:	open_list
	|
	;
open_entry:	TOK_INPUT id_list open_options { GenOpen ("input"); }
	|	TOK_OUTPUT id_list open_options { GenOpen ("output"); }
	|	TOK_EXTEND id_list open_options { GenOpen ("extend"); }
	|	TOK_I_O id_list open_options { GenOpen ("io"); }
	;

open_options:	TOK_REVERSED
	|	TOK_WITH TOK_NO TOK_REWIND
	|
	;

close_options:	TOK_UNIT close_method
	|	TOK_REEL close_method
	|	TOK_WITH TOK_LOCK
	|
	;

close_method:	TOK_FOR TOK_REMOVAL
	|	TOK_WITH TOK_NO TOK_REWIND
	;

write_from_clause:	TOK_FROM identifier
	|	{ strcpy (StringBuffer, ""); 
		  Push (SE_Identifier); }
	;

call_list:     string
         |     identifier
         ;

using_options: TOK_USING using_args using_identifier
        | 
        ;

using_args:    optional_by using_by
        |
        ;

using_by:      TOK_REFERENCE
        |      TOK_CONTENT
        ;

using_identifier:   identifier using_inde_next
        ;

using_inde_next:    TOK_COMMA using_identifier
        |
        ;

if_args:	optional_then if_args_2
	;
if_args_2:	compound_clause else_list
	;

initialize_args:	using_identifier replace_option
	;

replace_option:		TOK_REPLACING replace_next
	|
	;

replace_next:		replace_what optional_data TOK_BY value replace_next_pl
        ;

replace_next_pl:	optional_comma replace_next	
        |
	;

replace_what:	TOK_ALPHABETIC
	|	TOK_ALPHANUMERIC
	|	TOK_NUMERIC
	|	TOK_ALPHANUMERIC_EDITED
	|	TOK_NUMERIC_EDITED
	;

id_list:	{ Push (SE_Mark); } id_list_marked
	;
id_list_marked: identifier id_list_pl
	;
id_list_pl:	optional_comma id_list_marked
	|
	;

divide_action_word:	TOK_BY
	|		TOK_INTO
	;

optional_is:		TOK_IS
	|
	;
optional_comma:		TOK_COMMA
	|
	;
optional_word_record:	TOK_RECORD
	|
	;
optional_to:            TOK_TO
        |
        ;
optional_all:           TOK_ALL
        |
        ;
optional_than:          TOK_THAN
        |
        ;
optional_then:          TOK_THEN
        |
        ;
optional_optional:      TOK_OPTIONAL
        |
        ;
optional_by:            TOK_BY
        |
        ;
optional_mode:          TOK_MODE
        |
        ;
optional_file:          TOK_FILE
        |
        ;
optional_key:           TOK_KEY
        |
        ;
optional_sign:		TOK_SIGN
	|
	;
optional_character:	TOK_CHARACTER
        |
        ;
optional_size:		TOK_SIZE
	|
	;
optional_program:	TOK_PROGRAM
	|
	;
optional_with:		TOK_WITH
	|
	;
optional_right:		TOK_RIGHT
	|
	;
optional_data:		TOK_DATA
	|
	;
optional_lparen:	TOK_LPAREN
	|
	;
optional_rparen:	TOK_RPAREN
	|
	;

identifier:	TOK_IDENTIFIER { Push (SE_Identifier); }
	;
pic_text:	TOK_PIC_TEXT { Push (SE_Picture); }
	;
string:		TOK_STRING { Push (SE_String); }
	;
integer:	TOK_INTEGER { Push (SE_Integer); }
	;
float:		TOK_FLOAT { Push (SE_Float); }
	;
%%

int yyerror (char * msg)
{
char ErrorBuffer [80];

    sprintf (ErrorBuffer, "Line %d: %s\n\tDEBUG: yychar = %d, str = %s, ival = %ld, fval = %f\n", 
    		CurrentLine, msg, yychar, StringBuffer, ival, fval);
    WriteError (ErrorBuffer);
    return (0);
}

