// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// Implementation of the dBASE III .NDX file format
// The unusual thing is doing it without buffered streams (fopen)
// the reason is that index files need read/write access and
// buffered streams cannot provide that (no, "w+b" does NOT work!)

#include "cobfunc.h"
#include <errno.h>
#include <fcntl.h>

//----------------------------------------------------------------------

static void NDX_ReadHeader (NDX_FILE* fp);
static void NDX_WriteHeader (NDX_FILE* fp);
static void NDX_ReadPage (NDX_FILE* fp, uint32_t PageIndex);
static void NDX_WritePage (NDX_FILE* fp, uint32_t PageIndex);
static void NDX_FreePage (NDX_FILE* fp);
static int NDX_AddPage (NDX_FILE* fp);
static int NDX_FindKeyPlace (NDX_FILE* fp, const char* key, int AddPage);

//----------------------------------------------------------------------

static void NDX_ReadHeader (NDX_FILE* fp)
{
    lseek (fp->FileDesc, 0, SEEK_SET);
    read (fp->FileDesc, &fp->Info, sizeof(fp->Info));
}

static void NDX_WriteHeader (NDX_FILE* fp)
{
    lseek (fp->FileDesc, 0, SEEK_SET);
    write (fp->FileDesc, &fp->Info, sizeof(fp->Info));
}

static void NDX_ReadPage (NDX_FILE* fp, uint32_t PageIndex)
{
    lseek (fp->FileDesc, PageIndex * NDX_PAGE_SIZE, SEEK_SET);
    ssize_t BytesRead = read (fp->FileDesc, &fp->RecordsInPage, 4);
    if (BytesRead != 4)
	_RuntimeError ("NDX_ReadPage: %s", strerror(errno));
    for (uint32_t i = 0; i < fp->Info.KeysPerPage; ++i) {
	NDX_INDEX_ITEM* CurRec = &fp->CurPage[i];
	read (fp->FileDesc, &CurRec->LeftPage, 4);
	read (fp->FileDesc, &CurRec->DBFRecordNumber, 4);
	read (fp->FileDesc, CurRec->KeyData, fp->Info.KeyLength);
    }
    fp->PageLoaded = PageIndex;
}

static void NDX_WritePage (NDX_FILE* fp, uint32_t PageIndex)
{
    lseek (fp->FileDesc, PageIndex * NDX_PAGE_SIZE, SEEK_SET);
    write (fp->FileDesc, &fp->RecordsInPage, 4);
    for (uint32_t i = 0; i < fp->Info.KeysPerPage; ++i) {
	const NDX_INDEX_ITEM* CurRec = &fp->CurPage[i];
	write (fp->FileDesc, &CurRec->LeftPage, 4);
	write (fp->FileDesc, &CurRec->DBFRecordNumber, 4);
	write (fp->FileDesc, CurRec->KeyData, fp->Info.KeyLength);
    }
}

static void NDX_FreePage (NDX_FILE* fp)
{
    if (fp->CurPage) {
	for (uint32_t i = 0; i < fp->Info.KeysPerPage; ++i)
	    free (fp->CurPage[i].KeyData);
	free (fp->CurPage);
	fp->CurPage = NULL;
    }
    fp->PageLoaded = -1;
}

static void NDX_AllocatePage (NDX_FILE* fp)
{
    fp->CurPage = _AllocateBytes (fp->Info.KeysPerPage * sizeof(NDX_INDEX_ITEM));
    for (uint32_t i = 0; i < fp->Info.KeysPerPage; ++ i)
	fp->CurPage[i].KeyData = _AllocateBytes (fp->Info.KeyLength);
}

NDX_FILE* NDX_Open (const char* filename, const char* mode)
{
    int imode = O_RDONLY;
    if (mode[0] == 'w')
	imode = O_RDWR;
    int fd = open (filename, imode);
    if (fd < 0)
	return NULL;

    NDX_FILE* fp = _AllocateBytes (sizeof(NDX_FILE));
    fp->PageLoaded = -1;
    fp->FileDesc = fd;

    NDX_ReadHeader (fp);
    NDX_AllocatePage (fp);	// Load the root index page
    NDX_ReadPage (fp, fp->Info.RootPageOffset);
    return fp;
}

NDX_FILE* NDX_Create (const char* filename, const char* keyname, int keytype, int keylength)
{
    if (access (filename, 0) == 0)
	return NULL;
    int fd = open (filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    if (fd < 0)
	return NULL;

    NDX_FILE* fp = _AllocateBytes (sizeof(NDX_FILE));
    fp->PageLoaded = -1;
    fp->FileDesc = fd;

    fp->Info.KeyType = keytype;
    fp->Info.KeyLength = keylength;
    strcpy (fp->Info.KeyName, keyname);
    fp->Info.KeysPerPage = (NDX_PAGE_SIZE - 4) / (8 + keylength);
    fp->Info.KeyType = keytype;

    // Write the header twice, first to set the end for header
    NDX_WriteHeader (fp);
    fp->Info.RootPageOffset = NDX_AddPage (fp);
    NDX_WriteHeader (fp);

    // Load the root index page
    NDX_AllocatePage (fp);
    NDX_ReadPage (fp, fp->Info.RootPageOffset);
    return fp;
}

static int NDX_AddPage (NDX_FILE * fp)
{
    uint32_t PagePos = fp->Info.nPages + 1;
    lseek (fp->FileDesc, PagePos * NDX_PAGE_SIZE, SEEK_SET);
    char PageFiller [NDX_PAGE_SIZE];
    memset (PageFiller, 0, sizeof(PageFiller));
    write (fp->FileDesc, PageFiller, NDX_PAGE_SIZE);
    ++ fp->Info.nPages;
    return PagePos;
}

static int NDX_FindKeyPlace (NDX_FILE* fp, const char* key, int AddPage)
{
    int DBF_RecordIndex = -1, IndexInPage = -1;
    uint32_t NewPageIndex = fp->Info.RootPageOffset;
    while (NewPageIndex > 0 && DBF_RecordIndex == -1) {
	if (fp->PageLoaded != NewPageIndex)
	    NDX_ReadPage (fp, NewPageIndex);

	IndexInPage = -1;
	for (uint32_t i = 0; i < fp->RecordsInPage && IndexInPage == -1; ++ i)
	    if (strncmp (fp->CurPage[i].KeyData, key, fp->Info.KeyLength) >= 0)
		IndexInPage = i;
	if (IndexInPage == -1) {
	    IndexInPage = fp->RecordsInPage;
	    if (fp->RecordsInPage != fp->Info.KeysPerPage)
		break;
	    -- IndexInPage;
	}
	if (strncmp (fp->CurPage [IndexInPage].KeyData,key, fp->Info.KeyLength) == 0)
	    DBF_RecordIndex = IndexInPage;
	else {
	    if (fp->CurPage [IndexInPage].LeftPage == 0 && fp->RecordsInPage == fp->Info.KeysPerPage && AddPage == 1) {
		fp->CurPage [IndexInPage].LeftPage = NDX_AddPage (fp);
		NDX_WritePage (fp, fp->PageLoaded);
	    }
	    NewPageIndex = fp->CurPage [IndexInPage].LeftPage;
	}
    }
    if (NewPageIndex == 0)
	DBF_RecordIndex = IndexInPage;
    return DBF_RecordIndex;
}

void NDX_InsertKey (NDX_FILE* fp, const char* key, int recnum)
{
    assert (fp->RecordsInPage < fp->Info.KeysPerPage);

    int IndexInPage = NDX_FindKeyPlace (fp, key, 1);
    if (IndexInPage < 0) // returned -1 means to append at the end
	IndexInPage = fp->RecordsInPage;
    else {
	for (int i = fp->RecordsInPage; i > IndexInPage; -- i) {
	    fp->CurPage[i].LeftPage = fp->CurPage[i - 1].LeftPage;
	    fp->CurPage[i].DBFRecordNumber = fp->CurPage[i - 1].DBFRecordNumber;
	    strcpy (fp->CurPage[i].KeyData, fp->CurPage[i - 1].KeyData);
	}
    }

    strcpy (fp->CurPage[IndexInPage].KeyData, key);
    fp->CurPage[IndexInPage].LeftPage = 0;
    fp->CurPage[IndexInPage].DBFRecordNumber = recnum;
    ++ fp->RecordsInPage;
    NDX_WritePage (fp, fp->PageLoaded);
}

int NDX_LookupKey (NDX_FILE* fp, const char* key)
{
    int IndexInPage = NDX_FindKeyPlace (fp, key, 0);
    if (IndexInPage >= 0 && IndexInPage < (int) fp->RecordsInPage &&
	strncmp (fp->CurPage [IndexInPage].KeyData, key, fp->Info.KeyLength) == 0)
	return fp->CurPage [IndexInPage].DBFRecordNumber;
    else
	return -1;
}

void NDX_Close (NDX_FILE** fp)
{
    if (!fp || !*fp)
	return;
    close ((*fp)->FileDesc);
    NDX_FreePage (*fp);
    free (*fp);
    *fp = NULL;
}

