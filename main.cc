// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

//----------------------------------------------------------------------

extern FILE* yyin;
extern int yyparse (void);
CobcyConfigType g_Config;

//----------------------------------------------------------------------

class Cobcy {
public:
    static Cobcy&	Instance (void)	{ static Cobcy s_App; return s_App; }
    void		ProcessArgs (int argc, char* const* argv);
    int			Run (void);
private:
			Cobcy (void) = default;
    static void		Usage (void);
};

//----------------------------------------------------------------------

/*static*/ void Cobcy::Usage (void)
{
    cout << "Cobol to C compiler " COBCY_VERSTRING ", Copyright (c) 1996 by " COBCY_BUGREPORT
	    "\nUsage:\n"
	    "\t" COBCY_NAME " [options] <file.cob>\n"
	    "\n"
	    "\tOptions:\n"
	    "\t-g\t\tGenerate compiler debugging info (cpi trace)\n"
	    "\t-o <file.c>\tFile where to put the C code\n"
	    "\t\t\t<file.c>.h will also be made with decls.\n"
	    "\t\t\tIf this is not specified, Cobcy uses\n"
	    "\t\t\t<file.cob>.c and <file.cob>.h\n"
	    "\n";
}

void Cobcy::ProcessArgs (int argc, char* const* argv)
{
    bool outputSet = false;
    for (int o; -1 != (o = getopt (argc, argv, "go:"));) {
	if (o == 'g')
	    g_Config.GenDebug = true;
	else if (o == 'o') {
	    g_Config.CodeFile = optarg;
	    outputSet = true;
	}
    }
    if (optind >= argc) {
	Usage();
	return;
    }
    g_Config.SourceFile = argv[optind];
    if (!outputSet)
	g_Config.CodeFile = g_Config.SourceFile + ".c";
}

int Cobcy::Run (void)
{
    if (!g_Config.SourceFile[0]) {
	cerr << "Error: no COBOL source specified\n";
	return EXIT_FAILURE;
    }
    DTRACE ("Compiling %s into %s\n", g_Config.SourceFile.c_str(), g_Config.CodeFile.c_str());
    if (!(yyin = fopen (g_Config.SourceFile, "r"))) {
	cerr.format ("Error: could not open source file \"%s\"\n", g_Config.SourceFile.c_str());
	return EXIT_FAILURE;
    } else
	DTRACE ("Compiling %s ...\n", g_Config.SourceFile.c_str());
    yyparse();
    fclose (yyin);
    return ErrorOccured() ? EXIT_FAILURE : EXIT_SUCCESS;
}

//----------------------------------------------------------------------
//{{{ Process cleanup handlers

static void OnSignal (int sig) noexcept
{
    static atomic_flag doubleSignal = ATOMIC_FLAG_INIT;
    bool bFirst = !doubleSignal.test_and_set();
    WriteError (strsignal(sig));
    #ifndef NDEBUG
	if (bFirst)
	    try { CBacktrace bt; cerr << bt; } catch (...) {}
    #endif
    enum { qc_ShellSignalQuitOffset = 128 };
    auto exitCode = sig + qc_ShellSignalQuitOffset;
    if (bFirst)
	exit (exitCode);
    _Exit (exitCode);
}

/// Called by the framework on unrecoverable exception handling errors.
static void OnTerminate (void) noexcept
{
    assert (!"Unrecoverable exception handling error detected.");
    WriteError ("unrecoverable exception handling error");
    raise (SIGABRT);
    exit (EXIT_FAILURE);
}

/// Called when an exception violates a throw specification.
static void OnUnexpected (void) noexcept
{
    WriteError ("unexpected exception caught");
    OnTerminate();
}

/// Installs OnSignal as handler for signals.
static void InstallCleanupHandlers (void) noexcept
{
    static const uint8_t c_Signals[] = {
	SIGINT, SIGQUIT, SIGILL,  SIGTRAP, SIGABRT,
	SIGIOT, SIGBUS,  SIGFPE,  SIGSEGV, SIGTERM,
	SIGIO,  SIGCHLD
    };
    for (auto i = 0u; i < VectorSize(c_Signals); ++i)
	signal (c_Signals[i], OnSignal);
    std::set_terminate (OnTerminate);
    std::set_unexpected (OnUnexpected);
}

//}}}-------------------------------------------------------------------
//{{{ main

int main (int argc, char* const* argv)
{
    int ec = EXIT_FAILURE;
    InstallCleanupHandlers();
    try {
	auto& app (Cobcy::Instance());
	app.ProcessArgs (argc, argv);
	ec = app.Run();
    } catch (exception& e) {
	cerr.format ("Error: %s\n", e.what());
	#ifndef NDEBUG
	    cerr << e.backtrace();
	#endif
    }
    return ec;
}

//}}}-------------------------------------------------------------------
