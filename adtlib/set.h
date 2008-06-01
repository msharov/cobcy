/* set.h
**
**	Defines a generic set template.
**
** includes:
**	mdefs.h	- for BOOL
*/

#ifndef __SET_H
#define __SET_H		  

#include "mdefs.h"
#include "streamab.h"
#include "chain.h"

typedef unsigned long 	SetSizeType;

template <class SetEl> 
class Set : public Streamable {
protected:
    SetEl *	       			m_Data;
    SetSizeType      			m_Size;
    
public:
    INLINE_CTOR		       		Set (SetSizeType nElem = 0); 
    INLINE_CTOR				Set (const Set<SetEl>& ASet);
    INLINE_CTOR				Set (const SetEl * ASet, SetSizeType nElem);
    virtual inline Set<SetEl>&		operator= (const Set<SetEl>& toBe);
    virtual inline Set<SetEl>&		operator= (const SetEl * toBe);
    virtual inline BOOL			operator== (const Set<SetEl>& toCompare) const;
    virtual inline 			operator const SetEl * () const;

    virtual inline void			Resize (SetSizeType NewSize);
    virtual inline void			Fill (SetEl value);
    virtual inline SetSizeType    	Size (void) const;
    virtual inline SetSizeType		Find (SetEl match) const;

    virtual inline void			ReadBinaryStream (istream& is);
    virtual inline void			WriteBinaryStream (ostream& os);
    virtual inline WORD			StreamSize (void) const;

    virtual inline 	 	       ~Set (void);
};	

/*--------------------------------------------------------------------------*/

template <class SetEl>
inline Set<SetEl> :: Set (SetSizeType nElem) 
{ 	   
    m_Size = nElem;
    if (m_Size > 0) {
	m_Data = new SetEl [m_Size];
	assert (m_Data != NULL);
    }
    else
	m_Data = NULL;
};

template <class SetEl>
inline Set<SetEl> :: Set 
(const Set<SetEl>& ASet)
{
SetSizeType i;
    m_Size = ASet.m_Size;
    if (m_Size > 0) {
	m_Data = new SetEl [m_Size];
	assert (m_Data != NULL);
    }
    else
	m_Data = NULL;
    
    for (i = 0; i < m_Size; ++ i)
	m_Data[i] = ASet.m_Data[i];
}

template <class SetEl>
inline Set<SetEl> :: Set 
(const SetEl * ASet, SetSizeType nElem)
{
SetSizeType i;
    m_Size = nElem;
    if (m_Size > 0) {
	m_Data = new SetEl [m_Size];
	assert (m_Data != NULL);
    }
    else
	m_Data = NULL;
    
    for (i = 0; i < m_Size; ++ i)
	m_Data[i] = ASet[i];
}

template <class SetEl>
inline Set<SetEl>& Set<SetEl> :: operator=
(const Set<SetEl>& toBe)
{
SetSizeType i;
    Set<SetEl> :: Resize (toBe.m_Size);
    for (i = 0; i < toBe.m_Size; ++ i)
	m_Data[i] = toBe.m_Data[i];
    return (*this);
}

template <class SetEl>
inline Set<SetEl>& Set<SetEl> :: operator=
(const SetEl * toBe)
{
SetSizeType i;
    for (i = 0; i < m_Size; ++ i)
	m_Data[i] = toBe[i];
    return (*this);
}

template <class SetEl>
inline BOOL Set<SetEl> :: operator==
(const Set<SetEl>& toCompare) const
{
SetSizeType i;

    if (m_Size != toCompare.m_Size)
	return (FALSE);
    for (i = 0; i < m_Size; ++ i)
	if (!(m_Data[i] == toCompare.m_Data[i])) // to require only == in m_Data
	    return (FALSE);
    return (TRUE);    
}

template <class SetEl>
inline Set<SetEl> :: operator const SetEl * () const
{ 
    return (m_Data);
};

template <class SetEl>
inline void Set<SetEl> :: Fill (SetEl value)
{
SetSizeType i;
    for (i = 0; i < m_Size; ++ i)
	m_Data[i] = value;
}

template <class SetEl>
inline SetSizeType Set<SetEl> :: Size (void) const
{ 					  
    return (m_Size);
};

template <class SetEl>
inline void Set<SetEl> :: Resize (SetSizeType NewSize)
{ 					  
SetEl * NewSet = NULL;
SetSizeType i;
		 
    if (NewSize == m_Size)
	return;

    if (NewSize > 0) {
	NewSet = new SetEl [NewSize]; 
	assert (NewSet != NULL);
	for (i = 0; i < min (NewSize, m_Size); ++ i)
	    NewSet [i] = m_Data [i];
    }
    if (m_Size > 0)
	delete [] m_Data;
    m_Data = NewSet;
    m_Size = NewSize;
};

template <class SetEl>
inline SetSizeType Set<SetEl> :: Find (SetEl match) const
{			  
SetSizeType i;
long int index = S_ERROR;

    for (i = 0; i < m_Size; ++ i) {
	if (m_Data[i] == match) {
	    index = i;	      
	    break;
	}
    }
    return (index);
};

template <class SetEl>
inline void Set<SetEl> :: ReadBinaryStream (istream& is)
{	     
SetSizeType SavedSize, i;

    ReadWord (is, &SavedSize);
    Set<SetEl> :: Resize (SavedSize);
    for (i = 0; i < m_Size; ++ i)
	ReadRaw (is, &m_Data[i], sizeof(SetEl));
};

template <class SetEl>
inline void Set<SetEl> :: WriteBinaryStream (ostream& os)
{
SetSizeType i;

    WriteWord (os, &m_Size);
    for (i = 0; i < m_Size; ++ i)
	WriteRaw (os, &m_Data[i], sizeof(SetEl));
};

template <class SetEl>
inline WORD Set<SetEl> :: StreamSize (void) const
{
    return (sizeof(WORD) + m_Size * sizeof(SetEl));
}

template <class SetEl>
inline Set<SetEl> :: ~Set (void)
{	 
    if (m_Size > 0)
	delete [] m_Data;
};

#endif

