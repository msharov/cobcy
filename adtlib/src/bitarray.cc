/* bitarray.h
**
**	Defines a bit-level array. based on Array template <BYTE>.
*/

#include <bitarray.h>
#include <cendian.h>

BitArray :: BitArray (ArraySizeType nElem)
: Array<BYTE> (nElem / BITS_PER_BYTE + 1)
{
    m_nBits = nElem;
}

BitArray :: BitArray (const BitArray& AnArray)
: Array<BYTE> (AnArray)
{
    m_nBits = AnArray.m_nBits;
}

BitArray :: BitArray (const BYTE * AnArray, ArraySizeType nElem)
: Array<BYTE> (AnArray, nElem)
{
    m_nBits = nElem * BITS_PER_BYTE;
}

WORD BitArray :: GetAt 
(ArraySizeType iStart, ArraySizeType iEnd) const
{
WORD Result = 0, Mask = 1;
WORD i;

    assert (m_nBits > 0);
    assert (iStart < iEnd && iEnd <= m_nBits);
    for (i = iStart; i < iEnd; ++ i) {
        Result |= Mask * GetAt (i);
	Mask <<= 1;
    }
    return (Result);
}

void BitArray :: SetAt 
(ArraySizeType iStart, ArraySizeType iEnd, WORD Value)
{
WORD Mask = 1;
WORD i;

    assert (m_nBits > 0);
    assert (iStart < iEnd && iEnd <= m_nBits);
    for (i = iStart; i < iEnd; ++ i) {
        SetAt (i, ((Mask & Value) != 0));
	Mask <<= 1;
    }
}

void BitArray :: Resize (ArraySizeType NewSize)
{
    m_nBits = NewSize;
    Array<BYTE> :: Resize (NewSize / BITS_PER_BYTE + 1);
}

BitArray& BitArray :: operator= (const BitArray& ToBe)
{
    m_nBits = ToBe.m_nBits;
    Array<BYTE> :: operator= (ToBe);
    return (*this);
}

void BitArray :: ReadBinaryStream (istream& is)
{
    ReadWord (is, &m_nBits);
    Array<BYTE> :: ReadBinaryStream (is);
}

void BitArray :: WriteBinaryStream (ostream& os)
{
    WriteWord (os, &m_nBits);
    Array<BYTE> :: WriteBinaryStream (os);
}

