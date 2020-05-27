// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#define _GNU_SOURCE
#include "cobfunc.h"
#include <ctype.h>
#include <signal.h>
#include <stdarg.h>

char _space_var [201] = "                                                                                                                                                                                                        ";
long _zero_var = 0;
long _index = 0;

static char _strbuf[201];		// For ---toString routines. This is the buffer

void* _AllocateBytes (size_t n)
{
    void* p = calloc (1, n);
    if (!p)
	_RuntimeError ("out of memory");
    return p;
}

void* _ReallocateBytes (void* p, size_t n)
{
    p = realloc (p, n);
    if (!p)
	_RuntimeError ("out of memory");
    return p;
}

#define S(s) (1<<(s))
enum {
    sigset_Quit	= S(SIGINT)|S(SIGQUIT)|S(SIGTERM),
    sigset_Die	= S(SIGILL)|S(SIGABRT)|S(SIGBUS)|S(SIGFPE)
		|S(SIGSYS)|S(SIGSEGV)|S(SIGALRM)|S(SIGXCPU)
};
enum { qc_ShellSignalQuitOffset = 128 };

static void __OnFatalSignal (int sig)
{
    static volatile bool s_bOnce = false;
    if (!s_bOnce) {
	s_bOnce = true;
	psignal (sig, "[S] Fatal error");
	// The exit code should be success when terminated by user
	int exitcode = EXIT_SUCCESS;
	if (!(S(sig) & sigset_Quit))
	    exitcode = qc_ShellSignalQuitOffset+sig;
	exit (exitcode);
    }
    _Exit (qc_ShellSignalQuitOffset+sig);
}

void _InstallSignalHandlers (void)
{
    for (unsigned sig = 0; sig < NSIG; ++sig)
	if ((sigset_Quit|sigset_Die) & S(sig))
	    signal (sig, __OnFatalSignal);
}
#undef S

bool _Alphabetic (const char* s)
{
    if (!s)
	return false;
    for (; *s; ++s)
	if (!isalpha(*s) && !isspace(*s))
	    return false;
    return true;
}

/// Returns 1 for lower case, 2 for upper.
bool _AlphabeticCase (const char* str, int what)
{
    if (!_Alphabetic(str))
	return false;
    for (size_t i = 0; i < strlen(str); i++) {
	if (what == 2 && islower(str[i]))
	    return false;
	else if (what == 1 && isupper(str[i]))
	    return false;
    }
    return true;
}

void _RuntimeError (const char* message, ...)
{
    fflush (stdout);
    fprintf (stderr, "\nRuntime error:");
    va_list args;
    va_start (args, message);
    vfprintf (stderr, message, args);
    va_end (args);
    fprintf (stderr, "!\n\n");
    exit (EXIT_FAILURE);
}

void _ReadStringVar (FILE* stream, char* var, const char* pic)
{
    size_t sl = 0, pi, pl = strlen(pic);
    // Find length of the string by excluding editing characters from pic
    for (pi = 0; pi < pl; ++pi)
	if (!strchr(".,/bB0",pic[pi]))
	    ++sl;

    memset(var, ' ', sl);	// All string variables are spaces by default
    var[sl+1] = 0;
    size_t si = 0;		// Read characters until a count of chars is reached

    // Iterate over picture skipping all edited positions
    for (pi = 0; pi < pl && si < sl; ++pi) {
	char c = fgetc(stream);
	if (c == '\n' || c == '\x0' || feof(stream))
	    break;
	if (!strchr(".,/bB0",pic[pi]))
	    var[si++] = c;
    }
}

void _ReadIntegerVar (FILE* stream, long int* var, const char* pic)
{
    char buffer[20];
    _ReadStringVar (stream, buffer, pic);
    *var = atol (buffer);
}

void _ReadFloatVar (FILE* stream, double* var, const char* pic)
{
    char buffer[20];
    _ReadStringVar (stream, buffer, pic);
    *var = atof (buffer);
}

void _WriteStringVar (FILE* stream, const char* var, const char* pic)
{
    if (!var || !pic)
	return;
    for (; *var && *pic; ++pic) {
	switch (*pic) {
	    case 'x':	fputc (*var++, stream);	break;
	    case 'b':	fputc (' ', stream);	break;
	    case '/':	fputc ('/', stream);	break;
	    case '0':	fputc ('0', stream);	break;
	    default:	fputc (*pic, stream);	break;
	}
    }
}

void _WriteIntegerVar (FILE* stream, long int var, const char* pic)
{
    fprintf (stream, "%s", _IntegerToString (var, pic));
}

void _WriteFloatVar (FILE* stream, double var, const char* pic)
{
    fprintf (stream, "%s", _FloatToString (var, pic));
}

const char* _IntegerToString (long int var, const char* pic)
{
    unsigned long uvar = var > 0 ? var : -var;
    char barenum [32];
    snprintf (barenum, sizeof(barenum), "%lu", uvar);	// printed unsigned; sign added below by picture
    const size_t nl = strlen(barenum), pl = strlen(pic);

    size_t j = 0, fl = 0;
    // Calculate number of filler zeroes on the left
    for (size_t i = 0; i < pl; ++i)
	if (pic[i] == '9' || pic[i] == 'z' || pic[i] == '*')
	    ++fl;

    // If the number does not fit, cut leftmost digits
    if (fl < nl)
	j = nl - fl;

    char* sp = _strbuf;
    for (size_t i = 0; i < pl; ++i) {
	if (strchr (".,/0$", pic[i]))
	    *sp++ = pic[i];
	else if (pic[i] == 'b')
	    *sp++ = ' ';
	else if (pic[i] == 'z' || pic[i] == '*')
	    *sp++ = '0';
	else if (pic[i] == '+' || pic[i] == 's' || pic[i] == '-') {
	    if (var > 0 && pic[i] == '-')
		*sp++ = '+';
	    else if (var < 0)
		*sp++ = '-';
	    else
		*sp++ = ' ';
	} else if (pic[i] == '9') {
	    if (fl <= nl)
		*sp++ = barenum[j++];
	    else
		*sp++ = ' ';
	    --fl;
	} else
	    *sp++ = (fl - 1 <= nl) ? pic[i] : ' ';
    }
    *sp = 0;
    return _strbuf;
}

const char* _FloatToString (double var, const char* pic)
{
    const size_t piclen = strlen(pic);

    // Calculate number of filler zeroes on the left
    unsigned short fbp = 0, fap = 0;
    bool FoundPoint = false;
    for (size_t i = 0; i < piclen; ++i) {
	if (pic[i] == '9' || pic[i] == '0' || pic[i] == 'z' || pic[i] == '*') {
	    if (FoundPoint)
		++fap;
	    else
		++fbp;
	}
	else if (pic[i] == 'v')
	    FoundPoint = true;
    }

    char format_s[16];
    snprintf (format_s, sizeof(format_s), "%%%hu.%huf", fbp, fap);
    char barenum[32];
    snprintf (barenum, sizeof(barenum), format_s, var);

    // sprintf will print correct number of digits after point, but
    // only as many as needed before the point. We thus need to reset
    // the index variable j to the real starting point.
    int j = strlen(barenum) - (fbp + 1 + fap);
    char* sp = _strbuf;

    for (size_t i = 0; i < piclen; ++i) {
	if (pic[i] == '9') {
	    *sp++ = (j >= 0 ? barenum[j] : ' ');
	    ++j;
	} else if (pic[i] == '.')
	    *sp++ = '.';
	else if (pic[i] == ',')
	    *sp++ = ',';
	else if (pic[i] == '/')
	    *sp++ = '/';
	else if (pic[i] == 'b')
	    *sp++ = ' ';
	else if (pic[i] == '0')
	    *sp++ = '0';
	else if (pic[i] == '+' || pic[i] == 's' || pic[i] == '-') {
	    if (var > 0 && pic[i] == '-')
		*sp++ = '+';
	    else if (var < 0)
		*sp++ = '-';
	    else
		*sp++ = ' ';
	} else if (pic[i] == 'z' || pic[i] == '*') {
	    *sp++ = '0';
	    ++j;
	} else if (pic[i] == 'v') {
	    *sp++ = '.';
	    ++j;
	} else if (pic[i] == '$')
	    *sp++ = '$';
	else
	    *sp++ = pic[i];
    }
    *sp = 0;
    return _strbuf;
}

void _AssignVarString (char* var1, const char* var2, int sz1, int sz2)
{
    if (sz2 == 0)
	sz2 = strlen(var2);
    memset (var1, ' ', sz1);
    var1[sz1] = '\x0';
    memcpy (var1, var2, min (sz1, sz2));
    if (sz1 > sz2)
	var1[sz2] = ' ';
}

double _RoundResult (double num, const char* pic UNUSED)
{
    return (long)(num + 0.5);
}

static unsigned _FieldsInSig (const char* sig)
{
    unsigned IsToken = 0, nTokens = 0;
    for (size_t i = 0; i < strlen(sig); ++i) {
	if (sig[i] == ' ')
	    IsToken = 0;
	else {
	    if (!IsToken)
		++nTokens;
	    IsToken = 1;
	}
    }
    return nTokens / 4; // Each field is defined by NAME TYPE SIZE DECIMAL
}

// The syntax is same as strcpy, only that the source pointer
// is moved to the first char after read token. That's what
// char** is for.
static void _TokenCopy (char* dest, const char** src)
{
    const char* sptr = *src;
    char* dptr = dest;
    char c;
    while ((c = *sptr) != '\x0' && c == ' ')
	++sptr;
    while ((c = *sptr) != '\x0' && c != ' ')
	*dptr++ = *sptr++;
    *dptr = '\x0';
    *src = sptr;
}

static DBF_Field* _SigToFields (const char* sig, unsigned* lpnFields)
{
    unsigned nFields = _FieldsInSig(sig);
    *lpnFields = nFields;

    DBF_Field* Fields = _AllocateBytes (nFields * sizeof(DBF_Field));
    const char* cursigpos = sig;
    for (unsigned i = 0; i < nFields; ++i) {
	char buffer[50];
	// First token is the name of the field
	_TokenCopy(buffer, &cursigpos);
	// Assuming the last character to be set to 0 by _AllocateBytes
	strncpy(Fields[i].Name, buffer, DBF_FIELD_NAME_LENGTH - 1);

	// Second token is the character denoting type of field
	_TokenCopy(buffer, &cursigpos);
	Fields[i].Type = buffer[0];

	// Third token is the size of the field
	_TokenCopy(buffer, &cursigpos);
	Fields[i].FieldLength = atoi(buffer);

	// Fourth token is the number of decimal places, if any
	_TokenCopy(buffer, &cursigpos);
	Fields[i].DecimalPlaces = atoi(buffer);
    }
    return Fields; // Fields is freed in DBF_Close
}

void _OpenSequentialFile (FILE** fp, const char* filename, const char* mode)
{
    if (!(*fp = fopen (filename, mode)))
	_RuntimeError ("could not open file '%s'", filename);
}

void _OpenRelativeFile (DBF_FILE** fp, const char* filename, const char* sig, const char* mode)
{
    // If opening for OUTPUT, remove the file first
    if (mode[0] == 'w' && mode[1] != '+' && access(filename, 0) == 0)
	unlink(filename);

    // If the file does not exist, use Create call
    if (access(filename, 0) != 0) {
	// Cannot open file for reading if it does not exist
	if (mode[0] == 'r')
	    _RuntimeError ("file '%s' does not exist", filename);
	unsigned nFields;
	DBF_Field* Fields = _SigToFields (sig, &nFields);
	if (!(*fp = DBF_Create (filename, nFields, Fields)))
	    _RuntimeError ("could not create data file '%s'", filename);
    } else if (!(*fp = DBF_Open (filename, mode)))
	_RuntimeError ("could not open file '%s'", filename);
}

void _OpenIndexedFile (DBF_FILE** fp, const char* filename, const char* sig, const char* mode)
{
    // If opening for OUTPUT, remove the file first
    if (mode[0] == 'w' && mode[1] != '+' && access(filename, 0) == 0)
	unlink(filename);

    // If the file does not exist, use Create call
    if (access(filename, 0) != 0) {
	// Cannot open file for reading if it does not exist
	if (mode[0] == 'r')
	    _RuntimeError ("file '%s' does not exist", filename);
	unsigned nFields;
	DBF_Field* Fields = _SigToFields (sig, &nFields);
	if (!(*fp = DBF_Create (filename, nFields, Fields)))
	    _RuntimeError ("could not create data file '%s'", filename);
    } else if ((*fp = DBF_Open (filename, mode)) == NULL)
	_RuntimeError ("could not open data file '%s'", filename);
}

void _CloseSequentialFile (FILE * fp)
{
    fclose(fp);
}

void _CloseRelativeFile (DBF_FILE** fp)
{
    DBF_Close (*fp);
    *fp = NULL;
}

void _CloseIndexedFile (DBF_FILE** fpd)
{
    DBF_Close (*fpd);
    *fpd = NULL;
}

void _SeekRelativeFile (DBF_FILE* fp, long int record)
{
    DBF_SeekToRecord (fp, record);
}

void _SeekIndexedFile (DBF_FILE* fp, NDX_FILE* fpd, const char* key)
{
    int index = NDX_LookupKey (fpd, key);
    if (index >= 0)
	DBF_SeekToRecord(fp, index);
}

void _OpenIndexFile (NDX_FILE** ifd, const char* filename, const char* sig, const char* mode)
{
    if (mode[0] == 'w' && access (filename,0) == 0)
	unlink (filename);

    // If the file does not exist, use Create call
    if (access(filename, 0) != 0) {
	// Cannot open file for reading if it does not exist
	if (mode[0] == 'r')
	    _RuntimeError ("file '%s' does not exist", filename);
	unsigned nFields;
	DBF_Field* Fields = _SigToFields (sig, &nFields);
	if (!(*ifd = NDX_Create (filename, Fields->Name, Fields->Type, Fields->FieldLength)))
	    _RuntimeError ("could not create data file '%s'", filename);
	free (Fields);
    } else if ((*ifd = NDX_Open(filename, mode)) == NULL)
	_RuntimeError ("could not open data file '%s'", filename);
}

void _CloseIndexFile (NDX_FILE** ifd)
{
    NDX_Close (ifd);
}
