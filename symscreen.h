/* symscreen.h
**
**	Base class for screen elements.
*/

#ifndef __SYMSCREEN_H
#define __SYMSCREEN_H

#include <mdefs.h>
#include <llist.h>
#include "symbase.h"
#include "semtypes.h"

class CobolScreenField : public CobolSymbol {
private:
    typedef LList<CobolScreenField>	FieldList;

private:
    WORD		DeclLevel;
    int			FieldLine;
    int			FieldColumn;
    int			ForeColor;
    int			BackColor;
    char		DataVarName [MAX_SYMBOL_LENGTH];
    BOOL		BlankScreen;
    FieldList		Children;
    int			nChildren;
    StackEntryPtr	DefaultValue;

public:
			CobolScreenField (void);
    virtual	       ~CobolScreenField (void);
    inline void		AddChild (CobolScreenField* newChild);
    inline virtual CobolSymbolType Kind (void);
    inline void		SetDeclLevel (WORD NewLevel);
    inline WORD		GetDeclLevel (void) const;
    inline void		SetFieldLine (int newLine);
    inline void		SetFieldColumn (int newLine);
    inline void		SetForeColor (int newValue);
    inline void		SetBackColor (int newValue);
    inline void		SetDataVarName (const char* newName);
    inline void		SetScreenBlankFlag (BOOL newValue);
    inline void		SetDefaultValue (StackEntryPtr newValue);
    virtual void	GenRead (ostream& os, const char * stream);
    virtual void	GenWrite (ostream& os, const char * stream);
    virtual void	GenDeclare (void);
};

/*---------------------------------------------------------------------------*/

inline void CobolScreenField :: SetDeclLevel (WORD NewLevel)
{
    DeclLevel = NewLevel;
}

inline WORD CobolScreenField :: GetDeclLevel (void) const
{
    return (DeclLevel);
}

inline void CobolScreenField :: SetFieldLine (int newLine)
{
    FieldLine = newLine;
}

inline void CobolScreenField :: SetFieldColumn (int newColumn)
{
    FieldColumn = newColumn;
}

inline void CobolScreenField :: SetDataVarName (const char* newName)
{
    strcpy (DataVarName, newName);
}

inline void CobolScreenField :: SetScreenBlankFlag (BOOL newValue)
{
    BlankScreen = newValue;
}

inline void CobolScreenField :: SetForeColor (int newValue)
{
    ForeColor = newValue;
}

inline void CobolScreenField :: SetBackColor (int newValue)
{
    BackColor = newValue;
}

inline CobolSymbolType CobolScreenField :: Kind (void)
{
    return (CS_ScreenField);
}

inline void CobolScreenField::AddChild (CobolScreenField* newChild)
{
    ++ nChildren;
    Children.Tail();
    Children.InsertAfter (newChild);
}

inline void CobolScreenField::SetDefaultValue (StackEntryPtr newValue)
{
    DefaultValue = newValue;
}

/*---------------------------------------------------------------------------*/

#endif

