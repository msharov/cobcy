/* semextern.h
**
**	External structures common to all modules.
*/

#ifndef __SEMEXTERN_H
#define __SEMEXTERN_H

#include "adtlib/mdefs.h"
#include "adtlib/stack.h"
#include "adtlib/queue.h"
#include "adtlib/hash.h"
#include "semtypes.h"
#include "semutil.h"

#define MAX_FILENAME	30

/***--| Cobcy global configuration options |--***/
typedef struct {
    /* Filename locations */
    char 	SourceFile [MAX_FILENAME];	/* Input COBOL file */
    char 	CodeFile [MAX_FILENAME];	/* Output .c file */
    char 	DeclFile [MAX_FILENAME];	/* Output .h file */

    /* Build options */
    bool	GenDebug;	/* Generate debugging code? */
} CobcyConfigType;

// All of these are defined in seminit.cc
extern Stack<StackEntry> SemStack;
extern HashTable<CobolSymbol> SymTable;
extern int NestingLevel;
extern ofstream codef, declf;
extern CobcyConfigType CobcyConfig;

#endif

