/* symdata.h
**
**	Defines an abstract data thing.
*/

#ifndef __SYMDATA_H
#define __SYMDATA_H

#include "symbase.h"

class CobolFile;	// They are cross-linked
class CobolData : public CobolSymbol {
protected:
    uint32_t		CSize;
    uint32_t		DeclLevel;			// >01< ID IS RECORD
    CobolFile *		AssociatedStream;

public:
			CobolData (void);
    inline virtual void	SetDeclLevel (uint32_t NewLevel);
    inline virtual uint32_t	GetDeclLevel (void);
    void		AssociateWithStream (CobolFile * NewStream);
    CobolFile *		GetStream (void);
    virtual void	GenRead (ostream& os, const char* stream) = 0;
    virtual void	GenWrite (ostream& os, const char* stream) = 0;
    virtual void	GenSignature (ostream& os) = 0;
    void		GenRead (ostream& os);
    void		GenWrite (ostream& os);
    inline uint32_t		GetSize (void);
		       ~CobolData (void);
};

/*---------------------------------------------------------------------------*/

inline void CobolData :: SetDeclLevel (uint32_t NewLevel)
{
    DeclLevel = NewLevel;
}

inline uint32_t CobolData :: GetDeclLevel (void)
{
    return (DeclLevel);
}

inline uint32_t CobolData :: GetSize (void)
{
    return (CSize);
}

#endif

