/* semcontrol.h
**
**	Control statement semantics.
*/

#ifndef __SEMCONTROL_H
#define __SEMCONTROL_H

/*-----------------------------------------------------------------*/
  void 	GenParagraph (void);
  void 	GenGoto (void);
  void	GenPerform (void);
  void 	GenEndProc (void);
  void 	BeginCompound (void);
  void 	EndCompound (void);
  void 	GenStartProc (void);
  void 	GenStartIf (void);
  void 	GenStartElsif (void);
  void 	GenEndIf (void);
  void 	GenElse (void);
  void 	GenBool (void);
  void 	GenConnect (void);
  void 	GenStopRun (void);
  void	GenParagraphCalls (void);
  void	GenStartLoop (void);
  void	GenLoopInit (void);
  void	GenLoopCondition (void);
  void	GenLoopIncrement (void);
  void	GenEmptyClause (void);
/*-----------------------------------------------------------------*/

#endif

