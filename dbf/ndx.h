// ndx.h
//
//	This is an implementation of an index file for Cobcy.
//

#ifndef NDX_H_090C07BB6A4BA18A2304CEC370541651
#define NDX_H_090C07BB6A4BA18A2304CEC370541651

#include <stdint.h>

#define NDX_KEY_NAME_LENGTH	488	// Yup. it can be THAT big.
#define NDX_PAGE_SIZE		512

typedef struct {
    uint32_t	LeftPage;
    uint32_t	DBFRecordNumber;
    char*	KeyData;
} NDX_INDEX_ITEM;

typedef struct {
    uint32_t	RootPageOffset;
    uint32_t	nPages;
    uint32_t	m_Reserved1;
    uint16_t	KeyLength;
    uint16_t	KeysPerPage;
    uint16_t	KeyType;
    uint16_t	m_Reserved2;
    char	KeyName [NDX_KEY_NAME_LENGTH];
} NDX_HEADER;

typedef struct {
    NDX_HEADER		Info;
    int 		FileDesc;
    uint32_t		PageLoaded;
    uint32_t		RecordsInPage;
    NDX_INDEX_ITEM*	CurPage;
} NDX_FILE;

//----------------------------------------------------------------------

NDX_FILE* NDX_Open (const char* filename, const char* mode);
NDX_FILE* NDX_Create (const char* filename, const char* keyname, int keytype, int keylength);
void NDX_InsertKey (NDX_FILE* fp, const char* key, int recnum);
int NDX_LookupKey (NDX_FILE* fp, const char* key);
void NDX_Close (NDX_FILE** fp);

//----------------------------------------------------------------------

#endif

