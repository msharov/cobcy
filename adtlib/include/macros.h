/* macros.h
**
**	Defines some widely used macros.
**
*/

#ifndef __MACROS_H
#define __MACROS_H

#ifndef	min
#   define	min(a,b)	((a) < (b) ? (a) : (b))
#endif
#ifndef	max
#   define	max(a,b)	((a) > (b) ? (a) : (b))
#endif							 
#ifndef diff
#   define	diff(a,b)	((a) > (b) ? ((a) - (b)) : ((b) - (a)))
#endif
#ifndef randnum
#   define	randnum(range)	((range) != 0 ? random() % (range) : 0)
#endif
#ifndef sign
#   define	sign(x)		((x) == 0 ? 0 : ((x) < 0 ? (-1) : (1)))
#endif

#endif

