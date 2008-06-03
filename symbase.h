// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#ifndef SYMBASE_H_64F2F952279411DC249C520D6596F0D6
#define SYMBASE_H_64F2F952279411DC249C520D6596F0D6

#include "config.h"

#define MAX_SYMBOL_LENGTH	50
#define MAX_PREFIX_LENGTH	80
#define MAX_FULLNAME_LENGTH	(MAX_SYMBOL_LENGTH + MAX_PREFIX_LENGTH)

typedef enum {
    CS_Undefined,
    CS_Record, 
    CS_Variable,
    CS_Procedure,
    CS_FileDesc,
    CS_Label,
    CS_Constant,
    CS_Picture
} CobolSymbolType;

/// Defines an abstract symbol table entry.
class CobolSymbol {
private:
    char		ParentCobolName [MAX_SYMBOL_LENGTH];
    char		Prefix [MAX_PREFIX_LENGTH];
    char		CName [MAX_SYMBOL_LENGTH];	   // C name
    char		FullCName [MAX_FULLNAME_LENGTH];   // C name with prefix
    char		CobolName [MAX_SYMBOL_LENGTH];	   // Cobol name

protected:
    void		CobolToCName (char* str);

public:
				CobolSymbol (void);
    void			SetName (const char* NewName);
    inline const char*		GetName (void);
    inline const char*		GetCName (void);
    inline const char*		GetFullCName (void);
    void			SetParent (const char* NewParent);
    virtual CobolSymbolType	Kind (void) = 0;
    virtual void		text_write (ostringstream& os) const;
    virtual		       ~CobolSymbol (void);
};

/*------------------------------------------------------------------------*/

inline const char* CobolSymbol :: GetName (void)
{
    return (CobolName);
}

inline const char* CobolSymbol :: GetCName (void)
{
    return (CName);
}

inline const char* CobolSymbol :: GetFullCName (void)
{
    return (FullCName);
}

#endif

