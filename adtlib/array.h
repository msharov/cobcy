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
				    
#include "set.h"
#include "llist.h"

typedef SetSizeType 		ArraySizeType;

template <class ArrayEl> 
class Array : public Set<ArrayEl> {
public:
    INLINE_CTOR	       	Array (ArraySizeType nElem = 0);
    INLINE_CTOR		Array (const Array<ArrayEl>& AnArray);
    INLINE_CTOR		Array (const ArrayEl * AnArray, ArraySizeType nElem);

    virtual inline ArrayEl&	operator[] (ArraySizeType index) const;
    virtual inline void		InsertAt (ArraySizeType index,
    					const ArrayEl& data);
    virtual inline void		RemoveAt (ArraySizeType index);

    virtual inline Array<ArrayEl>&	operator= (LList<ArrayEl>& ToBe);
    virtual inline void			Sort (int Start = 0, int End = -1);
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
    assert (index < this->m_Size);
    return (this->m_Data [index]);
};

template <class ArrayEl>
inline void Array<ArrayEl> :: InsertAt 
(ArraySizeType index, const ArrayEl& NewEl)
{
ArraySizeType i;
    this->Resize (this->m_Size + 1);
    for (i = this->m_Size - 1; i > index; -- i)
	this->m_Data[i] = this->m_Data[i - 1];
    this->m_Data[index] = NewEl;
}

template <class ArrayEl>
inline void Array<ArrayEl> :: RemoveAt (ArraySizeType index)
{
ArraySizeType i;
    for (i = index; i < this->m_Size - 1; ++ i)
	this->m_Data[i] = this->m_Data[i + 1];
    this->Resize (this->m_Size - 1);
}

template <class ArrayEl>
inline Array<ArrayEl>& Array<ArrayEl> :: operator= (LList<ArrayEl>& ToBe)
{ 
ArraySizeType NewSize, i;

    NewSize = ToBe.Size();
    this->Resize (NewSize);
    ToBe.Head();
    for (i = 0; i < NewSize; ++ i) {
	this->m_Data[i] = *(ToBe.LookAt());
	ToBe.Next();
    }

    return (*this);
};

template <class ArrayEl>
inline void Array<ArrayEl> :: Sort (int Start, int End)
{
int i, j;
ArrayEl v, t;

    // To allow calling Sort() without knowing the size of the array
    if (End < 0)
	End = this->m_Size;

    if (End > Start) {
	assert (Start >= 0 && End <= (int) this->m_Size);
	v = this->m_Data[End];
	i = Start - 1;
	j = End;
	while (TRUE) {
	    while (i < (int) this->m_Size && this->m_Data [++i] < v);
	    while (j >= 0 && this->m_Data [--j] > v);
	    if (i >= j)
		break;
	    t = this->m_Data[i];
	    this->m_Data[i] = this->m_Data[j];
	    this->m_Data[j] = t;
	}
	Sort (Start, i - 1);
	Sort (i + 1, End);
    }
}
       
#endif

