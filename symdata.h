// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMDATA_H_73C9E3D93146C9CF2676EF61361A60DF
#define SYMDATA_H_73C9E3D93146C9CF2676EF61361A60DF

#include "symbase.h"

class CobolFile;	// They are cross-linked
/// Defines an abstract data thing.
class CobolData : public CobolSymbol {
public:
			CobolData (void);
			~CobolData (void);
    inline virtual void	SetDeclLevel (uint32_t NewLevel)	{ DeclLevel = NewLevel; }
    inline virtual uint32_t	GetDeclLevel (void)		{ return (DeclLevel); }
    void		AssociateWithStream (CobolFile * NewStream);
    CobolFile*		GetStream (void);
    virtual void	GenRead (ostringstream&, const char*)	{ assert (!"This data type is not readable"); };
    virtual void	GenWrite (ostringstream&, const char*)	{ assert (!"This data type is not writable"); };
    virtual void	GenSignature (ostringstream&)		{ assert (!"This data type has no signature"); };
    void		GenRead (ostringstream& os);
    void		GenWrite (ostringstream& os);
    inline uint32_t	GetSize (void)				{ return (CSize); }
protected:
    uint32_t		CSize;
    uint32_t		DeclLevel;			// >01< ID IS RECORD
    CobolFile*		AssociatedStream;
};

#endif
