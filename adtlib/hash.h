/* hash.h
**
**	Defines a hash table template. Template is the attributes for
** each string. Thus the string manipulation is completely encapsulated.
*/

#ifndef __HASH_H
#define __HASH_H

#include "mdefs.h"
#include <string.h>

// This seems reasonable (covers the alphabet)
#define DEFAULT_TABLE_SIZE	50

// We are going to use the string space method, allocating
//	space for names with page quantum below:
#define STRING_SPACE_PAGE	100

typedef struct {
    WORD	Index;
    WORD	Length;
} StringSpaceEntry;

// The bucket structure is accessible only to the hash table to
//	hide the above detail.
template <class HashAttr>
class HashTable;

template <class HashAttr>
class Bucket {
public:
    StringSpaceEntry	m_Name;  	// This is actually an index to m_StringSpace 
public:
    HashAttr *		m_Attr;
    Bucket<HashAttr> *	m_Next;
};

// Keys are generated internally, so do not assume anything
typedef WORD		HashKeyType;
    
template <class HashAttr>
class HashTable {
private:
    char *		m_StringSpace;
    WORD		m_StringSpaceIndex;
    WORD		m_PagesAllocated;

protected:
    Bucket<HashAttr> **	Table;
    WORD		TableSize;

protected:
    inline HashKeyType	GetKey (const char* Name);
    inline bool		IsEqual (const char* Name, Bucket<HashAttr>* ABucket);

public:
    INLINE_CTOR		HashTable (WORD NewTableSize = DEFAULT_TABLE_SIZE);
    inline void		Insert (const char* Name, HashAttr * Attr);
    inline void 	Remove (const char* Name);
    inline HashAttr *	Lookup (const char* Name);
    inline void		Clear (void);
    inline             ~HashTable (void);
};

template <class HashAttr>
inline HashKeyType HashTable<HashAttr> :: GetKey (const char* Name)
{
    // Key = (Name[First] + Name[Last]) % TableSize
    return ((HashKeyType)((Name[0] + Name[strlen(Name)]) % TableSize));
};

template <class HashAttr>
inline bool HashTable<HashAttr> :: IsEqual (const char* Name, Bucket<HashAttr>* ABucket )
{
WORD NameLength, i, ssi;
    NameLength = strlen (Name);
    ssi = ABucket->m_Name.Index;	// String space index
    if (NameLength != ABucket->m_Name.Length)
	return (false);
    for (i = 0; i < NameLength; ++ i) {
	if (Name[i] != m_StringSpace[ssi])
	    return (false);
	++ ssi;
    }
    return (true);
};

template <class HashAttr>
inline HashTable<HashAttr> :: HashTable (WORD NewTableSize)
{
HashKeyType i;
    TableSize = NewTableSize;
    Table = new Bucket<HashAttr>* [TableSize];
    assert (Table != NULL);
    for (i = 0; i < TableSize; ++ i)
	Table[i] = NULL;
    m_StringSpaceIndex = 0;
    // Allocate with *alloc to use realloc to expand
    m_StringSpace = (char*) calloc (1, STRING_SPACE_PAGE * sizeof(char));
    m_PagesAllocated = 1;
};

template <class HashAttr>
inline void HashTable<HashAttr> :: Insert (const char* Name, HashAttr * Attr)
{
HashKeyType NewKey;
Bucket<HashAttr> * NewBucket;
WORD NameLength, NewSSIndex, NeedAllocate;

    // Check if the name is already here
    if (Lookup (Name) != NULL)
	return;

    // First, setup the new bucket
    NewBucket = new Bucket<HashAttr>;
    assert (NewBucket != NULL);
    NewBucket->m_Attr = Attr;
    // Enter the name in m_StringSpace
    NameLength = strlen (Name); 
    NewBucket->m_Name.Length = NameLength;
    NewBucket->m_Name.Index = m_StringSpaceIndex;
    NewSSIndex = m_StringSpaceIndex + NameLength;

    // Check if m_StringSpace needs to be enlarged and do so
    NeedAllocate = ((NewSSIndex / STRING_SPACE_PAGE) + 1) - m_PagesAllocated;
    if (NeedAllocate > 0) {
	m_StringSpace = (char*) realloc (m_StringSpace, 
					(m_PagesAllocated + NeedAllocate) * 
					STRING_SPACE_PAGE * sizeof(char));
	m_PagesAllocated += NeedAllocate;
    }

    strncpy ((char*)(m_StringSpace + m_StringSpaceIndex), Name, NameLength);
    m_StringSpaceIndex += NameLength;

    // Insert the bucket into the table
    NewKey = GetKey (Name);
    NewBucket->m_Next = Table [NewKey];
    Table [NewKey] = NewBucket;
};

template <class HashAttr>
inline void HashTable<HashAttr> :: Remove (const char* Name)
{
HashKeyType Key;
Bucket<HashAttr> * Current, * Parent = NULL;

    Key = GetKey (Name);
    Current = Table [Key];
    while (Current != NULL && !IsEqual (Name, Current)) {
        Parent = Current;
        Current = Current->m_Next;
    }

    if (Current != NULL) {
	if (Parent == NULL)
	    Table [Key] = NULL;
	else
	    Parent->m_Next = Current->m_Next;
	delete Current;
    }
};

template <class HashAttr>
inline HashAttr * HashTable<HashAttr> :: Lookup (const char* Name)
{
HashKeyType Key;
Bucket<HashAttr> * Current;

    Key = GetKey (Name);
    Current = Table [Key];
    while (Current != NULL && !IsEqual (Name, Current))
        Current = Current->m_Next;

    if (Current != NULL)
	return (Current->m_Attr);
    else
	return (NULL);
};

template <class HashAttr>
inline void HashTable<HashAttr> :: Clear (void)
{
HashKeyType Key;
Bucket<HashAttr> * Current;

    for (Key = 0; Key < TableSize; ++ Key) {
	Current = Table [Key];
	while (Current != NULL) {
	    Table [Key] = Current->m_Next;
	    delete Current;
	    Current = Table [Key];
	}
    }
};

template <class HashAttr>
inline HashTable<HashAttr> :: ~HashTable (void)
{
    Clear();
    if (Table != NULL)
	delete [] Table;
    if (m_StringSpace != NULL)
	free (m_StringSpace);
};

#endif

