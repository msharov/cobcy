// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "symlabel.h"
#include "semutil.h"
#include "semcontrol.h"

CobolLabel::CobolLabel (void)
: Undeclared (true)
{
}

void CobolLabel::GenPrototype (ostringstream& os) const
{
    os << "static int " << *this << " (void);\n";
}

void CobolLabel::GenDeclare (ostringstream& os) const
{
    os << "static int " << *this << " (void)\n";
    BeginCompound();
    os << *this << ": UNUSED\n";
}

void CobolLabel::GenJump (ostringstream& os, const CobolLabel* dest) const
{
    GenIndent();
    // If in jumping to the top of the current paragraph, use goto.
    if (dest == this)
	os << "goto " << *this << ";\n";
    else {
	// Return the difference of indeces of the destination paragraph
	//	and the current paragraph. This is done so that when
	//	that paragraph is through, and we return here, we could
	//	tell the main routine how many paragraphs to advance.
	os << "return ";
	os << "_pi_" << *dest << " - ";
	os << "_pi_" << *this << ";\n";
    }
}
