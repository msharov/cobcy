/* symvar.h
**
**	Defines a variable symbol
*/

#ifndef __SYMVAR_H
#define __SYMVAR_H

#include <mdefs.h>
#include "symdata.h"
#include "symconst.h"
#include "sympic.h"

class CobolVar : public CobolData {
protected:
    PictureType		Picture;

public:
			CobolVar (void);
    void		SetPicture (char * NewPic);
    virtual CobolSymbolType	Kind (void);
    inline BOOL		IsNumeric (void);

    void		GenDeclare (ofstream& os);
    void		GenRead (ofstream& os, char * stream);
    void		GenWrite (ofstream& os, char * stream);
    void		GenMove (ofstream& os, CobolVar * data);
    void		GenMove (ofstream& os, CobolConstant& data);
    void		GenArith (ofstream& os, Streamable * op1,
    				  Streamable * op2, char op);
    inline void		WritePicture (ofstream& os);

    virtual	       ~CobolVar (void);
};

/*-----------------------------------------------------------------------*/

inline BOOL CobolVar :: IsNumeric (void)
{
    return (Picture.IsNumeric());
}

inline void CobolVar :: WritePicture (ofstream& os)
{
    os << Picture;
}

#endif

