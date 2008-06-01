/* lclist.h
**
**	Defines a circular linked list. Based on LList.
** This header file includes llist.h, which includes chain.h
**
** includes:
**	llist.h		- base class
*/

#ifndef __LCLIST_H
#define __LCLIST_H

#include "llist.h"

template <class LCListEl> 
class LCList : public LList<LCListEl> {
public:
    virtual inline void		Next (ListWin wid = 0);
    virtual inline void		Prev (ListWin wid = 0);
    virtual inline	       ~LCList (void);
};

/*--------------------------------------------------------------------------*/

template <class LCListEl>
inline void LCList<LCListEl> :: Next
(ListWin wid = 0)
{     
    if (m_Windows[wid] == MoveToTail())
	m_Windows[wid] = MoveToHead();
    else
	m_Windows[wid] = MoveToNext (m_Windows [wid]);
};

template <class LCListEl>
inline void LCList<LCListEl> :: Prev
(ListWin wid = 0)
{     
    if (m_Windows[wid] == MoveToHead())
	m_Windows[wid] = MoveToTail();
    else
	m_Windows[wid] = MoveToPrev (m_Windows [wid]);
};

template <class LCListEl>
inline LCList<LCListEl> :: ~LCList (void)
{     
};

#endif

