/* sympic.h
**
**	Defines a picture data type. Here are all the data-type dependent
** functions.
*/

#ifndef __SYMPIC_H
#define __SYMPIC_H

#include "adtlib/streamab.h"

class PictureType : public Streamable {
protected:
    WORD		Size;
    WORD		CSize;
    enum { 
       Undefined,
       Integer, 
       Float,
       String 
    } 			Kind;
    enum {
       NoSign = false,
       TrailingSign = true,
       LeadingSign
    }			Sign;
    bool		SignSeparate;
    WORD		nDigitsBDP;	// Before Decimal Point
    WORD		nDigitsADP;	// After Decimal Point
    WORD		nFillerZeroes;	// Zeroes after DP
    char *		Text;

protected:
    void		Expand (const char* pic, char* expanded);

public:
			PictureType (void);
    WORD		Set (const char* NewPicture);
    void		GenTypePrefix (ostream& os);
    void		GenTypeSuffix (ostream& os);
    void		GenReadFunction (ostream& os);
    void		GenWriteFunction (ostream& os);
    void		GenSignature (ostream& os);
    bool		GenCastFrom (ostream& os, PictureType& pic);
    bool		GenCastTo (ostream& os, PictureType& pic);
    virtual void	WriteTextStream (ostream& os);
    inline bool		IsNumeric (void);
    inline WORD		GetSize (void);
    virtual	       ~PictureType (void);
};

/*------------------------------------------------------------------------*/

inline bool PictureType :: IsNumeric (void)
{
    return (Kind == Integer || Kind == Float);
}

inline WORD PictureType :: GetSize (void)
{
    return (Size);
}

#endif

