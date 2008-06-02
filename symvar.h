/* symvar.h
**
**	Defines a variable symbol
*/

#ifndef __SYMVAR_H
#define __SYMVAR_H

#include "symdata.h"
#include "symconst.h"
#include "sympic.h"

class CobolVar : public CobolData {
protected:
    PictureType		Picture;

public:
			CobolVar (void);
    void		SetPicture (const char* NewPic);
    virtual CobolSymbolType	Kind (void);
    inline BOOL		IsNumeric (void);

    void		GenDeclare (ostream& os);
    void		GenRead (ostream& os, const char* stream);
    void		GenWrite (ostream& os, const char* stream);
    void		GenMove (ostream& os, CobolVar * data);
    void		GenMove (ostream& os, CobolConstant& data);
    void		GenArith (ostream& os, Streamable * op1,
    				  Streamable * op2, char op);
    void		GenSignature (ostream& os);
    void		GenCharCast (ostream& os);
    inline void		WritePicture (ostream& os);

    virtual	       ~CobolVar (void);
};

/*-----------------------------------------------------------------------*/

inline BOOL CobolVar :: IsNumeric (void)
{
    return (Picture.IsNumeric());
}

inline void CobolVar :: WritePicture (ostream& os)
{
    os << Picture;
}

#endif

