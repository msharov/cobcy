/* symbol.h
**
**	Defines a symbol table entry.
*/

#ifndef __SYMBOL_H
#define __SYMBOL_H

#include <mdefs.h>
#include <llist.h>

#define MAX_SYMBOL_LENGTH	50

typedef struct {
    WORD	Size;
    WORD	CSize;
    enum { 
       Undefined,
       Integer, 
       Float,
       String 
    } 		Kind;
    BOOL	Sign;
    WORD	nDigitsBDP;	// Before Decimal Point
    WORD	nDigitsADP;	// After Decimal Point
    WORD	nFillerZeroes;	// Zeroes after DP
    char *	Text;
} PictureType;

class CobolSymbol {
public:
    enum {
       Undefined,
       Record, 
       Variable,
       Procedure,
       FileDesc,
       Label
    }			Kind;
    PictureType		Picture;

    WORD		RecordCSize;

    char		CobolName [MAX_SYMBOL_LENGTH];	// Cobol name
    char		CName [MAX_SYMBOL_LENGTH];	// C name
    WORD		DeclLevel;			// >01< ID IS RECORD
    char		ParentCName [MAX_SYMBOL_LENGTH];
    WORD		nChildren;

    char		FileName[30];
    char		FileRecordName [MAX_SYMBOL_LENGTH];
    char		FileStatusVar [MAX_SYMBOL_LENGTH];
    char		FileRelativeKey [MAX_SYMBOL_LENGTH];
    char		FileRecordKey [MAX_SYMBOL_LENGTH];
    enum {
       AM_Sequential,
       AM_Random,
       AM_Dynamic
    }			FileAccessMode;
    enum {
       ORG_Sequential,
       ORG_Line_sequential,
       ORG_Relative,
       ORG_Indexed
    }			FileOrganization;

    BOOL		Undeclared;	// For forward references
    LList<CobolSymbol>	ChildList;

protected:
    void 		CobolToCName (char * str);
    void		ExpandPicture (char * pic, char * expanded);

public:
			CobolSymbol (void);
    void		SetPicture (char * NewPic);
    void		SetName (char * NewName);
    void		AddChild (CobolSymbol * NewChild);
		       ~CobolSymbol (void);
};

#endif

