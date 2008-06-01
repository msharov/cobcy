/* chain.h
**
** 	Defines a template for a chain of objects. Each link is
** an instance of ChainLink which has a private pointer to actual m_Data.
** Since it is quite inconvenient to use due to the requirement to know
** a pointer to the actual ChainLinks, it is only used as a base class
** for Linked List type things.
**
** includes:					   
**	mdefs.h		- for NULL
**	iostream.h	- for cout (error messages)
*/
	      
#ifndef __CHAIN_H
#define __CHAIN_H				   

#include "mdefs.h"

template <class ChainEl>
class Chain;

template <class DataType> 
class ChainLink {
public:
    DataType *			m_Data;
    ChainLink<DataType> *	m_Next;
    ChainLink<DataType> *	m_Prev;
public:			   
    INLINE_CTOR			ChainLink (void) { 
					m_Data = NULL; 
					m_Next = m_Prev = NULL;
				};
    inline void			SetData (DataType * NewData) { 
					m_Data = NewData;
				};
    inline DataType *		GetData (void) const { 
					return (m_Data);
				};
};
    
template <class ChainEl> 
class Chain {
private:
    ChainLink<ChainEl> *	m_Head;
    ChainLink<ChainEl> *	m_Tail;

protected:
    inline ChainLink<ChainEl> *		MoveToHead (void) const;
    inline ChainLink<ChainEl> *		MoveToTail (void) const;
    inline ChainLink<ChainEl> *		MoveToNext (const ChainLink<ChainEl> * current) const;
    inline ChainLink<ChainEl> *		MoveToPrev (const ChainLink<ChainEl> * current) const;

    inline void			Disconnect (ChainLink<ChainEl> * current);
    inline void		    	ConnectAfter (ChainLink<ChainEl> * current, 
    					      ChainLink<ChainEl> * newLink);
    inline void		    	ConnectBefore (ChainLink<ChainEl> * current, 
    					       ChainLink<ChainEl> * newLink);
    
public:
    INLINE_CTOR 		Chain (void);
    inline BOOL			IsEmpty (void) const;
    virtual inline 	       ~Chain (void);
};

/*--------------------------------------------------------------------------*/

template <class ChainEl> 
inline Chain<ChainEl> :: Chain (void)
{
    m_Head = m_Tail = NULL;
}

template <class ChainEl>
inline BOOL Chain<ChainEl> :: IsEmpty (void) const
{
    if (m_Head == NULL && m_Tail == NULL)
	return (TRUE);
    assert (!((m_Head == NULL || m_Tail == NULL) && m_Head != m_Tail));
    return (FALSE);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToHead
(void) const
{
    return (m_Head);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToTail
(void) const
{
    return (m_Tail);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToNext
(const ChainLink<ChainEl> * current) const
{
    if (current != m_Tail && current != NULL)
       	return (current->m_Next);
    else
       	return (NULL);
}

template <class ChainEl> 
inline ChainLink<ChainEl> * Chain<ChainEl> :: MoveToPrev
(const ChainLink<ChainEl> * current) const
{
    if (current != m_Head && current != NULL)
       	return (current->m_Prev);
    else
       	return (NULL);
}

template <class ChainEl> 
inline void Chain<ChainEl> :: Disconnect
(ChainLink<ChainEl> * current)
{
    assert (current != NULL);

    if (current == m_Head)
       	m_Head = current->m_Next;
    else
       	current->m_Prev->m_Next = current->m_Next; 
       
    if (current == m_Tail)
       	m_Tail = current->m_Prev;
    else
       	current->m_Next->m_Prev = current->m_Prev;
    
    current->m_Next = NULL;
    current->m_Prev = NULL;

    // Check for the memory error that messes up pointers
    assert (!((m_Head == NULL || m_Tail == NULL) && m_Head != m_Tail));
}

template <class ChainEl> 
inline void Chain<ChainEl> :: ConnectAfter
(ChainLink<ChainEl> * current, ChainLink<ChainEl> * newLink)
{
    if (current == NULL) {
	assert (IsEmpty());
	m_Head = newLink;
	m_Tail = newLink;
    }
    else {
       	newLink->m_Next = current->m_Next;
       	newLink->m_Prev = current;

       	if (current == m_Tail)
            m_Tail = newLink;
       	else		       
            current->m_Next->m_Prev = newLink;
       	current->m_Next = newLink;
    }
}

template <class ChainEl>
inline void Chain<ChainEl> :: ConnectBefore
(ChainLink<ChainEl> * current, ChainLink<ChainEl> * newLink)
{
    if (current == NULL) {
	assert (IsEmpty());
	m_Head = newLink;
	m_Tail = newLink;
    }
    else {
	newLink->m_Next = current;
	newLink->m_Prev = current->m_Prev;

	if (current == m_Head)
	    m_Head = newLink;
	else		       
	    current->m_Prev->m_Next = newLink;
	current->m_Prev = newLink;
    }
}

template <class ChainEl> 
inline Chain<ChainEl> :: ~Chain 
(void)
{	
ChainLink<ChainEl> * scrap;
ChainEl * m_Data;
					    
    while (!IsEmpty()) {			     
        scrap = MoveToHead();
        Disconnect (scrap);
	if ((m_Data = scrap->GetData()) != NULL)
	    delete (m_Data);
	delete scrap;
    }		    
}      

#endif

