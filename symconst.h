/* symconst.h
**
**	Defines a constant data type. This never enters the symbol table.
*/

#ifndef __SYMCONST_H
#define __SYMCONST_H

#include <mdefs.h>
#include <streamab.h>

#include "semsedef.h"

class CobolConstant : public Streamable {
protected:
    union {
        char *		cval;
	long int	ival;
	double		fval;
    }				data;
    enum {
        CC_Undefined,
        CC_String,
	CC_Integer,
	CC_Float
    }				CurKind;

public:
				CobolConstant (void);
				CobolConstant (char * cval);
				CobolConstant (long int ival);
				CobolConstant (double fval);
    CobolConstant&		operator= (char * cval);
    CobolConstant&		operator= (long int ival);
    CobolConstant&		operator= (double fval);
    CobolConstant&		operator= (StackEntry * se);
    virtual void		Write (ostream& os);
    virtual void		Write (ofstream& os);
    void			GenWrite (ofstream& os, char * stream);
    inline BOOL			IsNumeric (void);
    virtual		       ~CobolConstant (void);
};

/*---------------------------------------------------------------------------*/

inline BOOL CobolConstant :: IsNumeric (void)
{
    return (CurKind == CC_Integer || CurKind == CC_Float);
}

#endif

