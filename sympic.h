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
    WORD		Set (char * NewPicture);
    void		GenTypePrefix (ofstream& os);
    void		GenTypeSuffix (ofstream& os);
    void		GenReadFunction (ofstream& os);
    void		GenWriteFunction (ofstream& os);
    BOOL		GenCastFunction (ofstream& os, PictureType& pic);
    virtual void	Write (ostream& os);
    virtual void	Write (ofstream& os);
    inline BOOL		IsNumeric (void);
    inline WORD		GetSize (void);
    virtual	       ~PictureType (void);
};

/*------------------------------------------------------------------------*/

inline BOOL PictureType :: IsNumeric (void)
{
    return (Kind == Integer || Kind == Float);
}

inline WORD PictureType :: GetSize (void)
{
    return (Size);
}

#endif

