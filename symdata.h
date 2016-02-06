// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "symbase.h"

class CobolFile;	// They are cross-linked
/// Defines an abstract data thing.
class CobolData : public CobolSymbol {
public:
			CobolData (void);
    inline void		SetDeclLevel (uint32_t NewLevel)	{ DeclLevel = NewLevel; }
    inline auto		GetDeclLevel (void) const		{ return DeclLevel; }
    void		AssociateWithStream (const CobolFile* NewStream);
    auto		GetStream (void) const			{ return AssociatedStream; }
    virtual void	GenRead (ostringstream&, const char*) const	{ assert (!"This data type is not readable"); };
    virtual void	GenWrite (ostringstream&, const char*) const	{ assert (!"This data type is not writable"); };
    virtual void	GenSignature (ostringstream&) const		{ assert (!"This data type has no signature"); };
    void		GenRead (ostringstream& os) const;
    void		GenWrite (ostringstream& os) const;
    inline uint32_t	GetSize (void) const			{ return CSize; }
protected:
    uint32_t		CSize;
    uint32_t		DeclLevel;			// >01< ID IS RECORD
    const CobolFile*	AssociatedStream;
};
