/* symlabel.h
**
**	Defines a label symbol
*/

#ifndef __SYMLABEL_H
#define __SYMLABEL_H

#include "symbase.h"

class CobolLabel : public CobolSymbol {
public:
    BOOL		Undeclared;	// For forward references

public:
			CobolLabel (void);
    inline CobolSymbolType	Kind (void);
    void		GenDeclare (ostream& os);
    void		GenJump (ostream& os, CobolLabel * dest);
		       ~CobolLabel (void);
};

/*---------------------------------------------------------------*/

inline CobolSymbolType CobolLabel :: Kind (void)
{
    return (CS_Label);
}

#endif

