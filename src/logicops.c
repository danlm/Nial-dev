/*==============================================================

  MODULE   LOGICOPS.C

  COPYRIGHT NIAL Systems Limited  1983-2016

  This contains logic operation primitives.

================================================================*/



/* Q'Nial file that selects features */

#include "switches.h"

/* IOLIB */
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#ifdef UNIXSYS
#include <sys/mman.h>
#endif
#include <sys/fcntl.h>

/* STDLIB */
#include <stdlib.h>

/* STLIB */
#include <string.h>

/* SJLIB */
#include <setjmp.h>

/* CLIB */
#include <ctype.h>


/* Q'Nial header files */

#include "logicops.h"
#include "qniallim.h"
#include "lib_main.h"
#include "absmach.h"
#include "basics.h"

#include "utils.h"           /* for converters */
#include "trs.h"             /* for int_each etc */
#include "ops.h"             /* for splitfb and simple */
#include "faults.h"          /* for Logical fault */


/* declaration of internal static routines */

static void orboolvectors(nialptr x, nialptr y, nialptr z, nialint n);
static void xorboolvectors(nialptr x, nialptr y, nialptr z, nialint n);
static void andboolvectors(nialptr x, nialptr y, nialptr z, nialint n);
static void notbools(nialptr x, nialptr z, nialint n);
static void initbool(nialptr x, int v);


/*
 * Bitmasks to simplify boolean operations on vectors
 */

#ifdef INTS32
static nialint nialint_masks[33] = {
    0,
    0x1,
    0x3,
    0x7,
    0xf,
    0x1f,
    0x3f,
    0x7f,
    0xff,
    0x1ff,
    0x3ff,
    0x7ff,
    0xfff,
    0x1fff,
    0x3fff,
    0x7fff,
    0xffff,
    0x1ffff,
    0x3ffff,
    0x7ffff,
    0xfffff,
    0x1fffff,
    0x3fffff,
    0x7fffff,
    0xffffff,
    0x1ffffff,
    0x3ffffff,
    0x7ffffff,
    0xfffffff,
    0x1fffffff,
    0x3fffffff,
    0x7fffffff,
    0xffffffff
};
#endif
#ifdef INTS64
static nialint nialint_masks[65] = {
    0,
    0x1,
    0x3,
    0x7,
    0xf,
    0x1f,
    0x3f,
    0x7f,
    0xff,
    0x1ff,
    0x3ff,
    0x7ff,
    0xfff,
    0x1fff,
    0x3fff,
    0x7fff,
    0xffff,
    0x1ffff,
    0x3ffff,
    0x7ffff,
    0xfffff,
    0x1fffff,
    0x3fffff,
    0x7fffff,
    0xffffff,
    0x1ffffff,
    0x3ffffff,
    0x7ffffff,
    0xfffffff,
    0x1fffffff,
    0x3fffffff,
    0x7fffffff,
    0xffffffff,
    0x1ffffffff,
    0x3ffffffff,
    0x7ffffffff,
    0xfffffffff,
    0x1fffffffff,
    0x3fffffffff,
    0x7fffffffff,
    0xffffffffff,
    0x1ffffffffff,
    0x3ffffffffff,
    0x7ffffffffff,
    0xfffffffffff,
    0x1fffffffffff,
    0x3fffffffffff,
    0x7fffffffffff,
    0xffffffffffff,
    0x1ffffffffffff,
    0x3ffffffffffff,
    0x7ffffffffffff,
    0xfffffffffffff,
    0x1fffffffffffff,
    0x3fffffffffffff,
    0x7fffffffffffff,
    0xffffffffffffff,
    0x1ffffffffffffff,
    0x3ffffffffffffff,
    0x7ffffffffffffff,
    0xfffffffffffffff,
    0x1fffffffffffffff,
    0x3fffffffffffffff,
    0x7fffffffffffffff,
    0xffffffffffffffff
};
#endif


/* routine to implement the binary pervading operation or.
   This is called by ior when adding pairs of arrays.
   It uses supplementary routines to permit vector processing
   if such support routines are available.
*/

void
b_or()
{
  nialptr     y = apop(),
              x = apop(),
              z;
  int         kx = kind(x),
              ky = kind(y);

  if (kx == booltype && ky == booltype
      && (atomic(x) || atomic(y) || equalshape(x, y))) {
    if (atomic(x)) {
      if (atomic(y))
        z = createbool(boolval(x) || boolval(y));
      else if (boolval(x)) {
        int         v = valence(y);

        z = new_create_array(booltype, v, 0, shpptr(y, v));
        initbool(z, 1);
      }
      else
        z = y;
    }
    else if (atomic(y)) {
      if (boolval(y)) {
        int         v = valence(x);

        z = new_create_array(booltype, v, 0, shpptr(x, v));
        initbool(z, 1);
      }
      else
        z = x;
    }
    else {
      int         v = valence(x);
      nialint     tx = tally(x);

      z = new_create_array(booltype, v, 0, shpptr(x, v));
      orboolvectors(x, y, z, tx);
    }
  }
  else
   /* handle remaining cases */ 
  if (atomic(x) && atomic(y)) 
  { if (kx == faulttype)
    { if (ky == faulttype)
      { if (x == y)
          z = x;
        else
          z = Logical;
      }
      else
#ifdef V4AT
        z = Logical;
#else
        z = x;
#endif
    }
    else 
    if (ky == faulttype)
#ifdef V4AT
      z = Logical;
#else
      z = y;
#endif
    else                     /* other types cause a fault */
      z = Logical;
  }
  else {
    int_eachboth(b_or, x, y);
    return;
  }
  apush(z);
  freeup(x);
  freeup(y);
}

/* unary version of or */

void
ior()
{
  nialptr     x;
  nialint     tx;
  int         kx;

  x = apop();
  kx = kind(x);
  tx = tally(x);
  switch (kx) {
    case booltype:
        apush(createbool(orbools(x, tx)));
        break;
    case inttype:
    case chartype:
    case realtype:
    case phrasetype:
        apush(Logical);
        break;
    case faulttype:
        apush(x);
        break;
    case atype:
        if (tx == 0)
#ifdef V4AT
        { nialptr archetype = fetch_array(x,0);
          if (atomic(archetype))
          { if (kind(archetype)==booltype)
              { apush(False_val); }
            else
              apush(Logical);
          }
          else
          { apush(x);
            ipack();
            int_each(ior,apop());
            return;
          }
        }
#else
        {
          apush(False_val);
        }
#endif
        else if (simple(x)) {/* has non-numeric items */
          apush(testfaults(x, Logical));
        }
        else if (tx == 2) {
          apush(fetch_array(x, 0));
          apush(fetch_array(x, 1));
          b_or();
        }
        else {
          apush(x);
          ipack();
          int_each(ior, apop());
          return;
        }
        break;
  }
  freeup(x);
}

/* support routines for or 
  Does it word by word except for partial word at the end 
*/

int
orbools(nialptr x, nialint n)
{
  nialint     i = 0,
             *ptrx = pfirstint(x),  /* safe: no allocations */
              s = false,
              wds = n / boolsPW,
              exc = n % boolsPW;

  /* handle leading words */
  while (!s && i++ < wds)
    s = ((*ptrx++) != 0);        /* at least one bit is on */
    
  /* handle trailing bits */
  if (s == false && exc != 0) {
    nialint mask = ~nialint_masks[boolsPW-exc];
    s = ((*ptrx & mask)) != 0;
  }

  return (s);
}

static void
orboolvectors(nialptr x, nialptr y, nialptr z, nialint n)
{
  nialint     i,
             *ptrx = pfirstint(x),  /* safe: no allocations */
             *ptry = pfirstint(y),  /* safe: no allocations */
             *ptrz = pfirstint(z),  /* safe: no allocations */
              wds = n / boolsPW,    /* initial segment of full words */
              exc = n % boolsPW;    /* excess bits over full words */

  for (i = 0; i < wds; i++)
    *ptrz++ = *ptrx++ | *ptry++;
  if (exc != 0)
    *ptrz++ = (*ptrx++ | *ptry++) & ~nialint_masks[boolsPW-exc];
}

/* routine to implement the binary pervading operation xor.
   This is called by ixor when adding pairs of arrays.
   It uses supplementary routines to permit vector processing
   if such support routines are available.
*/

void
b_xor()
{
  nialptr     y = apop(),
              x = apop(),
              z;
  int         kx = kind(x),
              ky = kind(y);

  if (kx == booltype && ky == booltype
      && (atomic(x) || atomic(y) || equalshape(x, y))) {
    if (atomic(x)) {
      if (atomic(y))
        z = createbool(boolval(x) ^ boolval(y));
      else if (boolval(x)) {
        int         v = valence(y);

        z = new_create_array(booltype, v, 0, shpptr(y, v));
        initbool(z, 1);
      }
      else
        z = y;
    }
    else if (atomic(y)) {
      if (boolval(y)) {
        int         v = valence(x);

        z = new_create_array(booltype, v, 0, shpptr(x, v));
        initbool(z, 1);
      }
      else
        z = x;
    }
    else {
      int         v = valence(x);
      nialint     tx = tally(x);

      z = new_create_array(booltype, v, 0, shpptr(x, v));
      xorboolvectors(x, y, z, tx);
    }
  }
  else
   /* handle remaining cases */ 
  if (atomic(x) && atomic(y)) 
  { if (kx == faulttype)
    { if (ky == faulttype)
      { if (x == y)
          z = x;
        else
          z = Logical;
      }
      else
#ifdef V4AT
        z = Logical;
#else
        z = x;
#endif
    }
    else 
    if (ky == faulttype)
#ifdef V4AT
      z = Logical;
#else
      z = y;
#endif
    else                     /* other types cause a fault */
      z = Logical;
  }
  else {
    int_eachboth(b_xor, x, y);
    return;
  }
  apush(z);
  freeup(x);
  freeup(y);
}

/* unary version of xor */

void
ixor()
{
  nialptr     x;
  nialint     tx;
  int         kx;

  x = apop();
  kx = kind(x);
  tx = tally(x);
  switch (kx) {
    case booltype:
        apush(createbool(xorbools(x, tx)));
        break;
    case inttype:
    case chartype:
    case realtype:
    case phrasetype:
        apush(Logical);
        break;
    case faulttype:
        apush(x);
        break;
    case atype:
        if (tx == 0)
#ifdef V4AT
        { nialptr archetype = fetch_array(x,0);
          if (atomic(archetype))
          { if (kind(archetype)==booltype)
              { apush(False_val); }
            else
              apush(Logical);
          }
          else
          { apush(x);
            ipack();
            int_each(ixor,apop());
            return;
          }
        }
#else
        {
          apush(False_val);
        }
#endif
        else if (simple(x)) {/* has non-numeric items */
          apush(testfaults(x, Logical));
        }
        else if (tx == 2) {
          apush(fetch_array(x, 0));
          apush(fetch_array(x, 1));
          b_xor();
        }
        else {
          apush(x);
          ipack();
          int_each(ixor, apop());
          return;
        }
        break;
  }
  freeup(x);
}

/* support routines for xor 
  Does it word by word except for partial word at the end 
*/

int
xorbools(nialptr x, nialint n)
{
  nialint     i = 0, j = 0, bpos = 0, cw,
             *ptrx = pfirstint(x),  /* safe: no allocations */
              s = false,
              wds = n/ boolsPW,
              exc = n % boolsPW;
  
  /*
   * exclusive or is associative and commutative.
   * We can xor all the words then xor bits in result.
   */
  for (i = 0; i < wds; i++) 
    s ^= *ptrx++;
    
  /*
   * xor in bits of trailer. The zeros after masking
   * will not affect result as ^0  is identity
   */
  if (exc != 0)
    s ^= ((*ptrx) & ~nialint_masks[boolsPW-exc]);

  /*
  * Now fold the bits of the word with xor.
  * we only care about the last bit so dont need masks
  */
  for (j = boolsPW/2; j > 0; j >>= 1)
    s = s ^ (s >> j);
    
  return (s&1);
}


static void
xorboolvectors(nialptr x, nialptr y, nialptr z, nialint n)
{
  nialint     i,
             *ptrx = pfirstint(x),  /* safe: no allocations */
             *ptry = pfirstint(y),  /* safe: no allocations */
             *ptrz = pfirstint(z),  /* safe: no allocations */
              wds = n / boolsPW,    /* initial segment of full words */
              exc = n % boolsPW;    /* excess bits over full words */

  for (i = 0; i < wds; i++)
    *ptrz++ = *ptrx++ ^ *ptry++;
  if (exc != 0)
    *ptrz++ = (*ptrx++ ^ *ptry++) & ~nialint_masks[boolsPW-exc];
}

static void
initbool(nialptr x, int v)
{
  nialint     i,
             *ptrx = pfirstint(x),  /* safe: no allocations */
              wv = (v ? ALLBITSON : 0),
              n = tally(x),
              wds = n / boolsPW;

  for (i = 0; i < wds; i++)
    *ptrx++ = wv;
  for (i = wds * boolsPW; i < n; i++)
    store_bool(x, i, v);
}

/* routines to implement and. Same algorithm as ior */

void
b_and()
{
  nialptr     y = apop(),
              x = apop(),
              z;
  int         kx = kind(x),
              ky = kind(y);

  if (kx == booltype && ky == booltype
      && (atomic(x) || atomic(y) || equalshape(x, y))) {
    if (atomic(x)) {
      if (atomic(y))
        z = createbool(boolval(x) && boolval(y));
      else if (boolval(x))
        z = y;
      else {
        int         v = valence(y);

        z = new_create_array(booltype, v, 0, shpptr(y, v));
        initbool(z, 0);
      }
    }
    else if (atomic(y)) {
      if (boolval(y))
        z = x;
      else {
        int         v = valence(x);

        z = new_create_array(booltype, v, 0, shpptr(x, v));
        initbool(z, 0);
      }
    }
    else {
      int         v = valence(x);
      nialint     tx = tally(x);

      z = new_create_array(booltype, v, 0, shpptr(x, v));
      andboolvectors(x, y, z, tx);
    }
  }
  else
   /* handle remaining cases */ 
  if (atomic(x) && atomic(y)) 
  { if (kx == faulttype)
    { if (ky == faulttype)
      { if (x == y)
           z = x;
         else
           z = Logical;
      }
      else
#ifdef V4AT
        z = Logical;
#else
        z = x;
#endif
    }
    else 
    if (ky == faulttype)
#ifdef V4AT
      z = Logical;
#else
      z = y;
#endif
    else                     /* other types cause a fault */
      z = Logical;
  }
  else {
    int_eachboth(b_and, x, y);
    return;
  }
  apush(z);
  freeup(x);
  freeup(y);
}

void
iand()
{
  nialptr     x;
  nialint     tx;
  int         kx;

  x = apop();
  kx = kind(x);
  tx = tally(x);
  switch (kx) {
    case booltype:
        apush(createbool(andbools(x, tx)));
        break;
    case inttype:
    case chartype:
    case realtype:
    case phrasetype:
        apush(Logical);
        break;
    case faulttype:
        apush(x);
        break;
    case atype:
        if (tx == 0) 
#ifdef V4AT
        { nialptr archetype = fetch_array(x,0);
          if (atomic(archetype))
          { if (kind(archetype)==booltype)
              { apush(True_val); }
            else
              apush(Logical);
          }
          else
          { apush(x);
            ipack();
            int_each(iand,apop());
            return;
          }
        }
#else
        {
          apush(True_val);
        }
#endif
        else if (simple(x)) {/* has non-numeric items */
          apush(testfaults(x, Logical));
        }
        else if (tx == 2) {
          apush(fetch_array(x, 0));
          apush(fetch_array(x, 1));
          b_and();
        }
        else {
          apush(x);
          ipack();
          int_each(iand, apop());
          return;
        }
        break;
  }
  freeup(x);
}

/* support routines for and. 
   Does it word by word except for partial word at the end 
*/

int
andbools(nialptr x, nialint n)
{
  nialint     i = 0, cw,
             *ptrx = pfirstint(x),  /* safe: no allocations */
              s = true,
              wds = n / boolsPW,
              exc = n % boolsPW;

  /* Handle leading words */
  while (s && i++ < wds)
    s = (*ptrx++ == ALLBITSON);/* all bits are on */
  
  /* handle trailing bits */ 
  if (s && exc != 0) {
    nialint mask = ~nialint_masks[boolsPW-exc];
    s = (*ptrx & mask) == mask;
  }

   return (s);
}

static void
andboolvectors(nialptr x, nialptr y, nialptr z, nialint n)
{
  nialint     i,
             *ptrx = pfirstint(x),  /* safe: no allocations */
             *ptry = pfirstint(y),  /* safe: no allocations */
             *ptrz = pfirstint(z),  /* safe: no allocations */
              wds = n / boolsPW,
              exc = n % boolsPW;
              
  for (i = 0; i < wds; i++)
    *ptrz++ = *ptrx++ & *ptry++;
  if (exc != 0)
    *ptrz++ = (*ptrx++ & *ptry++) & ~nialint_masks[boolsPW-exc];
}

/* routine to implement not. Same algorithm as abs in arith.c */

void
inot()
{
  nialptr     z,
              x = apop();
  int         k = kind(x),
              v = valence(x);
  nialint     t = tally(x);

  switch (k) {
    case booltype:
        z = new_create_array(booltype, v, 0, shpptr(x, v));
        notbools(x, z, t);
        apush(z);
        freeup(x);
        break;
    case inttype:
    case realtype:
    case chartype:
        if (atomic(x)) {
          apush(Logical);
          freeup(x);
        }
        else
          int_each(inot, x);
        break;
    case phrasetype:
        apush(Logical);
        break;
    case faulttype:
        apush(x);
        break;
    case atype:
        int_each(inot, x);
        break;
  }
}

/* support routines for not 
   Does it word by word except for partial word at end.
*/

static void
notbools(nialptr x, nialptr z, nialint n)
{
  nialint     i,
              wds = n / boolsPW,
              exc = n % boolsPW,
             *ptrx = pfirstint(x),  /* safe: no allocations */
             *ptrz = pfirstint(z);  /* safe: no allocations */

  for (i = 0; i < wds; i++)
    *ptrz++ = ~(*ptrx++);
  if (exc != 0)
    *ptrz++ = (~(*ptrx++)) & ~nialint_masks[boolsPW-exc];
}
