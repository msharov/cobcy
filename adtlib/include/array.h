/* array.h
**
**	Defines a generic array template. An array is basically an
** indexed set. The only function added is the [] operator.
**
** includes:
**	set.h		- base class
**	streamable.h	- base class
*/

#ifndef __ARRAY_H
#define __ARRAY_H
				    
#include <set.h>

typedef SetSizeType 		ArraySizeType;

template <class ArrayEl> 
class Array : public Set<ArrayEl> {
public:
    INLINE_CTOR	       	Array (ArraySizeType nElem = 0);
    INLINE_CTOR		Array (const Array<ArrayEl>& AnArray);
    INLINE_CTOR		Array (const ArrayEl * AnArray, ArraySizeType nElem);

    virtual inline ArrayEl&	operator[] (ArraySizeType index) const;
    virtual inline void		Sort (int Start = 0, int End = -1);
};	

/*---------------------------------------------------------------------------*/

template <class ArrayEl>
inline Array<ArrayEl> :: Array 
(ArraySizeType nElem) : Set<ArrayEl> (nElem)
{ 	   
};

template <class ArrayEl>
inline Array<ArrayEl> :: Array
(const Array<ArrayEl>& AnArray) : Set<ArrayEl> (AnArray)
{ 	   
};

template <class ArrayEl>
inline Array<ArrayEl> :: Array 
(const ArrayEl * AnArray, ArraySizeType nElem) : Set<ArrayEl> (AnArray, nElem)
{ 	   
};

template <class ArrayEl>
inline ArrayEl& Array<ArrayEl> :: operator[] (ArraySizeType index) const
{ 
    return (m_Data [index]);
};

template <class ArrayEl>
inline void Array<ArrayEl> :: Sort (int Start, int End)
{
int i, j;
ArrayEl v, t;

    // To allow calling Sort() without knowing the size of the array
    if (End < 0)
	End = m_Size;

    if (End > Start) {
	assert (Start >= 0 && End <= (int) m_Size);
	v = m_Data[End];
	i = Start - 1;
	j = End;
	while (TRUE) {
	    while (i < (int) m_Size && m_Data [++i] < v);
	    while (j >= 0 && m_Data [--j] > v);
	    if (i >= j)
		break;
	    t = m_Data[i];
	    m_Data[i] = m_Data[j];
	    m_Data[j] = t;
	}
	Sort (Start, i - 1);
	Sort (i + 1, End);
    }
}
       
#endif

