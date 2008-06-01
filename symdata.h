/* symdata.h
**
**	Defines an abstract data thing.
*/

#ifndef __SYMDATA_H
#define __SYMDATA_H

#include <mdefs.h>
#include "symbase.h"

class CobolFile;	// They are cross-linked
class CobolData : public CobolSymbol {
protected:
    WORD		Size;
    WORD		DeclLevel;			// >01< ID IS RECORD
    CobolFile *		AssociatedStream;
    WORD		ArraySize;			// OCCURS 10 TIMES
    char		IndexVarName [MAX_SYMBOL_LENGTH]; // INDEXED BY VARX

protected:
    virtual void	CreateFullCName (void);

public:
			CobolData (void);
    virtual	       ~CobolData (void);
    inline virtual void	SetDeclLevel (WORD NewLevel);
    inline virtual WORD	GetDeclLevel (void) const;
    inline void		SetArraySize (WORD newSize);
    inline WORD		GetArraySize (void) const;
    inline void		SetArrayIndex (const char* newIndex);
    inline const char*	GetArrayIndex (void) const;
    void		AssociateWithStream (CobolFile * NewStream);
    CobolFile *		GetStream (void);
    virtual void	GenRead (ostream& os, const char * stream) = 0;
    virtual void	GenWrite (ostream& os, const char * stream) = 0;
    virtual void	GenSignature (ostream& os) = 0;
    void		GenRead (ostream& os);
    void		GenWrite (ostream& os);
    inline WORD		GetSize (void) const;
};

/*---------------------------------------------------------------------------*/

inline void CobolData :: SetDeclLevel (WORD NewLevel)
{
    DeclLevel = NewLevel;
}

inline WORD CobolData :: GetDeclLevel (void) const
{
    return (DeclLevel);
}

inline WORD CobolData :: GetSize (void) const
{
    return (Size);
}

inline void CobolData :: SetArraySize (WORD newSize)
{
    ArraySize = newSize;
}

inline WORD CobolData :: GetArraySize (void) const
{
    return (ArraySize);
}

inline void CobolData :: SetArrayIndex (const char* newIndex)
{
    strcpy (IndexVarName, newIndex);
    CobolToCName (IndexVarName);
}

inline const char* CobolData :: GetArrayIndex (void) const
{
    return (IndexVarName);
}

#endif

