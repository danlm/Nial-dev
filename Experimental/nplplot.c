#include "switches.h"

#ifdef N_PLPLOT

#include <string.h>
#include "plplot/plplot.h"


#define MATHLIB
#define STDLIB
#define IOLIB
#define LIMITLIB
#define SJLIB

#include "sysincl.h"

#include "qniallim.h"
#include "lib_main.h"
#include "absmach.h"
#include "ops.h"



static int pl_exit_called = 0;
static char pl_exit_text[4096];
static int pl_initialised = 0;


#define RESET_EXIT pl_exit_called = 0
#define CHECK_PLERR(fcall) { pl_exit_called = 0; \
  (fcall);                                       \
  if (pl_exit_called != 0) {                 \
  apush(makefault(pl_exit_text));  \
  freeup(x); \
  return; \
  } \
  }


static int pl_exit_handler(const char *txt) {
  strcpy(pl_exit_text, txt);
  fprintf(stdout, "***PL Error: %s\n", txt);
  fflush(stdout);
  pl_exit_called = 1;
  return 0;
}


/**
 * Initialise the system
 *
 * npl_init [device, nx, ny]
 */
void inpl_init(void) {
  nialptr x = apop();
  nialptr ndev, nx, ny;

  if (kind(x) != atype || tally(x) != 3) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  ndev = fetch_array(x, 0);
  nx   = fetch_array(x, 1);
  ny   = fetch_array(x, 2);

  if (kind(ndev) != chartype || kind(nx) != inttype || kind(ny) != inttype) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  if (pl_initialised == 0) {
    plsexit(pl_exit_handler);
    pl_initialised = 1;
  }

  CHECK_PLERR(plsdev(pfirstchar(ndev)));
  if (nx > 0 && ny > 0)
    CHECK_PLERR(plssub((PLINT)intval(nx), (PLINT)intval(ny)));
  CHECK_PLERR(plinit());

  apush(True_val);
  freeup(x);
  return;
}


/**
 * Advance to another part of the plot
 *
 * npl_adv n
 */
void inpl_adv(void) {
  nialptr x = apop();
  static int pl_initialised = 0;

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  CHECK_PLERR(pladv(intval(x)));

  apush(True_val);
  freeup(x);
  return;
}


/**
 * define the dimensions of the plot along with some options
 *
 * npl_env [xmin, ymin, xmax, ymax] []
 */
void inpl_env(void) {
  nialptr x = apop();
  nialptr ranges, popts;
  double *fptr;
  nialint *iptr;

  if (kind(x) != atype || tally(x) != 2) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  splitfb(x, &ranges, &popts);
  if (kind(ranges) != realtype || tally(ranges) != 4 || kind(popts) != inttype || tally(popts) != 2) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  fptr = pfirstreal(ranges);
  iptr = pfirstint(popts);

  plenv((PLFLT)fptr[0], (PLFLT)fptr[1], (PLFLT)fptr[2], (PLFLT)fptr[3], (PLINT)iptr[0], (PLINT)iptr[1]);

  apush(createint(1));
  freeup(x);
  return;
}


/**
 * Set the labels for the plot
 *
 * npl_lab x-axis y-axis title
 *
 */
void inpl_lab(void) {
  nialptr x = apop();
  nialptr x_axis, y_axis, ttl;

  if (kind(x) != atype || tally(x) != 3) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  x_axis = fetch_array(x, 0);
  y_axis = fetch_array(x, 1);
  ttl    = fetch_array(x, 2);

  pllab(pfirstchar(x_axis), pfirstchar(y_axis), pfirstchar(ttl));

  apush(True_val);
  freeup(x);
  return;
}


/**
 * Plot a line
 *
 * npl_line x y
 */
void inpl_line(void) {
  nialptr x = apop();
  nialptr x_vals, y_vals;

  if (kind(x) != atype || tally(x) != 2) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  x_vals = fetch_array(x, 0);
  y_vals = fetch_array(x, 1);

  if (kind(x_vals) != realtype || kind(y_vals) != realtype || tally(x_vals) != tally(y_vals)) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  /* Plot the data that was supplied */
  plline( tally(x_vals), pfirstreal(x_vals), pfirstreal(y_vals));
  
  apush(True_val);
  freeup(x);
  return;
}


/**
 * Plot a series of points with a glyph defined at each point
 *
 * npl_string x y glyph
 */
void inpl_string(void) {
  nialptr x = apop();
  nialptr x_vals, y_vals, glyph;

  if (kind(x) != atype || tally(x) != 3) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  x_vals = fetch_array(x, 0);
  y_vals = fetch_array(x, 1);
  glyph  = fetch_array(x, 2);

  if (kind(x_vals) != realtype || kind(y_vals) != realtype || kind(glyph) != chartype || tally(x_vals) != tally(y_vals)) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  /* Plot the data that was supplied */
  plstring( tally(x_vals), pfirstreal(x_vals), pfirstreal(y_vals), pfirstchar(glyph));
  
  apush(True_val);
  freeup(x);
  return;
}


/**
 * Write text on a graph
 *
 * npl_ptex [x, y, dx, dy] [just txt]
 */
void inpl_ptex(void) {
  nialptr x = apop();
  nialptr locn, p_opts, just, txt;
  double *rptr;

  if (kind(x) != atype || tally(x) != 2) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  locn = fetch_array(x, 0);
  p_opts  = fetch_array(x, 1);

  if (kind(locn) != realtype || kind(p_opts) != atype || tally(locn) != 4 || tally(p_opts) != 2) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  just = fetch_array(p_opts, 0);
  txt  = fetch_array(p_opts, 1);

  if (kind(just) != realtype || kind(txt) != chartype) {
   apush(makefault("?arg_types"));
    freeup(x);
    return;
  }


  rptr = pfirstreal(locn);

  /* Write the text that was supplied */
  plptex( (PLFLT)rptr[0], (PLFLT)rptr[1], (PLFLT)rptr[2], (PLFLT)rptr[3], 
          (PLFLT)realval(just), pfirstchar(txt));

  
  apush(True_val);
  freeup(x);
  return;
}


/**
 * Plot a histogram
 *
 * npl_hist [data, dmin, dmax, nbin, opt]
 */
void inpl_hist(void) {
  nialptr x = apop();
  nialptr ndata, dmin, dmax, nbin, nopt;
  double *rptr;

  if (kind(x) != atype || tally(x) != 5) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  ndata = fetch_array(x, 0);
  dmin  = fetch_array(x, 1);
  dmax  = fetch_array(x, 2);
  nbin  = fetch_array(x, 3);
  nopt  = fetch_array(x, 4);

  if (kind(ndata) != realtype || kind(dmin) != realtype || kind(dmax) != realtype ||
      kind(nbin) != inttype || kind(nopt) != inttype) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  /* Plot rhe histogram */
  plhist((PLINT)tally(ndata), (PLFLT*)pfirstreal(ndata), (PLFLT)realval(dmin), (PLFLT)realval(dmax),
         (PLINT)intval(nbin), (PLINT)intval(nopt)); 
  
  apush(True_val);
  freeup(x);
  return;
}


/**
 * Begin the plot
 */
void inpl_bop(void) {
  nialptr x = apop();

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  CHECK_PLERR(plbop()); 

  apush(True_val);
  freeup(x);
  return;
}


/**
 * Eject the curent plot
 */
void inpl_eop(void) {
  nialptr x = apop();

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  CHECK_PLERR(pleop()); 

  apush(True_val);
  freeup(x);
  return;
}


/**
 * Finish up the plot
 */
void inpl_end(void) {
  nialptr x = apop();

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  CHECK_PLERR(plend()); 

  apush(True_val);
  freeup(x);
  return;
}


/**
 * change pause at end of plot
 */
void inpl_spause(void) {
  nialptr x = apop();

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  switch (intval(x)) {
  case 0:
    CHECK_PLERR(plspause((PLBOOL)0));
    break;
  default:
    CHECK_PLERR(plspause((PLBOOL)1));
    break;
  }

  apush(True_val);
  freeup(x);
  return;
}


void inpl_gpage(void) {
  nialptr x = apop();
  PLFLT xp0, yp0;
  PLINT xleng0, yleng0, xoff0, yoff0;
  nialptr res;
  nialint rlen, *iptr;
  double *dptr;

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  CHECK_PLERR(plgpage( &xp0, &yp0, &xleng0, &yleng0, &xoff0, &yoff0 ));

  if (intval(x) == 0) {
    /* Return the xp0 and yp0 values */
    rlen = 2;
    res = new_create_array(realtype, 1, 0, &rlen);
    store_real(res, 0, (double)xp0);
    store_real(res, 1, (double)yp0);
  } else {
    /* return the int values */
    rlen = 4;
    res = new_create_array(inttype, 1, 0, &rlen);
    store_int(x, 0, (nialint)xleng0);
    store_int(x, 1, (nialint)yleng0);
    store_int(x, 2, (nialint)xoff0);
    store_int(x, 3, (nialint)yoff0);
  }

  apush(res);
  freeup(x);
  return;
}

  
#endif /* N_PLPLOT */
