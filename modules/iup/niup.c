/**
 * niup.c 
 * --------
 *
 * IUP wrapper for Nial.
 *
 */


#include "switches.h"

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

#include <string.h>

#include <iup.h>


/** Nial environment at startup */
extern char **nial_envp;
extern char **nial_argv;
extern int  nial_argc;


/* ------------------- IUP Handles ----------------- */

#define NIUP_DEF_HANDLES    4096

typedef struct {
  nialint  next;
  Ihandle* nhandle;
} NHandle, *NHandlePtr;



static nialint    niup_nhandles = 0;
static nialint    niup_free = -1;
static NHandlePtr niup_handles = NULL;

#define NIUP_ACTIVE_HANDLE(i) (0 <= i && i < niup_nhandles && niup_handles[i].nhandle != NULL)

#define NIUP_FREE_HANDLE (niup_free != -1)
#define NIUP_FREE_HPAIR (niup_free != -1 && niup_handles[niup_free].next != -1)
#define NIUP_GET_HANDLE(i) niup_handles[i].nhandle
 

/*Initialise the handle table */
static int niup_init_handles(nialint n) {
  nialint i;

  /* Avoid calling if already initialised */
  if (niup_handles != NULL)
    return -1;

  /* Allocate space for handles */
  niup_nhandles = (n < NIUP_DEF_HANDLES)? NIUP_DEF_HANDLES: n;
  niup_handles = (NHandlePtr)malloc(niup_nhandles*sizeof(NHandle));

  if (niup_handles == NULL) {
    /* malloc failed */
    niup_nhandles = 0;
    niup_free = -1;
  } else {
    /* Set up the structs and free list */
    niup_free = 0;
    for (i = 0; i < niup_nhandles - 1; i++) {
      niup_handles[i].next = i+1;
      niup_handles[i].nhandle = NULL;
    }
    niup_handles[niup_nhandles-1].next = -1;
    niup_handles[niup_nhandles-1].nhandle = NULL;
  }

  return niup_nhandles;
}
 

/**
 * allocate a handle
 */
static nialint niup_addhandle(Ihandle* h) {
  nialint ih = niup_free;

  if (ih != -1) {
    niup_free = niup_handles[ih].next;
    niup_handles[ih].nhandle = h;
    niup_handles[ih].next    = -2;
  }

  return ih;
}


/**
 * Free a handle
 */
static void niup_freehandle(nialint ih) {
  if (NIUP_ACTIVE_HANDLE(ih)) {
    niup_handles[ih].next = niup_free;
    niup_handles[ih].nhandle = NULL;
    niup_free = ih;
  }
  return;
}



/* ------------------ Callbacks ----------------- */

#define NCB_DISPLAY       0
#define NCB_KEY_PRESS     1
#define NCB_SPECIAL_KEY   2
#define NCB_CLOSE_WINDOW  3


#define MAX_NCB       64

typedef struct {
  int cb_type;
  int win;
  union {
    struct {
      int key;
      int x;
      int y;
    } cb_key_press;
  } cb_data;
} NCB_Data;

static int cb_count = 0;
static NCB_Data cb_vals[MAX_NCB];

static int cb_next(int cbt) {
  if (cb_count == MAX_NCB) {
    fprintf(stderr, "*** Discarding IUP callback %d\n", cbt);
    fflush(stderr);
    return -1;
  } else {
    cb_vals[cb_count].cb_type = cbt;
    cb_vals[cb_count].win = 0;
    return cb_count++;
  }
}


/* ------------- Primitives ----------------- */
  

void iniup_open(void) {
  nialptr x = apop();
  
  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }
  
  /*Initialise the handle table */
  switch (niup_init_handles(intval(x))) {
  case -1:
    apush(makefault("already initialised"));
    break;
  case 0:
    apush(makefault("?no_mem"));
    break;
  default:
    IupOpen(&nial_argc, &nial_argv);
    apush(Null);
    break;
  }
    
    
  freeup(x);
  return;
}


void iniup_close(void) {
  nialptr x = apop();

  IupClose();

  apush(Null);
  freeup(x);
  return;
}


void iniup_mainloop(void) {
  nialptr x = apop();

  IupMainLoop();

  apush(Null);
  freeup(x);
  return;
}


void iniup_message(void) {
  nialptr x = apop();
  nialptr wname, wmsg;

  if (kind(x) != atype || tally(x) != 2) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  splitfb(x, &wname, &wmsg);
  if (kind(wname) != chartype || kind(wmsg) != chartype) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  IupMessage(pfirstchar(wname), pfirstchar(wmsg));

  apush(Null);
  freeup(x);
  return;
}


void iniup_label(void) {
  nialptr x = apop();

  if (kind(x) != chartype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  if (NIUP_FREE_HANDLE) {
    nialint ihl = niup_addhandle(IupLabel(pfirstchar(x)));
    apush(createint(ihl));
  } else {
    apush(makefault("no handles"));
  }

  freeup(x);
  return;
}


void iniup_dialog(void) {
  nialptr x = apop();
  nialptr res;
  nialint n, reslen = 2;
  nialint ihb = -1, ihd;

  if (kind(x) != inttype) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  n = intval(x);
  if (NIUP_FREE_HPAIR) {
    if (n == 1 || n == 2) {
      /* A layout box is requested */
      Ihandle *hb = (n == 1)? IupVbox(NULL): IupHbox(NULL);
      ihb = niup_addhandle(hb);
      ihd = niup_addhandle(IupDialog(hb));
    } else {
      /* No layout box */
      ihd = niup_addhandle(IupDialog(NULL));
    }

    res = new_create_array(inttype, 1, 0, &reslen);
    store_int(res, 0, ihd);
    store_int(res, 1, ihb);

    apush(res);
  } else {
    apush(makefault("no_handles"));
  }

  freeup(x);
  return;
}


void iniup_append(void) {
  nialptr x = apop();
  Ihandle *h;
  nialint i;

  if (kind(x) != inttype || !NIUP_ACTIVE_HANDLE(intval(x))) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  /* Add any elements to the main entry */
  h = NIUP_GET_HANDLE(intval(x));
  for (i = 1; i < tally(x); i++) { 
    nialint ih = fetch_int(x, i);
    if (NIUP_ACTIVE_HANDLE(ih)) {
      IupAppend(h, NIUP_GET_HANDLE(ih));
      IupMap(NIUP_GET_HANDLE(ih));
    }
  }
  IupRefresh(h);

  apush(True_val);
  freeup(x);
  return;
}

  
void iniup_set_attribute(void) {
  nialptr x = apop();
  nialptr ahandle, aname, aval;

  if (kind(x) != atype || tally(x) != 3) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }
  
  ahandle = fetch_array(x, 0);
  aname   = fetch_array(x, 1);
  aval    = fetch_array(x, 2);

  if (kind(aname) != chartype || kind(aval) != chartype || kind(ahandle) != inttype || !NIUP_ACTIVE_HANDLE(intval(ahandle))) {
    apush(makefault("?arg_types"));
    freeup(x);
    return;
  }

  IupSetAttribute(NIUP_GET_HANDLE(intval(ahandle)), pfirstchar(aname), pfirstchar(aval));

  apush(True_val);
  freeup(x);
  return;
}


static int niup_xpos[] = {IUP_LEFT, 
			  IUP_CENTER, 
			  IUP_RIGHT, 
			  IUP_MOUSEPOS,
			  IUP_CENTERPARENT,
			  IUP_CURRENT};

static int niup_ypos[] = {IUP_TOP, 
			  IUP_CENTER, 
			  IUP_BOTTOM, 
			  IUP_MOUSEPOS,
			  IUP_CENTERPARENT,
			  IUP_CURRENT};

#define NIUP_TRANS(idx,tbl,cnt) ((0<idx && idx < cnt)? tbl[idx]: tbl[0])
  


void iniup_showxy(void) {
  nialptr x = apop();
  nialint ih, ix, iy;

  if (kind(x) != inttype || tally(x) != 3 || !NIUP_ACTIVE_HANDLE(intval(x))) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  ih = fetch_int(x, 0);
  ix = fetch_int(x, 1);
  iy = fetch_int(x, 2);

  apush(createint(IupShowXY(NIUP_GET_HANDLE(ih), NIUP_TRANS(ix, niup_xpos, 6), NIUP_TRANS(iy, niup_ypos, 6))));
  freeup(x);
  return;
}


  
  
      

