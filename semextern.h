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

// All of these are defined in seminit.cc
extern Stack<StackEntry> SemStack;
extern HashTable<CobolSymbol> SymTable;
extern int NestingLevel;
extern ofstream codef, declf;

#endif

