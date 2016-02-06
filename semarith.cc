// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (c) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "semextern.h"
#include "symvar.h"
#include "symrec.h"
#include "symconst.h"
#include <stdio.h>

bool RoundResult = false;

void GenMove (void)
{
    DTRACE("\tIn GenMove\n");
    auto edest = PopStatement();	// destinations first, source last
    if (edest.empty())
	return;
    reverse (edest);
    auto esrc = PopIdentifier();

    for (auto& d : edest) {
	auto dest = LookupIdentifier<CobolVar> (d.ident);
	if (!dest)
	    return WriteError ("unknown destination %s", d.ident.c_str());
	if (esrc.kind == SE_Identifier) {
	    auto src = LookupIdentifier<CobolVar> (esrc.ident);
	    if (!src)
		return WriteError ("unknown source %s", esrc.ident.c_str());
	    DTRACE ("\t\tMoving %s to %s\n", src->GetFullCName().c_str(), dest->GetFullCName().c_str());
	    dest->GenMove (codef, src);
	} else {
	    CobolConstant csrc = esrc;
	    #ifndef NDEBUG
		if (g_Config.GenDebug)
		    cerr << "\t\tMoving " << csrc << " to " << *dest << '\n';
	    #endif
	    dest->GenMove (codef, csrc);
	}
    }
}

void SetResultRounding (void)
{
    RoundResult = true;
}

static void GenericArithmetic (bool sourceFirst, char opChar)
{
    auto egiving = PopIdentifier();
    vector<StackEntry> edest;
    StackEntry esrc;
    if (sourceFirst) {
	esrc = PopIdentifier();
	edest = PopStatement();
    } else {
	edest = PopStatement();
	esrc = PopIdentifier();
    }
    const CobolVar* giving = nullptr;
    if (egiving.kind == SE_Identifier)
	giving = LookupIdentifier<CobolVar> (egiving.ident);
    for (auto& d : edest) {
	auto dest = LookupIdentifier<CobolVar> (d.ident);
	if (!dest)
	    return WriteError ("unknown destination %s", d.ident.c_str());
	CobolConstant csrc = esrc;
	const CobolSymbol* src = &csrc;
	if (esrc.kind == SE_Identifier) {
	    src = LookupIdentifier<CobolVar> (esrc.ident);
	    if (!src)
		return WriteError ("unknown source %s", esrc.ident.c_str());
	}
	(giving ? giving : dest)->GenArith (codef, dest, src, opChar);
    }
    RoundResult = false;
}

void GenAdd (void)
{
    DTRACE ("\tIn GenAdd\n");
    GenericArithmetic (false, '+');
}

void GenSubtract (void)
{
    DTRACE ("\tIn GenSubtract\n");
    GenericArithmetic (false, '-');
}

void GenMultiply (void)
{
    DTRACE ("\tIn GenMultiply\n");
    GenericArithmetic (true, '*');
}

void GenDivide (void)
{
    DTRACE ("\tIn GenDivide\n");
    GenericArithmetic (true, '/');
}

void GenCompute (void)
{
    DTRACE ("\tIn GenCompute\n");

    auto prms = PopStatement();
    reverse (prms.begin(), prms.end());

    GenIndent();
    for (auto& p : prms) {
	switch (p.kind) {
	    case SE_Identifier:
		// i > 0 because we only want casting on the righthand side
		//	entry 0 is the variable where stuff will be placed
		if (RoundResult && &p != prms.begin())
		    codef << "(double)";
		PrintIdentifier (p.ident, codef);
		break;
	    case SE_Operator:
		switch (p.opkind) {
		    case OP_Addition:		codef << " + "; break;
		    case OP_Subtraction:	codef << " - "; break;
		    case OP_Multiplication:	codef << " * "; break;
		    case OP_Division:		codef << " / "; break;
		    case OP_LParen:		codef << "(";	break;
		    case OP_RParen:		codef << ")";	break;
		    case OP_Equal:
			codef << " = ";
			if (RoundResult)
			    codef << "_RoundResult (";
			break;
		}
		break;
	    case SE_Integer:
	    case SE_Float:
		PrintConstant (p, codef);
		if (RoundResult && p.kind == SE_Integer)
		    codef << ".0";
		break;
	    default:
		WriteError ("Invalid expression in COMPUTE");
		break;
	}
    }

    // The whole expression will be on one line...
    if (RoundResult) {
	codef << ", ";
	auto dest = LookupIdentifier<CobolVar> (prms[0].ident);
	if (dest) {
	    codef << '\"';
	    dest->WritePicture (codef);
	    codef << '\"';
	}
	codef << ")";
	RoundResult = false;
    }
    codef << ";\n";
}
