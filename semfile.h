/* semfile.h
**
**	Defines file semantic actions for COBOL compiler.
*/

#ifndef __SEMFILE_H
#define __SEMFILE_H

#define PRINTER_COMMAND		"lpr -h"
#define PRINTER_SPOOL_FILE	"/tmp/cobol.prn"

#define PRINTER_STREAM_NAME	"printer"
#define DISPLAY_STREAM_NAME	"stdout"

/*-------------------------------------------*/
  void 	FileDecl (void);
  void 	GenFileDesc (void);
  void 	AssociateFileRecord (void);
  void 	GenOpen (char * mode);
  void 	GenClose (void);
  void 	GenRead (void);
  void 	GenWrite (void);
  void 	AssociateRecordsWithFD (void);
  void	SetFileStatus (void);
  void	SetAccessMode (char * mode);
  void	SetOrganization (char * org);
  void	SetRelativeKey (void);
  void	SetRecordKey (void);
  void	OpenSpecialFiles (void);
  void	CloseSpecialFiles (void);
  void	FlushPrinterOutput (void);
/*-------------------------------------------*/

#endif

