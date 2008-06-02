// dbf.h
//
//	Defines ANSI C structures to deal with dBASE IV files.
//

#ifndef DBF_H_3DA1E0F75FD740083F92610B470AE378
#define DBF_H_3DA1E0F75FD740083F92610B470AE378

#include <stdint.h>
#include <stdio.h>

#define HEADER_END_CHAR		'\x0D'
#define DBF_FIELD_NAME_LENGTH	11

typedef struct {
    uint8_t		Version;
    struct {
	uint8_t	Year;
        uint8_t	Month;
	uint8_t	Day;
    }			Date;
    uint32_t		nRecords;
    uint16_t		HeaderLength;
    uint16_t		RecordLength;
} DBF_Header;

typedef struct {
    char		Name [DBF_FIELD_NAME_LENGTH];
    uint8_t		Type;
    uint32_t		Address;
    uint8_t		FieldLength;
    uint8_t		DecimalPlaces;
} DBF_Field;

typedef struct {
    char		Filename [256];
    FILE*		DataDesc;
    DBF_Field*		Fields;
    DBF_Header		Header;
    uint16_t		nFields;
    char		OpenMode [5];
} DBF_FILE;

//----------------------------------------------------------------------

DBF_FILE*	DBF_Open (const char* filename, const char* mode);
DBF_FILE*	DBF_Create (const char* filename, uint16_t nFields, DBF_Field* def);
void		DBF_SeekToRecord (DBF_FILE* fp, uint32_t record);
void		DBF_SeekToNext (DBF_FILE* fp);
void		DBF_SeekToFirst (DBF_FILE* fp);
void		DBF_SeekToLast (DBF_FILE* fp);
void		DBF_AppendRecord (DBF_FILE* fp);
void		DBF_RewriteRecord (DBF_FILE* fp, const void* data);
void		DBF_ReadRecord (DBF_FILE* fp, void* data);
void		DBF_DeleteRecord (DBF_FILE* fp);
void		DBF_Pack (DBF_FILE* fp);
void		DBF_Close (DBF_FILE* fp);

//----------------------------------------------------------------------

#endif

