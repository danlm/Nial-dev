/*==============================================================

  MODULE   NFILES.C

  COPYRIGHT NIAL Systems Limited  1983-2016

  This contains the system independent directory operations
   and support routines

================================================================*/


/* Q'Nial file that selects features and loads the xxxspecs.h file */

#include "switches.h"

/* standard library header files */

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

/* VARARGSLIB */
#include <stdarg.h>
#include <stddef.h>


/* Q'Nial header files */

#include "qniallim.h"
#include "absmach.h"
#include "basics.h"
#include "lib_main.h"
#include "if.h"
#include "ops.h"

#include "utils.h"           /* for ngetname */

#ifdef NFILES

/**
 * Create a new directory
 */
void imkdir(void) {
  nialptr x = apop();

  if (istext(x)) {
#ifdef UNIXSYS
    int r = mkdir(pfirstchar(x), 0777);
#endif
#ifdef WINNIAL
    int r = mkdir(pfirstchar(x));
#endif
    apush((r==-1)? False_val: True_val);
  } else {
    apush(makefault("?args"));
  }
  
  freeup(x);
  return;
}


/**
 * Remove a directory
 */
void irmdir(void) {
  nialptr x = apop();
  
  if (istext(x)) {
    int res = rmdir(pfirstchar(x));
    apush((res == -1)? False_val: True_val);
  } else {
    apush(makefault("?args"));
  }
  
  freeup(x);
  return;
}


/**
 * Rename a file or directory
 */
void irename(void) {
  nialptr x = apop();
  nialptr to_ptr, from_ptr;
  
  if (kind(x) == atype || tally(x) == 2) {
    splitfb(x, &from_ptr, &to_ptr);
    if (istext(from_ptr) &&  istext(to_ptr)) {
      int res = rename(pfirstchar(from_ptr), pfirstchar(to_ptr));
      apush((res == -1)? False_val: True_val);
      freeup(x);
      return;
    }
  }
  
  apush(makefault("?args"));
  freeup(x);
  return;
}

#endif  /* NFILES */

