/* ndx.c
**
**	Implementation of the dBASE III .NDX file format
** The unusual thing is doing it without buffered streams (fopen)
** the reason is that index files need read/write access and
** buffered streams cannot provide that (no, "w+b" does NOT work!)
*/

#include <malloc.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#ifndef __MSDOS__
#include <unistd.h>
#endif
#include "ndx.h"

/*--------------------------------------------------------------------------*/
  static void 	NDX_ReadHeader (NDX_FILE * fp);
  static void 	NDX_WriteHeader (NDX_FILE * fp);
  static void 	NDX_ReadPage (NDX_FILE * fp, int PageIndex);
  static void 	NDX_WritePage (NDX_FILE * fp, int PageIndex);
  static void 	NDX_FreePage (NDX_FILE * fp);
  static int 	NDX_AddPage (NDX_FILE * fp);
  static int 	NDX_FindKeyPlace (NDX_FILE * fp, char * key, int AddPage);
/*--------------------------------------------------------------------------*/

static void NDX_ReadHeader (NDX_FILE * fp)
{
int reserved;
    lseek (fp->FileDesc, 0, SEEK_SET);
    read (fp->FileDesc, &fp->RootPageOffset, 4);
    read (fp->FileDesc, &fp->nPages, 4);
    read (fp->FileDesc, &reserved, 4);
    read (fp->FileDesc, &fp->KeyLength, 2);
    assert (fp->KeyLength < NDX_PAGE_SIZE / 4);
    read (fp->FileDesc, &fp->KeysPerPage, 2);
    read (fp->FileDesc, &fp->KeyType, 2);
    read (fp->FileDesc, &reserved, 2);
    read (fp->FileDesc, &fp->KeyName, 488);
}

static void NDX_WriteHeader (NDX_FILE * fp)
{
int reserved = 0;
    lseek (fp->FileDesc, 0, SEEK_SET);
    write (fp->FileDesc, &fp->RootPageOffset, 4);
    write (fp->FileDesc, &fp->nPages, 4);
    write (fp->FileDesc, &reserved, 4);
    write (fp->FileDesc, &fp->KeyLength, 2);
    write (fp->FileDesc, &fp->KeysPerPage, 2);
    write (fp->FileDesc, &fp->KeyType, 2);
    write (fp->FileDesc, &reserved, 2);
    write (fp->FileDesc, &fp->KeyName, 488);
}

static void NDX_ReadPage (NDX_FILE * fp, int PageIndex)
{
int i, BytesRead;
NDX_INDEX_ITEM * CurRec;

    lseek (fp->FileDesc, PageIndex * NDX_PAGE_SIZE, SEEK_SET);

    BytesRead = read (fp->FileDesc, &fp->RecordsInPage, 4);
    if (BytesRead != 4) {
	perror ("NDX_ReadPage");
	exit (1);
    }

    for (i = 0; i < fp->KeysPerPage; ++ i) {
	CurRec = &fp->CurPage[i];
	read (fp->FileDesc, &CurRec->LeftPage, 4);
	read (fp->FileDesc, &CurRec->DBFRecordNumber, 4);
	read (fp->FileDesc, CurRec->KeyData, fp->KeyLength);
    }

    fp->PageLoaded = PageIndex;
}

static void NDX_WritePage (NDX_FILE * fp, int PageIndex)
{
int i;
NDX_INDEX_ITEM * CurRec;

    lseek (fp->FileDesc, PageIndex * NDX_PAGE_SIZE, SEEK_SET);

    write (fp->FileDesc, &fp->RecordsInPage, 4);
    for (i = 0; i < fp->KeysPerPage; ++ i) {
	CurRec = &fp->CurPage[i];
	write (fp->FileDesc, &CurRec->LeftPage, 4);
	write (fp->FileDesc, &CurRec->DBFRecordNumber, 4);
	write (fp->FileDesc, CurRec->KeyData, fp->KeyLength);
    }
}

static void NDX_FreePage (NDX_FILE * fp)
{
int i;
    if (fp->CurPage != NULL) {
	for (i = 0; i < fp->KeysPerPage; ++ i)
	    free (fp->CurPage[i].KeyData);
	free (fp->CurPage);
	fp->CurPage = NULL;
    }
    fp->PageLoaded = -1;
}

static void NDX_AllocatePage (NDX_FILE * fp)
{
int i;
NDX_INDEX_ITEM * CurRec;

    fp->CurPage = (NDX_INDEX_ITEM*) calloc (1, fp->KeysPerPage * 
    					(2 * sizeof(int) + fp->KeyLength));
    for (i = 0; i < fp->KeysPerPage; ++ i) {
	CurRec = &fp->CurPage[i];
	CurRec->KeyData = (char*) calloc (1, fp->KeyLength);
    }
}

NDX_FILE * NDX_Open (char * filename, char * mode)
{
NDX_FILE * fp;
int imode;

    fp = (NDX_FILE*) malloc (sizeof(NDX_FILE));
    fp->CurPage = NULL;
    fp->PageLoaded = -1;
    fp->RecordsInPage = 0;

    /* Need read access all the time, so... */
    if (mode[0] == 'w')
	imode = O_RDWR | O_TRUNC;
    else
	imode = O_RDONLY;

    fp->FileDesc = open (filename, imode, 0666);
    assert (fp->FileDesc >= 0);

    NDX_ReadHeader (fp);

    /* Load the root index page */
    NDX_AllocatePage (fp);
    NDX_ReadPage (fp, fp->RootPageOffset);

    return (fp);
}

NDX_FILE * NDX_Create (char * filename, char * keyname,
		int keytype, int keylength)
{
NDX_FILE * fp;

    fp = (NDX_FILE*) malloc (sizeof(NDX_FILE));
    fp->CurPage = NULL;
    fp->PageLoaded = -1;
    fp->RecordsInPage = 0;

    if (access (filename, 0) == 0)
	return (NULL);

    fp->FileDesc = open (filename, O_RDWR | O_CREAT | O_TRUNC, 0666);
    assert (fp->FileDesc >= 0);

    fp->KeyType = keytype;
    fp->KeyLength = keylength;
    strcpy (fp->KeyName, keyname);
    fp->KeysPerPage = (NDX_PAGE_SIZE - sizeof(int)) / (8 + keylength);

    fp->KeyType = keytype;
    /* Write the header twice, first to set the end for header */
    NDX_WriteHeader (fp);
    fp->RootPageOffset = NDX_AddPage (fp);
    NDX_WriteHeader (fp);

    /* Load the root index page */
    NDX_AllocatePage (fp);
    NDX_ReadPage (fp, fp->RootPageOffset);

    return (fp);
}

static int NDX_AddPage (NDX_FILE * fp)
{
int PagePos;
char PageFiller [NDX_PAGE_SIZE];

    PagePos = fp->nPages + 1;
    lseek (fp->FileDesc, PagePos * NDX_PAGE_SIZE, SEEK_SET);
    memset (PageFiller, 0, NDX_PAGE_SIZE);
    write (fp->FileDesc, PageFiller, NDX_PAGE_SIZE);
    ++ fp->nPages;
    return (PagePos);
}

static int NDX_FindKeyPlace (NDX_FILE * fp, char * key, int AddPage)
{
int DBF_RecordIndex = -1, IndexInPage = -1;
int NewPageIndex, i;

    NewPageIndex = fp->RootPageOffset;
    while (NewPageIndex > 0 && DBF_RecordIndex == -1) {
	if (fp->PageLoaded != NewPageIndex)
	    NDX_ReadPage (fp, NewPageIndex);

	IndexInPage = -1;
	for (i = 0; i < fp->RecordsInPage && IndexInPage == -1; ++ i)
	    if (strncmp (fp->CurPage[i].KeyData, key, fp->KeyLength) >= 0)
		IndexInPage = i;
	if (IndexInPage == -1) {
	    IndexInPage = fp->RecordsInPage;
	    if (fp->RecordsInPage == fp->KeysPerPage)
		-- IndexInPage;
	    else
		break;
	}
	if (strncmp (fp->CurPage [IndexInPage].KeyData,key, fp->KeyLength) == 0)
	    DBF_RecordIndex = IndexInPage;
	else {
	    if (fp->CurPage [IndexInPage].LeftPage == 0 &&
		fp->RecordsInPage == fp->KeysPerPage && AddPage == 1) 
	    {
		fp->CurPage [IndexInPage].LeftPage = NDX_AddPage (fp);
		NDX_WritePage (fp, fp->PageLoaded);
	    }
	    NewPageIndex = fp->CurPage [IndexInPage].LeftPage;
	}
    }

    if (NewPageIndex == 0)
	DBF_RecordIndex = IndexInPage;

    return (DBF_RecordIndex);
}

void NDX_InsertKey (NDX_FILE * fp, char * key, int recnum)
{
int IndexInPage, i;

    IndexInPage = NDX_FindKeyPlace (fp, key, 1);

    assert (fp->RecordsInPage < fp->KeysPerPage);

    /* returned -1 means to append at the end */
    if (IndexInPage < 0)
	IndexInPage = fp->RecordsInPage;
    else {
	for (i = fp->RecordsInPage; i > IndexInPage; -- i) {
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

int NDX_LookupKey (NDX_FILE * fp, char * key)
{
int IndexInPage;

    IndexInPage = NDX_FindKeyPlace (fp, key, 0);
    if (IndexInPage >= 0 && IndexInPage < fp->RecordsInPage &&
	strncmp (fp->CurPage [IndexInPage].KeyData, key, fp->KeyLength) == 0)
    {
	return (fp->CurPage [IndexInPage].DBFRecordNumber);
    }
    else
	return (-1);
}

void NDX_Close (NDX_FILE ** fp)
{
    if (fp == NULL || *fp == NULL)
	return;

    close ((*fp)->FileDesc);
    NDX_FreePage (*fp);
    free (*fp);
    *fp = NULL;
}

