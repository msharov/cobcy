/* cobfunc.h
**
**	Defines cobol function library.
*/

#ifndef __COBFUNC_H
#define __COBFUNC_H

#include "dbf/dbf.h"
#include "dbf/ndx.h"

/*----------------------------------------------------------------------------*/

typedef enum {
    _dt_Null,
    _dt_String,
    _dt_Float,
    _dt_Integer
} _EDataType;

typedef struct {
    int		column;
    int		line;
    int		foreground;
    int		background;
    _EDataType	dataType;
    union {
	char*		cval;
	double*		fval;
	long int*	ival;
    }		data;
    const char*	picture;
    int		blankScreen;
} _SScreenField;

/*----------------------------------------------------------------------------*/
  int 		_Alphabetic (const char * str);
  int 		_AlphabeticCase (const char * str, int what);
  void 		_RuntimeError (const char * message);
  void 		_ReadStringVar (FILE * stream, char * var, const char * pic);
  void 		_ReadIntegerVar (FILE * stream, long int * var, const char * pic);
  void 		_ReadFloatVar (FILE * stream, double * var, const char * pic);
  void 		_WriteStringVar (FILE * stream, const char * var, const char * pic);
  void 		_WriteIntegerVar (FILE * stream, long int var, const char * pic);
  void 		_WriteFloatVar (FILE * stream, double var, const char * pic);
  char * 	_IntegerToString (long int var, const char * pic);
  char * 	_FloatToString (double var, const char * pic);
  long int	_StringToInteger (const char * string);
  double	_StringToFloat (const char * string);
  void 		_AssignVarString (char * var1, const char * var2, 
  				  int p1, int p2);
  void		_OpenSequentialFile (FILE ** fp, const char * filename, const char * mode);
  void		_OpenRelativeFile (DBF_FILE ** fp, const char * filename, 
  				   const char * sig, const char * mode);
  void		_OpenIndexedFile (DBF_FILE ** fpd, const char * filename, 
  				  const char * sig, const char * mode);
  void		_CloseSequentialFile (FILE * fp);
  void		_CloseRelativeFile (DBF_FILE ** fp);
  void		_CloseIndexedFile (DBF_FILE ** fpd);
  void		_OpenIndexFile (NDX_FILE ** ifd, const char * filename, 
  				  const char * sig, const char * mode);
  void		_CloseIndexFile (NDX_FILE ** ifd);
  void		_SeekRelativeFile (DBF_FILE * fp, long int record);
  void		_SeekIndexedFile (DBF_FILE * fpd, NDX_FILE * fpi, const char * key);
  double 	_RoundResult (double num, const char * pic);
  void		_EnableCurses (void);
  void 		_DisplayScreen (const _SScreenField* pScreen, int screenSize);
  void 		_AcceptScreen (_SScreenField* pScreen, int screenSize);
/*----------------------------------------------------------------------------*/

#endif

