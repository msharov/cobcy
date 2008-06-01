/* symlabel.h
**
**	Defines a label symbol
*/

#ifndef __SYMLABEL_H
#define __SYMLABEL_H

#include <mdefs.h>
#include "symbase.h"

class CobolLabel : public CobolSymbol {
public:
    BOOL		Undeclared;	// For forward references
    int			Order;

public:
			CobolLabel (void);
    virtual	       ~CobolLabel (void);
    inline CobolSymbolType	Kind (void);
    void		GenDeclare (ostream& os);
    void		GenJump (ostream& os, CobolLabel * dest);
    virtual void	WriteBinaryStream (ostream& os);
    virtual void	WriteTextStream (ostream& os);
};

/*---------------------------------------------------------------*/

inline CobolSymbolType CobolLabel :: Kind (void)
{
    return (CS_Label);
}

#endif

