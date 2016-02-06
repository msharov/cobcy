// This file is part of cobcy, a COBOL-to-C compiler.
//
// Copyright (C) 1995-2008 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once

void GenParagraph (void);
void GenGoto (void);
void GenPerform (void);
void GenEndProc (void);
void BeginCompound (void);
void EndCompound (void);
void GenStartProc (void);
void GenStartIf (void);
void GenStartElsif (void);
void GenEndIf (void);
void GenElse (void);
void GenBool (void);
void GenConnect (void);
void GenStopRun (void);
void GenParagraphCalls (void);
void GenStartLoop (void);
void GenLoopInit (void);
void GenLoopCondition (void);
void GenLoopIncrement (void);
void GenEmptyClause (void);
void GenCall (void);
