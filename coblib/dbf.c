// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "cobfunc.h"
#include <time.h>
#include <sys/types.h>

//----------------------------------------------------------------------

static void DBF_SetToday (DBF_Header* head);
static int DBF_ReadHeader (DBF_FILE* dfd);
static void DBF_WriteHeader (DBF_FILE* dfd);

//----------------------------------------------------------------------

/// Opens database in \p filename in given \p mode.
DBF_FILE* DBF_Open (const char* filename, const char* mode)
{
    DBF_FILE* fp = _AllocateBytes (sizeof(DBF_FILE));
    snprintf (fp->Filename, sizeof(fp->Filename), "%s", filename);
    snprintf (fp->OpenMode, sizeof(fp->OpenMode), "%s", mode);
    if ((fp->DataDesc = fopen (filename, mode)) && !DBF_ReadHeader (fp))
	DBF_SeekToFirst (fp);
    else {
	fprintf (stderr, "DBF: Could not open '%s'!\n", filename);
	DBF_Close (fp);
	fp = NULL;
    }
    return fp;
}

/// Creates a new database in \p filename with \p nFields described in \p Fields.
DBF_FILE* DBF_Create (const char* filename, uint16_t nFields, DBF_Field* Fields)
{
    DBF_FILE* fp = _AllocateBytes (sizeof(DBF_FILE));
    snprintf (fp->Filename, sizeof(fp->Filename), "%s", filename);
    strcpy (fp->OpenMode, "wb");
    fp->nFields = nFields;
    fp->Fields = Fields;
    fp->Header.Version = '\x03';			// dBASE IV, no memo field
    fp->Header.nRecords = 0;
    fp->Header.HeaderLength = 32 + nFields * 32;	// Field descriptions take up 32 bytes each, and everything else is 32 bytes together
    fp->Header.RecordLength = 0;
    for (unsigned i = 0; i < nFields; ++i)
	fp->Header.RecordLength += Fields[i].FieldLength;
    ++fp->Header.RecordLength;	// for reserved byte at the end

    // All header info is now set, create the actual file
    if ((fp->DataDesc = fopen (fp->Filename, fp->OpenMode)))
	DBF_WriteHeader (fp);
    else {
	fprintf (stderr, "DBF: Could not open '%s' for writing!\n", filename);
	DBF_Close (fp);
	fp = NULL;
    }
    return fp;
}

/// Sets the header access date to today.
static void DBF_SetToday (DBF_Header* h)
{
    time_t today = time (NULL);
    const struct tm* todate = localtime (&today);
    h->Date.Year = todate->tm_year;
    h->Date.Month = todate->tm_mon;
    h->Date.Day = todate->tm_mday;
}

/// Writes the header to \p dfd.
static void DBF_WriteHeader (DBF_FILE* dfd)
{
    DBF_SetToday (&dfd->Header);
    fseek (dfd->DataDesc, 0, SEEK_SET);

    // First, the static data.
    fwrite (&dfd->Header, 1, sizeof(dfd->Header), dfd->DataDesc);

    // Reserved 20 zero bytes
    static const char zeroes[20] = {};
    fwrite (zeroes, 1, 20, dfd->DataDesc);

    // Now at 0x20 in the file, start writing fields.
    const unsigned nFields = (dfd->Header.HeaderLength / 32) - 1;
    for (unsigned i = 0; i < nFields; ++i) {
	fwrite (&dfd->Fields[i], 1, 18, dfd->DataDesc);
	fwrite (zeroes, 1, 14, dfd->DataDesc);	// Reserved 14 zeroes
    }

    fputc (HEADER_END_CHAR, dfd->DataDesc);
}

/// Reads the header into \p dfd.
static int DBF_ReadHeader (DBF_FILE* dfd)
{
    fseek (dfd->DataDesc, 0, SEEK_SET);
    // First, the static data.
    fread (&dfd->Header, 1, sizeof(dfd->Header), dfd->DataDesc);
    if (dfd->Header.Version != 0x03 && dfd->Header.Version != 0x83)
	return 1;	// incompatible version
    fseek (dfd->DataDesc, 20, SEEK_CUR); // skip 20 reserved bytes

    // Now at 0x20 in the file, the fields.
    dfd->nFields = (dfd->Header.HeaderLength / 32) - 1;
    dfd->Fields = _ReallocateBytes (dfd->Fields, sizeof(DBF_Field) * dfd->nFields);
    for (unsigned i = 0; i < dfd->nFields; ++i) {
	fread (&dfd->Fields[i], 1, 18, dfd->DataDesc);
	fseek (dfd->DataDesc, 14, SEEK_CUR); // skip 14 reserved bytes
	if (feof (dfd->DataDesc))
	    return 2;	// premature end of file
    }
    if (HEADER_END_CHAR != fgetc (dfd->DataDesc))
	return 3;	// header not terminated
    return 0;
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
void DBF_AppendRecord (DBF_FILE* fp)
{
    // Go to the end of the file and write a new record filled with zeroes
    fseek (fp->DataDesc, 0, SEEK_END);
    fputc (' ', fp->DataDesc);	// ' ' status indicates used record
    for (unsigned i = 0; i < fp->Header.RecordLength; ++i)
	fputc (0, fp->DataDesc);	// write zeroed record

    // Increment number of records
    ++fp->Header.nRecords;
    fseek (fp->DataDesc, 0x04, SEEK_SET);
    fwrite (&fp->Header.nRecords, 1, sizeof(fp->Header.nRecords), fp->DataDesc);

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
    // Pointing to start of data, so move back 1 to the status char
    fseek (fp->DataDesc, -1, SEEK_CUR);
    fputc ('*', fp->DataDesc);
}

/// Removes deleted records from database \p fp.
void DBF_Pack (DBF_FILE* fp)
{
    char tmpfilename [PATH_MAX];
    snprintf (tmpfilename, sizeof(tmpfilename), "%s.XXXXXX", fp->Filename);
    int fd = mkstemp (tmpfilename);
    if (fd < 0)
	return;
    DBF_FILE outfp = *fp;
    outfp.DataDesc = fdopen (fd, "wb");
    if (!outfp.DataDesc)
	return;

    DBF_WriteHeader (&outfp);
    DBF_SeekToFirst (fp);
    char* recordbuf = _AllocateBytes (fp->Header.RecordLength);
    for (unsigned i = 0; i < fp->Header.nRecords; ++i) {
	DBF_ReadRecord (fp, recordbuf);
	if (recordbuf[0] == '*')
	    --outfp.Header.nRecords;
	else
	    DBF_RewriteRecord (&outfp, recordbuf);
    }
    DBF_WriteHeader (&outfp);	// rewrite number of records

    fclose (outfp.DataDesc);
    fclose (fp->DataDesc);
    rename (tmpfilename, fp->Filename);
    fp->DataDesc = fopen (fp->Filename, fp->OpenMode);
}

/// Closes the database \p fp.
void DBF_Close (DBF_FILE* fp)
{
    if (fp) {
	if (fp->DataDesc)
	    fclose (fp->DataDesc);
	if (fp->Fields)
	    free (fp->Fields);
	memset (fp, 0, sizeof(DBF_FILE));
	free (fp);
    }
}
