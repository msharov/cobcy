/* sympic.h
**
**	Defines a picture data type. Here are all the data-type dependent
** functions.
*/

#ifndef __SYMPIC_H
#define __SYMPIC_H

#include <mdefs.h>
#include <streamab.h>

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
       NoSign = FALSE,
       TrailingSign = TRUE,
       LeadingSign
    }			Sign;
    BOOL		SignSeparate;
    WORD		nDigitsBDP;	// Before Decimal Point
    WORD		nDigitsADP;	// After Decimal Point
    WORD		nFillerZeroes;	// Zeroes after DP
    char *		Text;

protected:
    void		Expand (char * pic, char * expanded);

public:
			PictureType (void);
    virtual	       ~PictureType (void);
    WORD		Set (char * NewPicture);
    void		GenTypePrefix (ostream& os);
    void		GenTypeSuffix (ostream& os);
    void		GenReadFunction (ostream& os);
    void		GenWriteFunction (ostream& os);
    void		GenSignature (ostream& os);
    BOOL		GenCastFrom (ostream& os, PictureType& pic);
    BOOL		GenCastTo (ostream& os, PictureType& pic);
    virtual void	WriteTextStream (ostream& os);
    inline BOOL		IsNumeric (void) const;
    inline BOOL		IsInteger (void) const;
    inline WORD		GetSize (void) const;
};

/*------------------------------------------------------------------------*/

inline BOOL PictureType :: IsNumeric (void) const
{
    return (Kind == Integer || Kind == Float);
}

inline BOOL PictureType :: IsInteger (void) const
{
    return (Kind == Integer);
}

inline WORD PictureType :: GetSize (void) const
{
    return (Size);
}

#endif

