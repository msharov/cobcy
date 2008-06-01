/* dbf.h
**
**	Defines ANSI C structures to deal with dBASE IV files.
*/

#ifndef __DBF_H
#define __DBF_H

#include "../adtlib/mdefs.h"
#include <stdio.h>

#define HEADER_END_CHAR		'\x0D'
#define DBF_FIELD_NAME_LENGTH	11

typedef struct {
    BYTE		Version;
    struct {
	WORD	Year;
        WORD	Month;
	WORD	Day;
    }			Date;
    WORD		nRecords;
    WORD		HeaderLength;
    WORD		RecordLength;
} DBF_Header;

typedef struct {
    BYTE		Name [DBF_FIELD_NAME_LENGTH];
    BYTE		Type;
    WORD		Address;
    BYTE		FieldLength;
    BYTE		DecimalPlaces;
} DBF_Field;

typedef struct {
    FILE *		DataDesc;
    DBF_Header		Header;
    WORD		nFields;
    DBF_Field *		Fields;
    char		Filename [PATH_MAX];
    char		OpenMode [5];
} DBF_FILE;

/*-------------------------------------------------------------------------*/
  DBF_FILE *	DBF_Open (char * filename, char * mode);
  DBF_FILE *	DBF_Create (char * filename, WORD nFields, DBF_Field * def);
  void		DBF_SeekToRecord (DBF_FILE * fp, WORD record);
  void		DBF_SeekToNext (DBF_FILE * fp);
  void		DBF_SeekToFirst (DBF_FILE * fp);
  void		DBF_SeekToLast (DBF_FILE * fp);
  void		DBF_AppendRecord (DBF_FILE * fp);
  void		DBF_RewriteRecord (DBF_FILE * fp, void * data);
  void		DBF_ReadRecord (DBF_FILE * fp, void * data);
  void		DBF_DeleteRecord (DBF_FILE * fp);
  void		DBF_Pack (DBF_FILE * fp);
  void		DBF_Close (DBF_FILE * fp);
/*-------------------------------------------------------------------------*/

#endif

