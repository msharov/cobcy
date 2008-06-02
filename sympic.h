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
    uint32_t		Size;
    uint32_t		CSize;
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
    uint32_t		nDigitsBDP;	// Before Decimal Point
    uint32_t		nDigitsADP;	// After Decimal Point
    uint32_t		nFillerZeroes;	// Zeroes after DP
    char *		Text;

protected:
    void		Expand (const char* pic, char* expanded);

public:
			PictureType (void);
    uint32_t		Set (const char* NewPicture);
    void		GenTypePrefix (ostream& os);
    void		GenTypeSuffix (ostream& os);
    void		GenReadFunction (ostream& os);
    void		GenWriteFunction (ostream& os);
    void		GenSignature (ostream& os);
    bool		GenCastFrom (ostream& os, PictureType& pic);
    bool		GenCastTo (ostream& os, PictureType& pic);
    virtual void	WriteTextStream (ostream& os);
    inline bool		IsNumeric (void);
    inline uint32_t	GetSize (void);
    virtual	       ~PictureType (void);
};

/*------------------------------------------------------------------------*/

inline bool PictureType :: IsNumeric (void)
{
    return (Kind == Integer || Kind == Float);
}

inline uint32_t PictureType :: GetSize (void)
{
    return (Size);
}

#endif

