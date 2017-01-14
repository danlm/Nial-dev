#ifndef _NIALTYPES_H
#define _NIALTYPES_H

#include <inttypes.h>

/**
 * This file contains the base set of configuration 
 * parameters for a nial system.
 */

/*
 * Data alignment boundary for this system. Chosen as 8 for doubles
 */
#define NIAL_ALIGN_BYTES 8
#define NIAL_ALIGNED_VAL(x) (NIAL_ALIGN_BYTES*(((x)+NIAL_ALIGN_BYTES-1)/NIAL_ALIGN_BYTES))


/* the range for characters assuming an ascii character set */
#define LOWCHAR 0
#define HIGHCHAR 255

/* The editor to be invoked in defedit */
#define DEFEDITOR "vi"

#define collatingseq "0123456789 _&aAbBcCdDeEfFgGhHiIjJkKlLmMnNoOpPqQrRsStTuUvVwWxXyYzZ-~.,;:()[]{}<>+?!#$%'=^|\\\"/@`*"

/**
 * base typedefs to configure core activities
 * Nial is parametrized along these types  
 */

#ifdef INTS32

typedef int nialword;                  /* memory organised as array of nial words */
typedef int nialint;                  /* data holding a signed integer */
typedef unsigned int unialint;        /* data holding an unsigned nial int */
typedef int nialptr;                  /* index of an entry in the workspace */
typedef short nialvalence;            /* storage for a valence value */

#elif defined(INTS64)

#ifdef UNIXSYS
typedef long nialword;              /* memory organised as array of nial words */
typedef long nialint;               /* data holding a signed integer */
typedef unsigned long unialint;     /* data holding an unsigned nial int */
typedef long nialptr;               /* index of an entry in the workspace */
typedef int nialvalence;            /* storage for a valence value */
#endif

#ifdef WINNIAL
typedef long long nialword;         /* memory organised as array of nial words */
typedef long long nialint;          /* data holding a signed integer */
typedef unsigned long long unialint; /* data holding an unsigned nial int */
typedef long long nialptr;          /* index of an entry in the workspace */
typedef int nialvalence;            /* storage for a valence value */
#endif


#else 
#error missing declaration of integer size
#endif


/**
 * Header structure for a workspace block to contain nial objects
 */
typedef struct {
  nialint size;                /* size of this block */
  nialint ref_count;           /* for heap memory management */

  /* Header varies if block is free or allocated */
  union {

    /* header information for an allocated block */
    struct {                    
      union {
	struct _skv {
	  char        sk[2];    /* sort flag and kind */
	  short       val;      /* valence */
	} skv;
	nialint block_flags;          /* sort, kind, valence */
      } flags;
      nialint block_tally;               /* data count */
    } allocatedblock;
    
    /* header information for a free block */
    struct {                    
      nialptr fwd_link;              /* next free block */
      nialptr bck_link;              /* previous free block */
    } free_block;

  } hdrdata;

} nialhdr;



/**
 * Simple structure at the end of workspace blocks 
 */ 
typedef union {
  nialint allocatedflag;            /* flag to test if block is allocated */
  nialptr back_ptr;                 /* negative back link to start of block if free, 0 otherwise */
} nialtrailer;


/**
 * Define the minimum data part for a nial object
 */
typedef union {
  double double_entry;
  nialint int_entry;
} nial_mindata;


/**
 * Compute the number of nial words to hold nbytes of data and to be
 * an integral multiple of doubles for memory alignment
 *
 * NB Neet to change this to shift model for speed
 */
#define ALIGNED_WORD_COUNT(_n) ((NIAL_ALIGN_BYTES*((_n*sizeof(nialword) + NIAL_ALIGN_BYTES - 1)/NIAL_ALIGN_BYTES))/sizeof(nialword))


/*
extern void trace_point(const char *t);
extern void trace_intv(const char *t, int v);
*/

#endif

