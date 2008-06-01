/* cendian.h
**
**	A library to convert host short and long integers to
** little or big endian format. This assumes 16-bit short and 32-bit long
** The machine is assumed to be little-endian (Intel) unless 
** __BYTE_ORDER == __BIG_ENDIAN #define is defined before this file is included.
*/

#ifndef __CENDIAN_H
#define __CENDIAN_H

inline short ShortToLittleEndian (short Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0x00FF) << 8) & (Number >> 8));
#else
    return (Number);
#endif
}

inline short ShortToBigEndian (short Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0x00FF) << 8) & (Number >> 8));
#else
    return (Number);
#endif
}

inline short LittleEndianToShort (short Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0x00FF) << 8) & (Number >> 8));
#else
    return (Number);
#endif
}

inline short BigEndianToShort (short Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0x00FF) << 8) & (Number >> 8));
#else
    return (Number);
#endif
}

inline long LongToLittleEndian (long Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0xFF000000) >> 24) | ((Number & 0x00FF0000) >> 8) |
    	    ((Number & 0x0000FF00) << 8)  | ((Number & 0x000000FF) << 24));
#else
    return (Number);
#endif
}

inline long LongToBigEndian (long Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0xFF000000) >> 24) | ((Number & 0x00FF0000) >> 8) |
    	    ((Number & 0x0000FF00) << 8)  | ((Number & 0x000000FF) << 24));
#else
    return (Number);
#endif
}

inline long LittleEndianToLong (long Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0xFF000000) >> 24) | ((Number & 0x00FF0000) >> 8) |
    	    ((Number & 0x0000FF00) << 8)  | ((Number & 0x000000FF) << 24));
#else
    return (Number);
#endif
}

inline long BigEndianToLong (long Number)
{
#if __BYTE_ORDER == __BIG_ENDIAN
    return (((Number & 0xFF000000) >> 24) | ((Number & 0x00FF0000) >> 8) |
    	    ((Number & 0x0000FF00) << 8)  | ((Number & 0x000000FF) << 24));
#else
    return (Number);
#endif
}

#endif

