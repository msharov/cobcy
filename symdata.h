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
    virtual		~CobolData (void) override;
    inline void		SetDeclLevel (uint32_t NewLevel)	{ DeclLevel = NewLevel; }
    inline auto		GetDeclLevel (void)			{ return DeclLevel; }
    void		AssociateWithStream (CobolFile* NewStream);
    CobolFile*		GetStream (void);
    virtual void	GenRead (ostringstream&, const char*)	{ assert (!"This data type is not readable"); };
    virtual void	GenWrite (ostringstream&, const char*)	{ assert (!"This data type is not writable"); };
    virtual void	GenSignature (ostringstream&)		{ assert (!"This data type has no signature"); };
    void		GenRead (ostringstream& os);
    void		GenWrite (ostringstream& os);
    inline uint32_t	GetSize (void)				{ return CSize; }
protected:
    uint32_t		CSize;
    uint32_t		DeclLevel;			// >01< ID IS RECORD
    CobolFile*		AssociatedStream;
};
