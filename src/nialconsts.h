#ifndef _NIAL_CONSTS_H
#define _NIAL_CONSTS_H

#include <inttypes.h>

/* INTS32 vs INTS64 build variations */

#ifdef INTS32

#define NIALONEBIT (1)
#define ALLBITSON (-1)
#define LARGEINT 2147483647
#define SMALLINT (-2147483647 - 1)
#define LARGEINT_Boundary 2147483647.0
#define SMALLINT_Boundary -2147483648.0

/* Size of nial word in bytes */
#define bytespu (4)

/* Aligned header block size in words */
#define hdrsize (4)

/* Aligned trailer block size in words */
#define trlsize (2)

/* Min aligned block size in words (includes space for double) */
#define minsize (8)

/* Min data size in words */
#define mindatasize (2)

/* Bits per nial memory word - 1 for shifts */
#define BoolPackBase (31)

/* bitsize for nial representations */

#define sizeofnialint sizeof(int)
#define sizeofnialreal sizeof(double)
#define sizeofnialchar sizeof(char)

/* How types are fitted into words */
#define WParray (1)
#define WPint (1)
#define WPchar (1)
#define WPbool (1)
#define WPreal (2)

/* How types are packed into words */
#define boolsPW (32)
#define charsPW (4)

#define NIALINT_FORMAT "%d"

#elif defined(INTS64)

#ifdef UNIXSYS
#define NIALONEBIT (1L)
#define ALLBITSON (-1)
#define LARGEINT 9223372036854775807
#define SMALLINT (-9223372036854775807 - 1)
#define LARGEINT_Boundary 9007199254740992.0
#define SMALLINT_Boundary -9007199254740992.0
#endif

#ifdef WINDOWS
#define NIALONEBIT (1LL)
#define ALLBITSON (-1LL)
#define LARGEINT 9223372036854775807LL
#define SMALLINT (-9223372036854775807LL - 1)
#define LARGEINT_Boundary 9007199254740992.0
#define SMALLINT_Boundary -9007199254740992.0
#endif

/* Size of nial word in bytes */
#define bytespu (8)

/* Aligned header block size in words */
#define hdrsize (4)

/* Aligned trailer block size in words */
#define trlsize (2)

/* Min aligned block size in words (includes space for double) */
#define minsize (8)

/* Min data size in words */
#define mindatasize (3)

/* Bits per nial memory word - 1 for shifts */
#define BoolPackBase (63)

/* bitsize for nial representations */

#ifdef UNIXSYS
#define sizeofnialint sizeof(long)
#endif
#ifdef WINDOWS
#define sizeofnialint sizeof(long long)
#endif

#define sizeofnialreal sizeof(double)
#define sizeofnialchar sizeof(char)

/* How types are fitted into words */
#define WParray (1)
#define WPint (1)
#define WPchar (1)
#define WPbool (1)
#define WPreal (1)

/* How types are packed into words */
#define boolsPW (64)
#define charsPW (8)

#ifdef UNIXSYS
#define NIALINT_FORMAT "%ld"
#endif
#ifdef WINDOWS
#define NIALINT_FORMAT "%lld"
#endif

#else
#error missing declaration of integer size
#endif

#endif /* _NIAL_CONSTS_H */
