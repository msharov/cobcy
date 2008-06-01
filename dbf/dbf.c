/* dbf.c
**
**	Routines for manipulating dBASE IV files
*/

#include <time.h>
#include <string.h>
#include "dbf.h"

/*-------------------------------------------------------*/
  static void DBF_SetToday (DBF_Header * head);
  static int DBF_ReadHeader (DBF_FILE * dfd);
  static void DBF_WriteHeader (DBF_FILE * dfd);
/*-------------------------------------------------------*/

DBF_FILE * DBF_Open (char * filename, char * mode)
{
DBF_FILE * NewFD;

    NewFD = (DBF_FILE*) malloc (sizeof(DBF_FILE));
    strcpy (NewFD->Filename, filename);
    strcpy (NewFD->OpenMode, mode);
    NewFD->DataDesc = fopen (filename, mode);
    if (NewFD->DataDesc == NULL) {
       fprintf (stderr, "DBF: Could not open '%s'!\n", filename);
       DBF_Close (NewFD);
       return (NULL);
    }

    /* Read the header. No record reading as of yet */
    if (DBF_ReadHeader (NewFD) != 0) {
       DBF_Close (NewFD);
       return (NULL);
    }

    /* Position at the first record */
    DBF_SeekToFirst (NewFD);

    return (NewFD);
}

DBF_FILE * DBF_Create (char * filename, WORD nFields, DBF_Field * Fields)
{
DBF_FILE * NewFD;
int i;

    NewFD = (DBF_FILE*) malloc (sizeof(DBF_FILE));
    strcpy (NewFD->Filename, filename);
    strcpy (NewFD->OpenMode, "wb");
    NewFD->nFields = nFields;
    NewFD->Fields = Fields;
    NewFD->Header.Version = '\x03';	/* dBASE IV, no memo field */
    NewFD->Header.nRecords = 0;
    /* Fields take up 32 bytes each, and everything else is 32 bytes together */
    NewFD->Header.HeaderLength = 32 + nFields * 32;

    /* Now find out the size of each record */
    NewFD->Header.RecordLength = 0;
    for (i = 0; i < nFields; ++ i)
       NewFD->Header.RecordLength += Fields[i].FieldLength;
    /* Add one for reserved byte */
    ++ NewFD->Header.RecordLength;

    /* All header info is now set, create the actual file */
    NewFD->DataDesc = fopen (filename, "wb");
    if (NewFD->DataDesc == NULL) {
       fprintf (stderr, "DBF: Could not open '%s' for writing!\n", filename);
       DBF_Close (NewFD);
       return (NULL);
    }

    /* Write the header. No records as of yet */
    DBF_WriteHeader (NewFD);

    return (NewFD);
}

static void DBF_SetToday (DBF_Header * head)
{
struct tm * DateToday;
time_t TodayTime;

    TodayTime = time (NULL);
    DateToday = localtime (&TodayTime);
    head->Date.Year = DateToday->tm_year;
    head->Date.Month = DateToday->tm_mon;
    head->Date.Day = DateToday->tm_mday;
}

static void DBF_WriteHeader (DBF_FILE * dfd)
{
char dummy [20];
int nFields, i;
DBF_Field * cf;

    /* Have a source of zeroes */
    memset (dummy, 0, 20);

    fseek (dfd->DataDesc, 0, SEEK_SET);
    /* The header starts with a version byte, 0x03 here */
    fwrite (&dfd->Header.Version, 1, 1, dfd->DataDesc);

    /* Then the date of last write access (now) */
    DBF_SetToday (&dfd->Header);
    fwrite (&dfd->Header.Date.Year, 1, 1, dfd->DataDesc);
    fwrite (&dfd->Header.Date.Month, 1, 1, dfd->DataDesc);
    fwrite (&dfd->Header.Date.Day, 1, 1, dfd->DataDesc);

    /* Then goes the number of records. Ditch the endian issue for now */
    fwrite (&dfd->Header.nRecords, 1, 4, dfd->DataDesc);

    /* Then the header and record lengths */
    fwrite (&dfd->Header.HeaderLength, 1, 2, dfd->DataDesc);
    fwrite (&dfd->Header.RecordLength, 1, 2, dfd->DataDesc);

    /* Reserved 20 bytes are all zero */
    fwrite (dummy, 1, 20, dfd->DataDesc);

    /* Now at 0x20 in the file, start writing fields. */
    nFields = (dfd->Header.HeaderLength / 32) - 1;
    for (i = 0; i < nFields; ++ i) {
	cf = &dfd->Fields[i];
	fwrite (cf->Name, 1, 11, dfd->DataDesc);
	fwrite (&cf->Type, 1, 1, dfd->DataDesc);
	fwrite (&cf->Address, 1, 4, dfd->DataDesc);
	fwrite (&cf->FieldLength, 1, 1, dfd->DataDesc);
	fwrite (&cf->DecimalPlaces, 1, 1, dfd->DataDesc);
	/* The rest are reserved or internal use */
	fwrite (dummy, 1, 14, dfd->DataDesc);
    }

    /* And terminate the header */
    dummy[0] = HEADER_END_CHAR;
    fwrite (dummy, 1, 1, dfd->DataDesc);
}

static int DBF_ReadHeader (DBF_FILE * dfd)
{
char dummy [20];
int i;
DBF_Field * cf;

    fseek (dfd->DataDesc, 0, SEEK_SET);
    /* The header starts with a version byte, 0x03 here */
    fread (&dfd->Header.Version, 1, 1, dfd->DataDesc);
    if (dfd->Header.Version != 0x03)
	return (1);	/* Incompatible version error */

    /* Then the date of last write access */
    fread (&dfd->Header.Date.Year, 1, 1, dfd->DataDesc);
    fread (&dfd->Header.Date.Month, 1, 1, dfd->DataDesc);
    fread (&dfd->Header.Date.Day, 1, 1, dfd->DataDesc);

    /* Then goes the number of records. Ditch the endian issue for now */
    fread (&dfd->Header.nRecords, 1, 4, dfd->DataDesc);

    /* Then the header and record lengths */
    fread (&dfd->Header.HeaderLength, 1, 2, dfd->DataDesc);
    fread (&dfd->Header.RecordLength, 1, 2, dfd->DataDesc);

    /* Reserved 20 bytes are all zero */
    fread (dummy, 1, 20, dfd->DataDesc);

    /* Now at 0x20 in the file, start reading fields. */
    dfd->nFields = (dfd->Header.HeaderLength / 32) - 1;
    dfd->Fields = (DBF_Field*) malloc (sizeof(DBF_Field) * dfd->nFields);
    for (i = 0; i < dfd->nFields; ++ i) {
	cf = &dfd->Fields[i];
	fread (cf->Name, 1, 11, dfd->DataDesc);
	fread (&cf->Type, 1, 1, dfd->DataDesc);
	fread (&cf->Address, 1, 4, dfd->DataDesc);
	fread (&cf->FieldLength, 1, 1, dfd->DataDesc);
	fread (&cf->DecimalPlaces, 1, 1, dfd->DataDesc);
	/* The rest are reserved or internal use */
	fread (dummy, 1, 14, dfd->DataDesc);
	if (feof (dfd->DataDesc))
	    return (2);	/* Premature end of file */
    }

    /* And terminate the header */
    fread (dummy, 1, 1, dfd->DataDesc);

    /* If nothing bad happened to this point, its fine */
    return (0);
}

void DBF_SeekToRecord (DBF_FILE * fp, WORD record)
{
WORD offset;

    /* First 1 is for end of header, second is for first char of record,
    ** reserved for internal use.
    */

    /* If requesting an existing record, point to its beginning, */
    if (record < fp->Header.nRecords) {
	offset = fp->Header.HeaderLength + 1 + record * fp->Header.RecordLength;
	fseek (fp->DataDesc, offset, SEEK_SET);
    }
    else /* otherwise point to end of file to flag the error */
	fseek (fp->DataDesc, 0, SEEK_END);
}

void DBF_SeekToNext (DBF_FILE * fp)
{
WORD fpos, record;
  
    fpos = ftell (fp->DataDesc);
    record = (double)(fpos - (fp->Header.HeaderLength + 1)) / 
    			(double) fp->Header.RecordLength;
    DBF_SeekToRecord (fp, record + 1);
}

void DBF_SeekToFirst (DBF_FILE * fp)
{
    DBF_SeekToRecord (fp, 0);
}

void DBF_SeekToLast (DBF_FILE * fp)
{
    DBF_SeekToRecord (fp, fp->Header.nRecords);
}

/*
** This will append a blank record at the end of file and position the
** file pointer at its beginning.
*/
void DBF_AppendRecord (DBF_FILE * fp)
{
char fc = ' ';
int i;

    /* Go to the end of the file and write a new record filled with zeroes */
    fseek (fp->DataDesc, 0, SEEK_END);
    /* The status character is a space */
    fwrite (&fc, 1, 1, fp->DataDesc);
    fc = '\x0';
    for (i = 0; i < fp->Header.RecordLength; ++ i)
	fwrite (&fc, 1, 1, fp->DataDesc);

    /* And increment number of records */
    ++ fp->Header.nRecords;
    fseek (fp->DataDesc, 0x04, SEEK_SET);
    fwrite (&fp->Header.nRecords, 1, 2, fp->DataDesc);

    /* Finally, position file pointer at the beginning of this record */
    /* It is the last record (duh!) so its number is nRecords - 1     */
    DBF_SeekToRecord (fp, fp->Header.nRecords - 1);
}

void DBF_RewriteRecord (DBF_FILE * fp, void * data)
{
    fwrite (data, 1, fp->Header.RecordLength, fp->DataDesc);
}

void DBF_ReadRecord (DBF_FILE * fp, void * data)
{
    fread (data, 1, fp->Header.RecordLength, fp->DataDesc);
}

void DBF_DeleteRecord (DBF_FILE * fp)
{
unsigned char buffer = '*';
    /* Move back one char, since pointing to start of data */
    fseek (fp->DataDesc, -1, SEEK_CUR);
    fwrite (&buffer, 1, 1, fp->DataDesc);
}

void DBF_Pack (DBF_FILE * fp)
{
FILE * PackedFile = NULL;
char TempFilename [PATH_MAX];
char * buffer;
WORD i, nPackedRecords;

    strcpy (TempFilename, fp->Filename);
    strcat (TempFilename, ".bak");

    fclose (fp->DataDesc);
    rename (fp->Filename, TempFilename);

    PackedFile = fopen (fp->Filename, "wb");
    fp->DataDesc = fopen (TempFilename, "rb");

    buffer = (char*) malloc (max (fp->Header.HeaderLength + 1, 
    				  fp->Header.RecordLength));

    /* Header goes over intact for now */
    fread (buffer, 1, fp->Header.HeaderLength + 1, fp->DataDesc);
    fwrite (buffer, 1, fp->Header.HeaderLength + 1, PackedFile);

    /* Now delete records starting with '*' */
    nPackedRecords = fp->Header.nRecords;
    for (i = 0; i < fp->Header.nRecords; ++ i) {
       fread (buffer, 1, fp->Header.RecordLength, fp->DataDesc);
       if (buffer[0] == '*')
	  -- nPackedRecords;
       else
	  fwrite (buffer, 1, fp->Header.RecordLength, PackedFile);
    }

    /* Now update the records field at 0x04 */
    fseek (PackedFile, 0x04, SEEK_SET);
    fwrite (&nPackedRecords, 1, 2, PackedFile);

    /* Close files, reopen fp */
    fclose (PackedFile);
    fclose (fp->DataDesc);
    fp->DataDesc = fopen (fp->Filename, fp->OpenMode);

    free (buffer);
}

void DBF_Close (DBF_FILE * fp)
{
    if (fp != NULL) {
       if (fp->DataDesc != NULL)
	  fclose (fp->DataDesc);
       if (fp->Fields != NULL)
	  free (fp->Fields);
       free (fp);
    }
}

