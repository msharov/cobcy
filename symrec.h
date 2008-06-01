/* symrec.h
**
**	Defines a cobcy record symbol
*/

#ifndef __SYMREC_H
#define __SYMREC_H

#include <mdefs.h>
#include <llist.h>
#include "symdata.h"
#include "symfile.h"

class CobolRecord : public CobolData {
public:
    WORD			nChildren;
    LList<CobolData>		ChildList;

public:
				CobolRecord (void);
    virtual		       ~CobolRecord (void);
    void			AddChild (CobolData * NewChild);
    CobolSymbolType		Kind (void);
    void			GenDeclareBegin (ostream& os);
    void			GenDeclareEnd (ostream& os);
    virtual void		GenRead (ostream& os, const char * stream);
    virtual void		GenWrite (ostream& os, const char * stream);
    void			GenSignature (ostream& os);
};

#endif

