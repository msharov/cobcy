/* symbase.h
**
**	Defines an abstract symbol table entry.
** This is an abstract virtual class, so don't try to make any.
*/

#ifndef __SYMBASE_H
#define __SYMBASE_H

#include <mdefs.h>
#include <streamab.h>

#define MAX_SYMBOL_LENGTH	50
#define MAX_PREFIX_LENGTH	80
#define MAX_FULLNAME_LENGTH	(MAX_SYMBOL_LENGTH + MAX_PREFIX_LENGTH)

typedef enum {
    CS_Undefined,
    CS_Record, 
    CS_Variable,
    CS_Procedure,
    CS_FileDesc,
    CS_Label
} CobolSymbolType;

class CobolSymbol : public Streamable {
private:
    char		ParentCobolName [MAX_SYMBOL_LENGTH];
    char		Prefix [MAX_PREFIX_LENGTH];
    char		CName [MAX_SYMBOL_LENGTH];	   // C name
    char		FullCName [MAX_FULLNAME_LENGTH];   // C name with prefix
    char		CobolName [MAX_SYMBOL_LENGTH];	   // Cobol name

protected:
    void		CobolToCName (char * str);

public:
				CobolSymbol (void);
    void			SetName (char * NewName);
    inline char *		GetName (void);
    inline char *		GetCName (void);
    inline char *		GetFullCName (void);
    void			SetParent (char * NewParent);
    virtual CobolSymbolType	Kind (void) = 0;
    virtual void		Write (ostream& os);
    virtual void		Write (ofstream& os);
    virtual		       ~CobolSymbol (void);
};

/*------------------------------------------------------------------------*/

inline char * CobolSymbol :: GetName (void)
{
    return (CobolName);
}

inline char * CobolSymbol :: GetCName (void)
{
    return (CName);
}

inline char * CobolSymbol :: GetFullCName (void)
{
    return (FullCName);
}

#endif

