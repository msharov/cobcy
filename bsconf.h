/* This file is part of bsconf - a configure replacement.
 *
 * This is the configuration file used by bsconf.c to specify information
 * specific to your project that it needs to substitute into files listed
 * in g_Files. Being a configuration file, this file can be used or
 * modified entirely without restriction. You should change all values
 * appropriately to the name of your project and its requirements. The
 * bsconf license does not apply to this file. It can and should be
 * treated as a template for the creation of your own configuration file.
 *
 * All substituted variable names are given without enclosing @@. For
 * example: "CC" will match "@CC@" in config.h.in and replace it with
 * "gcc" in config.h.
*/

#define PACKAGE_NAME		"cobcy"
#define PACKAGE_MAJOR		"0"
#define PACKAGE_MINOR		"5"
#define PACKAGE_BUILD		"0"

#define PACKAGE_VERSION		PACKAGE_MAJOR "." PACKAGE_MINOR
#define PACKAGE_TARNAME		PACKAGE_NAME
#define PACKAGE_STRING		PACKAGE_NAME " " PACKAGE_VERSION
#define PACKAGE_BUGREPORT	"Mike Sharov <msharov@users.sourceforge.net>"

static cpchar_t g_Files [] = {
    "Config.mk",
    "config.h"
};

/* Values substitute @VARNAME@ */
static cpchar_t g_EnvVars [] = {
    "CC",
    "CXX"
};

/*  VARIABLE	PROGRAM		HOW TO CALL	IF NOT FOUND */
static cpchar_t g_ProgVars [] = {
    "CC",	"gcc",		"gcc",		"@CC@",
    "CC",	"cc",		"cc",		"gcc",
    "CXX",	"g++",		"g++",		"@CXX@",
    "CXX",	"c++",		"c++",		"g++",
    "AR",	"ar",		"ar",		"ar",
    "RANLIB",	"ranlib",	"ranlib",	"touch",
    "LEX",	"flex",		"flex",		"@LEX@",
    "LEX",	"lex",		"lex",		"lex",
    "YACC",	"bison",	"bison -y",	"@YACC@",
    "YACC",	"yacc",		"yacc",		"yacc",
    "INSTALL",	"install",	"install -c",	"cp"
};

/*   NAME		IF NOT FOUND			IF FOUND */
static cpchar_t	g_Headers [] = {
    "ustl.h",		"#undef HAVE_USTL_H",		"#define HAVE_USTL_H 1"
};

/*   NAME	IF NOT FOUND	IF FOUND */
static cpchar_t g_Libs [] = {
    "ustl",	"",		"-lustl",
    "fl",	"",		"-lfl"
};

/*   NAME	IF NOT FOUND	IF FOUND */
static cpchar_t g_Functions [] = {
};

/*   NAME	WITHOUT TEXT		WITH TEXT */
static cpchar_t g_Components [] = {
    "debug",	"#DEBUG\t\t= 1",	"DEBUG\t\t= 1 "
};

/* Parallel to g_Components */
static SComponentInfo g_ComponentInfos [VectorSize(g_Components) / 3] = {
    { 0, "Compiles the executable with debugging information" }
};

/* Substitutes names like @PACKAGE_NAME@ with the second field */
static cpchar_t g_CustomVars [] = {
    "PACKAGE_NAME",		PACKAGE_NAME,
    "PACKAGE_VERSION",		PACKAGE_VERSION,
    "PACKAGE_MAJOR",		PACKAGE_MAJOR,
    "PACKAGE_MINOR",		PACKAGE_MINOR,
    "PACKAGE_BUILD",		PACKAGE_BUILD,
    "PACKAGE_TARNAME",		PACKAGE_TARNAME,
    "PACKAGE_STRING",		PACKAGE_STRING,
    "PACKAGE_BUGREPORT",	PACKAGE_BUGREPORT,
    "GCC4_SIZEOPTS",
#if __GNUC__ >= 4
				"-fno-threadsafe-statics -fno-enforce-eh-specs",
#else
				"",
#endif
};

#define BSCONF_VERSION		0x03

