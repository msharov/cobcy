/* symrec.h
**
**	Defines a cobcy record symbol
*/

#ifndef __SYMREC_H
#define __SYMREC_H

#include "symdata.h"
#include "symfile.h"
#include "adtlib/llist.h"

class CobolRecord : public CobolData {
public:
    uint32_t			nChildren;
    LList<CobolData>		ChildList;

public:
				CobolRecord (void);
    void			AddChild (CobolData * NewChild);
    CobolSymbolType		Kind (void);
    void			GenDeclareBegin (ostream& os);
    void			GenDeclareEnd (ostream& os);
    void			GenRead (ostream& os, const char* stream);
    void			GenWrite (ostream& os, const char* stream);
    void			GenSignature (ostream& os);
    virtual		       ~CobolRecord (void);
};

#endif

