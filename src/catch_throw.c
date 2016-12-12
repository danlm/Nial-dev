/*==============================================================

  MODULE   CATCH_THROW.C

  COPYRIGHT NIAL Systems Limited  1983-2016

  Catch Throw module for Nial

================================================================*/

/* Q'Nial file that selects features and loads the xxxspecs.h file */

#include "switches.h"

#ifdef CATCHTHROW

/* standard library header files */

/* IOLIB */
#include <stdio.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/fcntl.h>

/* LIMITLIB */
#include <limits.h>

/* STDLIB */
#include <stdlib.h>

/* STLIB */
#include <string.h>

/* SIGLIB */
#include <signal.h>

/* SJLIB */
#include <setjmp.h>

/* PROCESSLIB */
#include <sys/types.h>
#include <pwd.h>
#include <fcntl.h>
#include <netdb.h>
#include <errno.h>

#include "absmach.h"
#include "qniallim.h"
#include "lib_main.h"
#include "ops.h"
#include "trs.h"
#include "fileio.h"

#include <setjmp.h>

extern void pair(nialptr x, nialptr y);
extern void iapply(void);
extern void nct_do_eval(nialptr exp);
extern void apply(nialptr fn);

extern int get_current_call_stack();
extern void reset_current_call_stack(int old_stack);


/** This holds the currently active jump buffer */
jmp_buf *ct_current_jmp_buf = NULL;

void icatch(void) {
  nialptr f, x;
  jmp_buf catch_buf, *current_catch_buf;
  nialint arg_stack;
  int call_stack;
  int rc;

  /* Get the function argument */
  f = apop();

  /* Get the data argument */
  x = apop();

  /* Add another longjmp sequence */
  current_catch_buf = ct_current_jmp_buf;
  ct_current_jmp_buf = &catch_buf;

  /* Save the stack poistions for rewind */
  arg_stack = topstack;
  call_stack = get_current_call_stack();

  /* Set up the jump_buf */
  rc = setjmp(catch_buf);
  if (rc) {
    /* Entered via a long jump */
    nialptr res = apop();        /* get result off stack */

    ct_current_jmp_buf = current_catch_buf;      /* unwind jmpbuf */
    reset_current_call_stack(call_stack);   /* unwind call stack */

    while (arg_stack != topstack) {         /* unwind arg stack */
      freeup(apop());
    }

    apush(res);
    return;
  }

  /* Put x back on the stack and call apply */
  apush(x);
  apply(f);

  /* if we return here there were no throws */
  ct_current_jmp_buf = current_catch_buf;      /* unwind jmpbuf */
  return;
}


/**
 * Function to throw a result value.
 * This should only be called if we are in a catch/throw
 * sequence.
 *
 * The rc_code argument is for the future but at 
 * moment should always be 1.
 *
 * Used by eval.c as wel as below.
 */

void nct_throw_result(nialptr res, int rc_code) {
  apush(res);
  longjmp(*ct_current_jmp_buf, rc_code);
  return;
}

  
/**
 * primitive to throw a result value. If we are not in a
 * catch/throw sequence it returns a fault.
 */
void ithrow(void) {
  if (ct_current_jmp_buf == NULL) {
    nialptr x = apop();
    apush(makefault("?no_catch"));
    freeup(x);
  } else {
    nct_throw_result(apop(), 1);
  }
  return;
}

#ifdef OMITTED
/**
 * Function to throw a fault to a catch.
 * This should only be called if we are in a catch/throw
 * sequence.
 *
 * The rc_code argument is for the future but at 
 * moment should always be 1.
 */
void nct_throw_fault(nialptr nfault, int rc_code) {
  apush(nfault);
  longjmp(*ct_current_jmp_buf, rc_code);
  return;
}
#endif /* OMITTED */


/**
 * Return true if we are in a catch/throw sequence
   This is use by eval.c
 */
int nct_in_catch_throw() {
  return (ct_current_jmp_buf != NULL);
}

#endif /* CATCHTHROW */
