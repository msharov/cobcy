/* semextern.h
**
**	External structures common to all modules.
*/

#ifndef __SEMEXTERN_H
#define __SEMEXTERN_H

#include <iostream.h>
#include <stack.h>
#include <queue.h>
#include <hash.h>
#include <string.h>
#include "symbol.h"
#include "semsedef.h"
#include "semutil.h"

extern Stack<StackEntry> SemStack;
extern HashTable<CobolSymbol> SymTable;
extern int NestingLevel;
extern ofstream outfile;
extern char OutputFile[];
extern char SourceFile[];

#endif

