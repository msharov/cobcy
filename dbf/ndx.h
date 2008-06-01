/* ndx.h
**
**	This is an implementation of an index file for Cobcy.
*/

#ifndef __NDX_H
#define __NDX_H

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define NDX_KEY_NAME_LENGTH	488	/* Yup. it can be THAT big. */
#define NDX_PAGE_SIZE		512

typedef struct {
    int		LeftPage;
    int		DBFRecordNumber;
    char *	KeyData;
} NDX_INDEX_ITEM;

typedef struct {
    int 	FileDesc;
    char	KeyName [NDX_KEY_NAME_LENGTH];
    int		KeyLength;
    int		KeysPerPage;
    int		KeyType;
    int		nPages;
    int		RootPageOffset;

    int		PageLoaded;
    int		RecordsInPage;
    NDX_INDEX_ITEM *	CurPage;	
} NDX_FILE;

/*---------------------------------------------------------------*/
  NDX_FILE *	NDX_Open (char * filename, char * mode);
  NDX_FILE * 	NDX_Create (char * filename, char * keyname, 
  			int keytype, int keylength);
  void	NDX_InsertKey (NDX_FILE * fp, char * key, int recnum);
  int	NDX_LookupKey (NDX_FILE * fp, char * key);
  void	NDX_Close (NDX_FILE ** fp);
/*---------------------------------------------------------------*/

#endif

