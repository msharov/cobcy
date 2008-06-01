/* bitarray.h
**
**	Defines a bit-level array. based on Array template <BYTE>.
*/

#ifndef __BITARRAY_H
#define __BITARRAY_H
   
#include "array.h"

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE		8
#define BPB_SHIFT		3
#endif

typedef SetSizeType 		ArraySizeType;

class BitArray : public Array<BYTE> {
protected:
    ArraySizeType	m_nBits;

public:
    			BitArray (ArraySizeType nElem = 0);
    			BitArray (const BitArray& AnArray);
    			BitArray (const BYTE * AnArray, ArraySizeType nElem);

    inline const BOOL		GetAt (ArraySizeType index) const;
    WORD			GetAt (ArraySizeType iStart, 
    					ArraySizeType iEnd) const;
    inline void			SetAt (ArraySizeType index, BOOL Value);
    void			SetAt (ArraySizeType iStart,
    					ArraySizeType iEnd, WORD Value);
    inline void			FlipAt (ArraySizeType index);

    inline virtual void		Fill (BOOL Value);
    virtual void		Resize (ArraySizeType NewSize);
    inline virtual ArraySizeType	Size (void) const;

    virtual BitArray&		operator= (const BitArray& ToBe);
    virtual void		ReadBinaryStream (istream& is);
    virtual void		WriteBinaryStream (ostream& os);
    virtual inline WORD		StreamSize (void) const;
};	

/*--------------------------------------------------------------------------*/

inline const BOOL BitArray :: GetAt (ArraySizeType index) const
{
    assert (index < m_nBits);
    return ((m_Data [index >> BPB_SHIFT] & 
    	     (1 << (index % BITS_PER_BYTE))) != 0);
}

inline void BitArray ::	SetAt (ArraySizeType index, BOOL Value)
{
    assert (index < m_nBits);
    if (Value)
	m_Data [index >> BPB_SHIFT] |= (BYTE) 1 << (index % BITS_PER_BYTE);
    else
	m_Data [index >> BPB_SHIFT] &= ~((BYTE) 1 << (index % BITS_PER_BYTE));
}

inline void BitArray ::	FlipAt (ArraySizeType index)
{
    assert (index < m_nBits);
    m_Data [index >> BPB_SHIFT] ^= (BYTE) 1 << (index % BITS_PER_BYTE);
}

inline void BitArray :: Fill (BOOL Value)
{
    if (Value == TRUE)
	Array<BYTE> :: Fill (0xFF);
    else
	Array<BYTE> :: Fill (0x00);
}

inline ArraySizeType BitArray :: Size (void) const
{
    return (m_nBits);
}

inline ArraySizeType BitArray :: StreamSize (void) const
{
    return (sizeof(WORD) + Array<BYTE> :: StreamSize());
}

#endif

