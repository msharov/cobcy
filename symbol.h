/* symbol.h
**
**	Defines a symbol table entry.
*/

#ifndef __SYMBOL_H
#define __SYMBOL_H

#include <mdefs.h>
#include <llist.h>

#define MAX_SYMBOL_LENGTH	50
#define MAX_PREFIX_LENGTH	80

typedef struct {
    WORD	Size;
    WORD	CSize;
    enum { 
       Undefined,
       Integer, 
       Float,
       String 
    } 		Kind;
    enum {
       NoSign = FALSE,
       TrailingSign = TRUE,
       LeadingSign
    }		 Sign;
    BOOL	SignSeparate;
    WORD	nDigitsBDP;	// Before Decimal Point
    WORD	nDigitsADP;	// After Decimal Point
    WORD	nFillerZeroes;	// Zeroes after DP
    char *	Text;
} PictureType;

class CobolSymbol {
private:
    char		ParentCobolName [MAX_SYMBOL_LENGTH];

public:
    enum {
       Undefined,
       Record, 
       Variable,
       Procedure,
       FileDesc,
       Label
    }			Kind;
    WORD		RecordCSize;
    PictureType		Picture;

    char		CobolName [MAX_SYMBOL_LENGTH];	// Cobol name
    char		CName [MAX_SYMBOL_LENGTH];	// C name
    WORD		DeclLevel;			// >01< ID IS RECORD
    WORD		nChildren;
    char		Prefix [MAX_PREFIX_LENGTH];

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
    void		SetParent (char * NewParent);
		       ~CobolSymbol (void);
};

#endif

