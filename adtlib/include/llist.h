/* llist.h
**
**	Defines a template linked list. The current element is pointed
** to by one of the 10 built-in 'windows'. Window #0 is the default for
** all operations. Each window can point to a different element.
**	The location of window #0 is undefined after all function calls.
**
** This file includes chain.h	- base class
**		      mdefs.h	- for BOOL
*/

#ifndef __LLIST_H
#define __LLIST_H
 	 
#include <mdefs.h>
#include <chain.h>

// The following constant sets the number of m_Windows available
#define  MaxWindows		10
       
// The following type is used to reference m_Windows
typedef unsigned int		ListWin;

// Define the type for m_Size of the list
typedef WORD		ListSizeType;

/*
** Here is the actual LList template
*/
template <class LListEl> 
class LList : public Chain<LListEl> {
private:
    ListSizeType		m_Size; 
 
protected:				
    ChainLink<LListEl> **	m_Windows;
  
public:
    INLINE_CTOR			LList (void);
				
    virtual inline void	        Next (ListWin wid = 0);
    virtual inline void	      	Prev (ListWin wid = 0); 
    inline void        		Head (ListWin wid = 0); 
    inline void	       		Tail (ListWin wid = 0); 
    inline void			Skip (int count, ListWin wid = 0);
    inline BOOL	      		OnList (ListWin wid = 0) const;
					
    inline void			InsertBefore (LListEl * data, ListWin wid = 0);
    inline void			InsertAfter (LListEl * data, ListWin wid = 0);
    inline LListEl *  		Remove (ListWin wid = 0);
    inline void			Delete (ListWin wid = 0);
    inline LListEl *		LookAt (ListWin wid = 0) const;
    inline LListEl *		operator[] (ListSizeType index);
    inline ListSizeType		Size (void) const;
    
    virtual inline	       ~LList (void);
};

/*--------------------------------------------------------------------------*/

template <class LListEl>
inline LList<LListEl> :: LList (void)
{				 		  
    m_Windows = new ChainLink<LListEl> * [MaxWindows];
    m_Size = 0;
}
    
template <class LListEl>
inline void LList<LListEl> :: Next 
(ListWin wid)
{ 
    m_Windows [wid] = MoveToNext (m_Windows [wid]);
}

template <class LListEl>
inline void LList<LListEl> :: Prev
(ListWin wid)
{ 
    m_Windows [wid] = MoveToPrev (m_Windows [wid]);
}

template <class LListEl>
inline void LList<LListEl> :: Head 
(ListWin wid)
{ 
    m_Windows [wid] = MoveToHead ();
}

template <class LListEl>
inline void LList<LListEl> :: Tail
(ListWin wid)
{ 
    m_Windows [wid] = MoveToTail ();
}

template <class LListEl>
inline void LList<LListEl> :: Skip
(int count, ListWin wid)
{ 
int i;
    
    if (count < 0) {
	for (i = 0; i < abs (count); ++ i) 
	    Prev (wid); 
    }   
    else if (count > 0) {
	for (i = 0; i < abs (count); ++ i) 
	    Next (wid); 
    }   
}

template <class LListEl>
inline BOOL LList<LListEl> :: OnList
(ListWin wid) const
{ 
    return (m_Windows[wid] != NULL);
}

template <class LListEl>
inline LListEl * LList<LListEl> :: LookAt
(ListWin wid) const
{ 
    return ((LListEl*)((m_Windows[wid] == NULL) ? 
    		NULL : m_Windows[wid]->GetData()));
}

template <class LListEl>
inline LListEl * LList<LListEl> :: operator[]
(ListSizeType index)
{   	 
ListSizeType i;

    if (index >= m_Size) {
	cout << "LList: index out of range.\n";
	return (NULL);
    }
    		     
    Head (0);
    for (i = 0; i < index; ++ i)
	Next (0);
    
    return (LookAt (0));
}

template <class LListEl>
inline LListEl * LList<LListEl> :: Remove 
(ListWin wid)
{	   
ChainLink<LListEl> * ObtainedLink;
LListEl * ObtainedData;
	       
    if (IsEmpty()) {
	cout << "LList: Removing an element from an empty list!\n";
	return (NULL);
    }		 
    
    Disconnect (m_Windows [wid]);
    ObtainedLink = m_Windows [wid];
    m_Windows [wid] = MoveToHead();
    ObtainedData = ObtainedLink->GetData();
    delete ObtainedLink;
    -- m_Size;
    return (ObtainedData);
}

template <class LListEl>
inline void LList<LListEl> :: Delete 
(ListWin wid)
{	   
LListEl * ObtainedData;
    ObtainedData = Remove (wid);
    if (ObtainedData != NULL)
	delete ObtainedData;
}
	       
template <class LListEl>
inline void LList<LListEl> :: InsertBefore
(LListEl * data, ListWin wid)
{
ChainLink<LListEl> * NewLink;

    NewLink = new ChainLink<LListEl>;
    NewLink->SetData (data);
    ConnectBefore (m_Windows [wid], NewLink);
    ++ m_Size;
}

template <class LListEl>
inline void LList<LListEl> :: InsertAfter
(LListEl * data, ListWin wid)
{
ChainLink<LListEl> * NewLink;

    NewLink = new ChainLink<LListEl>;
    NewLink->SetData (data);
    ConnectAfter (m_Windows [wid], NewLink);
    ++ m_Size;
}

template <class LListEl>
inline ListSizeType LList<LListEl> :: Size 
(void) const
{
    return (m_Size);
}

template <class LListEl>
inline LList<LListEl> :: ~LList (void)
{				 		  
    delete [] m_Windows;
}

#endif

