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
    CS_ScreenField,
    CS_Label
} CobolSymbolType;

class CobolSymbol : public Streamable {
private:
    char		ParentCobolName [MAX_SYMBOL_LENGTH];
    char		Prefix [MAX_PREFIX_LENGTH];
    char		CName [MAX_SYMBOL_LENGTH];	   // C name
    char		CobolName [MAX_SYMBOL_LENGTH];	   // Cobol name
    WORD		DeclLevel;

protected:
    char		FullCName [MAX_FULLNAME_LENGTH];   // C name with prefix

protected:
    void		CobolToCName (char * str);
    virtual void	CreateFullCName (void);

public:
				CobolSymbol (void);
    virtual		       ~CobolSymbol (void);
    inline void			SetDeclLevel (WORD NewLevel);
    inline WORD			GetDeclLevel (void) const;
    void			SetName (const char * NewName);
    inline const char *		GetName (void) const;
    inline const char *		GetCName (void) const;
    inline const char *		GetFullCName (void) const;
    inline const char *		GetParentName (void) const;
    void			SetParent (const char * NewParent);
    virtual CobolSymbolType	Kind (void) = 0;
    virtual void		WriteBinaryStream (ostream& os);
    virtual void		WriteTextStream (ostream& os);
};

/*------------------------------------------------------------------------*/

inline const char * CobolSymbol :: GetName (void) const
{
    return (CobolName);
}

inline const char * CobolSymbol :: GetCName (void) const
{
    return (CName);
}

inline const char * CobolSymbol :: GetFullCName (void) const
{
    return (FullCName);
}

inline const char * CobolSymbol :: GetParentName (void) const
{
    return (ParentCobolName);
}

inline void CobolSymbol :: SetDeclLevel (WORD NewLevel)
{
    DeclLevel = NewLevel;
}

inline WORD CobolSymbol :: GetDeclLevel (void) const
{
    return (DeclLevel);
}

/*------------------------------------------------------------------------*/

#endif

