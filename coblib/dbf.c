// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "dbf.h"
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

//----------------------------------------------------------------------

static void DBF_SetToday (DBF_Header* head);
static int DBF_ReadHeader (DBF_FILE* dfd);
static void DBF_WriteHeader (DBF_FILE* dfd);
#ifndef max
#define max(a,b)	((a) < (b) ? (b) : (a))
#endif

//----------------------------------------------------------------------

/// Opens database in \p filename in given \p mode.
DBF_FILE* DBF_Open (const char* filename, const char* mode)
{
    DBF_FILE* fp = (DBF_FILE*) malloc (sizeof(DBF_FILE));
    strncpy (fp->Filename, filename, sizeof(fp->Filename));
    strncpy (fp->OpenMode, mode, sizeof(fp->OpenMode));
    fp->Filename [sizeof(fp->Filename)-1] = 0;
    fp->OpenMode [sizeof(fp->OpenMode)-1] = 0;
    if ((fp->DataDesc = fopen (filename, mode)) && !DBF_ReadHeader (fp))
	DBF_SeekToFirst (fp);
    else {
	fprintf (stderr, "DBF: Could not open '%s'!\n", filename);
	DBF_Close (fp);
	fp = NULL;
    }
    return (fp);
}

/// Creates a new database in \p filename with \p nFields described in \p Fields.
DBF_FILE* DBF_Create (const char* filename, uint16_t nFields, DBF_Field* Fields)
{
    DBF_FILE* fp = (DBF_FILE*) malloc (sizeof(DBF_FILE));
    strncpy (fp->Filename, filename, sizeof(fp->Filename));
    fp->Filename [sizeof(fp->Filename)-1] = 0;
    strcpy (fp->OpenMode, "wb");
    fp->nFields = nFields;
    fp->Fields = Fields;
    fp->Header.Version = '\x03';			// dBASE IV, no memo field
    fp->Header.nRecords = 0;
    fp->Header.HeaderLength = 32 + nFields * 32;	// Fields take up 32 bytes each, and everything else is 32 bytes together
    fp->Header.RecordLength = 0;
    for (int i = 0; i < nFields; ++i)
	fp->Header.RecordLength += Fields[i].FieldLength;
    ++ fp->Header.RecordLength;	// for reserved byte

    // All header info is now set, create the actual file
    if ((fp->DataDesc = fopen (filename, "wb")))
	DBF_WriteHeader (fp);
    else {
	fprintf (stderr, "DBF: Could not open '%s' for writing!\n", filename);
	DBF_Close (fp);
	fp = NULL;
    }
    return (fp);
}

/// Sets the header access date to today.
static void DBF_SetToday (DBF_Header* h)
{
    time_t TodayTime = time (NULL);
    const struct tm* DateToday = localtime (&TodayTime);
    h->Date.Year = DateToday->tm_year;
    h->Date.Month = DateToday->tm_mon;
    h->Date.Day = DateToday->tm_mday;
}

/// Writes the header to \p dfd.
static void DBF_WriteHeader (DBF_FILE* dfd)
{
    DBF_SetToday (&dfd->Header);
    fseek (dfd->DataDesc, 0, SEEK_SET);

    // First, the static data.
    fwrite (&dfd->Header, 1, 12, dfd->DataDesc);

    // Reserved 20 bytes are all zero
    char dummy [20];
    memset (dummy, 0, 20);
    fwrite (dummy, 1, 20, dfd->DataDesc);

    // Now at 0x20 in the file, start writing fields.
    const uint32_t nFields = (dfd->Header.HeaderLength / 32) - 1;
    for (uint32_t i = 0; i < nFields; ++ i) {
	fwrite (&dfd->Fields[i], 1, 18, dfd->DataDesc);
	fwrite (dummy, 1, 14, dfd->DataDesc);	// The rest are reserved or internal use
    }

    // And terminate the header
    dummy[0] = HEADER_END_CHAR;
    fwrite (dummy, 1, 1, dfd->DataDesc);
}

/// Reads the header into \p dfd.
static int DBF_ReadHeader (DBF_FILE* dfd)
{
    fseek (dfd->DataDesc, 0, SEEK_SET);
    // First, the static data.
    fread (&dfd->Header, 1, 12, dfd->DataDesc);
    if (dfd->Header.Version != 0x03 && dfd->Header.Version != 0x83)
	return (1);	// Incompatible version error
    fseek (dfd->DataDesc, 20, SEEK_CUR); // Reserved 20 bytes are all zero

    // Now at 0x20 in the file, the fields.
    dfd->nFields = (dfd->Header.HeaderLength / 32) - 1;
    dfd->Fields = (DBF_Field*) realloc (dfd->Fields, sizeof(DBF_Field) * dfd->nFields);
    if (!dfd->Fields)
	dfd->nFields = 0;
    for (int i = 0; i < dfd->nFields; ++ i) {
	fread (&dfd->Fields[i], 1, 18, dfd->DataDesc);
	fseek (dfd->DataDesc, 14, SEEK_CUR); // Reserved 14 bytes are all zero
	if (feof (dfd->DataDesc))
	    return (2);	/* Premature end of file */
    }
    fseek (dfd->DataDesc, 1, SEEK_CUR); // And terminate the header
    return (0);
}

/// Seeks to \p record index in \p fp.
void DBF_SeekToRecord (DBF_FILE* fp, uint32_t record)
{
    // First 1 is for end of header, second is for first char of record, reserved for internal use.
    int whence = SEEK_END;
    off_t offset = 0; // point to end of file to flag errors
    if (record < fp->Header.nRecords) {
	offset = fp->Header.HeaderLength + 1 + record * fp->Header.RecordLength;
	whence = SEEK_SET;
    }
    fseek (fp->DataDesc, offset, whence);
}

/// Seeks to the next record start in \p fp.
void DBF_SeekToNext (DBF_FILE* fp)
{
    off_t fpos = ftell (fp->DataDesc);
    uint32_t record = (fpos - (fp->Header.HeaderLength + 1)) / fp->Header.RecordLength;
    DBF_SeekToRecord (fp, record + 1);
}

/// Seeks to the first record in the database.
void DBF_SeekToFirst (DBF_FILE* fp)
{
    DBF_SeekToRecord (fp, 0);
}

/// Seeks to the last record in the database.
void DBF_SeekToLast (DBF_FILE* fp)
{
    DBF_SeekToRecord (fp, fp->Header.nRecords - 1);
}

/// This will append a blank record and point fp to it.
void DBF_AppendRecord (DBF_FILE * fp)
{
    // Go to the end of the file and write a new record filled with zeroes
    fseek (fp->DataDesc, 0, SEEK_END);
    char fc = ' '; // The status character is a space
    fwrite (&fc, 1, 1, fp->DataDesc);
    fc = '\x0';
    for (int i = 0; i < fp->Header.RecordLength; ++ i)
	fwrite (&fc, 1, 1, fp->DataDesc);

    // And increment number of records
    ++fp->Header.nRecords;
    fseek (fp->DataDesc, 0x04, SEEK_SET);
    fwrite (&fp->Header.nRecords, 1, 2, fp->DataDesc);

    // Finally, position file pointer at the beginning of this record
    DBF_SeekToLast (fp);
}

/// Writes record \p data.
void DBF_RewriteRecord (DBF_FILE* fp, const void* data)
{
    fwrite (data, 1, fp->Header.RecordLength, fp->DataDesc);
}

/// Reads record \p data.
void DBF_ReadRecord (DBF_FILE* fp, void* data)
{
    fread (data, 1, fp->Header.RecordLength, fp->DataDesc);
}

/// Deletes current record.
void DBF_DeleteRecord (DBF_FILE* fp)
{
    fseek (fp->DataDesc, -1, SEEK_CUR);	// Move back one char, since pointing to start of data
    const char buffer = '*';
    fwrite (&buffer, 1, 1, fp->DataDesc);
}

/// Removes deleted records from database \p fp.
void DBF_Pack (DBF_FILE* fp)
{
    char TempFilename [sizeof(fp->Filename)];
    strcpy (TempFilename, fp->Filename);
    strcat (TempFilename, ".bak");

    fclose (fp->DataDesc);
    rename (fp->Filename, TempFilename);

    FILE* PackedFile = fopen (fp->Filename, "wb");
    fp->DataDesc = fopen (TempFilename, "rb");

    char* buffer = (char*) malloc (max (fp->Header.HeaderLength + 1, fp->Header.RecordLength));

    // Header goes over intact for now
    fread (buffer, 1, fp->Header.HeaderLength + 1, fp->DataDesc);
    fwrite (buffer, 1, fp->Header.HeaderLength + 1, PackedFile);

    // Now delete records starting with '*'
    uint32_t nPackedRecords = fp->Header.nRecords;
    for (uint32_t i = 0; i < fp->Header.nRecords; ++ i) {
	fread (buffer, 1, fp->Header.RecordLength, fp->DataDesc);
	if (buffer[0] == '*')
	    -- nPackedRecords;
	else
	    fwrite (buffer, 1, fp->Header.RecordLength, PackedFile);
    }

    // Now update the records field at 0x04
    fseek (PackedFile, 0x04, SEEK_SET);
    fwrite (&nPackedRecords, 1, 2, PackedFile);

    // Close files, reopen fp
    fclose (PackedFile);
    fclose (fp->DataDesc);
    fp->DataDesc = fopen (fp->Filename, fp->OpenMode);

    free (buffer);
}

/// Closes the database \p fp.
void DBF_Close (DBF_FILE* fp)
{
    if (fp) {
	if (fp->DataDesc)
	    fclose (fp->DataDesc);
	if (fp->Fields)
	    free (fp->Fields);
	free (fp);
    }
}

