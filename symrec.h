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
    void			AddChild (CobolData * NewChild);
    CobolSymbolType		Kind (void);
    void			GenDeclareBegin (ofstream& os);
    void			GenDeclareEnd (ofstream& os);
    void			GenRead (ofstream& os, char * stream);
    void			GenWrite (ofstream& os, char * stream);
    virtual		       ~CobolRecord (void);
};

#endif

