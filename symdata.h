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
    WORD		CSize;
    WORD		DeclLevel;			// >01< ID IS RECORD
    CobolFile *		AssociatedStream;

public:
			CobolData (void);
    inline virtual void	SetDeclLevel (WORD NewLevel);
    inline virtual WORD	GetDeclLevel (void);
    void		AssociateWithStream (CobolFile * NewStream);
    CobolFile *		GetStream (void);
    virtual void	GenRead (ostream& os, char * stream) = 0;
    virtual void	GenWrite (ostream& os, char * stream) = 0;
    virtual void	GenSignature (ostream& os) = 0;
    void		GenRead (ostream& os);
    void		GenWrite (ostream& os);
    inline WORD		GetSize (void);
		       ~CobolData (void);
};

/*---------------------------------------------------------------------------*/

inline void CobolData :: SetDeclLevel (WORD NewLevel)
{
    DeclLevel = NewLevel;
}

inline WORD CobolData :: GetDeclLevel (void)
{
    return (DeclLevel);
}

inline WORD CobolData :: GetSize (void)
{
    return (CSize);
}

#endif

