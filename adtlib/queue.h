/* queue.h
**
**	An out-of-the-book queue class. Very trivial. Just Append and Serve. 
**
** includes:
**	chain.h	- for Chain base class
*/

#ifndef __QUEUE_H
#define __QUEUE_H		      

#include "chain.h"

template <class QueueEl>
class Queue : public Chain<QueueEl> {
public:
    virtual inline void		Append (QueueEl * element);
    virtual inline QueueEl *	Serve (void);
    inline QueueEl *		Front (void);
};

/*--------------------------------------------------------------------------*/

template <class QueueEl>
inline void Queue<QueueEl> :: Append 
(QueueEl * element)
{
ChainLink<QueueEl> * NewLink;
 
    NewLink = new ChainLink<QueueEl>;
    NewLink->SetData (element);
    ConnectBefore (this->MoveToHead(), NewLink);
}

template <class QueueEl>
inline QueueEl * Queue<QueueEl> :: Serve (void)
{
ChainLink<QueueEl> * ptr;
QueueEl * data;

    assert (!this->IsEmpty());

    ptr = this->MoveToTail();
    assert (ptr != NULL);
    Disconnect (ptr);
    data = ptr->GetData();
    delete ptr;
    return (data);
}

template <class QueueEl>
inline QueueEl * Queue<QueueEl> :: Front (void)
{
ChainLink<QueueEl> * ptr;
QueueEl * data = NULL;

    ptr = this->MoveToTail();
    if (ptr != NULL)
	data = ptr->GetData();
    return (data);
}

#endif 

