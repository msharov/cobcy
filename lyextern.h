/* lyextern.h
**
**	For C++ modules to have yyparse and yylex.
*/

#ifndef __LYEXTERN_H
#define __LYEXTERN_H

#ifdef _cplusplus
extern "C" {
#endif

int yylex (void);

#ifdef _cplusplus
}
#endif

#endif

