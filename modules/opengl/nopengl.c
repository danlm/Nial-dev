/**
 * nopengl.c 
 * --------
 *
 * OpenGL wrapper for Nial.
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


extern int  n_glutGetWindow();
extern void n_glClear(int mode);
extern void n_glFlush();
extern void n_glutSwapBuffers(void);
extern void n_glutPostRedisplay(void);
extern void n_glutInit(int argc, char **argv);
extern void n_glutInitDisplayMode(int mode);
extern int  n_glutCreateWindow(char *title);
extern void n_glutMainLoopEvent(void);
extern void n_glRotate(double angle, double x, double y, double z);
extern void n_glBegin(int mode);
extern void n_glEnd(void);
extern void n_glVertex3(double x, double y, double z);
extern void n_glutDisplayFunc(void (*func)(void));
extern void n_glutSpecialFunc(void (*func)(int key, int x, int y));
extern void n_glutKeyboardFunc(void (*func)(unsigned char key, int x, int y));
extern void n_glutCloseFunc(void (*func)(void));




/** Nial environment at startup */
extern char **nial_envp;
extern char **nial_argv;
extern int  nial_argc;


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
    fprintf(stderr, "*** Discarding OpenGL callback %d\n", cbt);
    fflush(stderr);
    return -1;
  } else {
    cb_vals[cb_count].cb_type = cbt;
    cb_vals[cb_count].win = n_glutGetWindow();
    return cb_count++;
  }
}

  
/**
 * Callback when key pressed
 */
static void ngl_key_press(unsigned char key, int x, int y) {
  int cid = cb_next(NCB_KEY_PRESS);

  if (cid != -1) {
    cb_vals[cid].cb_data.cb_key_press.key = key;
    cb_vals[cid].cb_data.cb_key_press.x   = x;
    cb_vals[cid].cb_data.cb_key_press.y   = y;
  }

  return;
}


/**
 * Callback when key pressed
 */
static void ngl_special_key_press(int key, int x, int y) {
  int cid = cb_next(NCB_SPECIAL_KEY);

  if (cid != -1) {
    cb_vals[cid].cb_data.cb_key_press.key = key;
    cb_vals[cid].cb_data.cb_key_press.x   = x;
    cb_vals[cid].cb_data.cb_key_press.y   = y;
  }

  return;
}


static void ngl_display () {
  cb_next(NCB_DISPLAY);
  return;
}


static void ngl_closefunc () {
  cb_next(NCB_CLOSE_WINDOW);
  return;
}


    
void inglClear(void) {
  nialptr x = apop();

  n_glClear (intval(x));

  apush(Null);
  freeup(x);
  return;
}


void inglFlush(void) {
  nialptr x = apop();

  n_glFlush();

  apush(Null);
  freeup(x);
  return;
}


void inglutSwapBuffers(void) {
  nialptr x = apop();

  n_glutSwapBuffers();

  apush(Null);
  freeup(x);
  return;
}


void inglutPostRedisplay(void) {
  nialptr x = apop();

  n_glutPostRedisplay();
  
  freeup(x);
  apush(Null);
  return;
}
 

void inglutInit(void) {
  nialptr x = apop();
  
  n_glutInit(nial_argc, nial_argv);

  
  freeup(x);
  apush(True_val);
  return;
}


void inglutInitDisplayMode(void) {
  nialptr x = apop();
  nialint *iptr;
  nialint i;
  unsigned int mode = 0;

  if (kind(x) != inttype) {
    apush(makefault("??args"));
    freeup(x);
    return;
  }

  n_glutInitDisplayMode(intval(x));
  
  apush(True_val);
  freeup(x);
  return;
}


void inglutCreateWindow(void) {
  nialptr x = apop();
  int win;
  
  if (kind(x) != chartype) {
    apush(makefault("??args"));
    freeup(x);
    return;
  }

  win = n_glutCreateWindow(pfirstchar(x));

  /* Set the display functions */ 
  n_glutDisplayFunc(ngl_display);
  n_glutKeyboardFunc(ngl_key_press);
  n_glutSpecialFunc(ngl_special_key_press);
  n_glutCloseFunc(ngl_closefunc);

  apush(createint(win));
  freeup(x);
  return;
}


void inglutMainLoopEvent() {
  nialptr x = apop();

  n_glutMainLoopEvent();

  if (cb_count > 0) {
    nialint rlen = cb_count;
    nialptr res = new_create_array(atype, 1, 0, &rlen);
    int i;
    
    for (i = 0; i < cb_count; i++) {
      nialptr  eres;
      nialint  elen;

      switch(cb_vals[i].cb_type) {
      case NCB_DISPLAY:
        elen = 2;
        eres = new_create_array(inttype, 1, 0, &elen);
	store_int(eres, 0, NCB_DISPLAY);
	store_int(eres, 1, cb_vals[i].win);
	break;
      case NCB_KEY_PRESS:
	elen = 5;
	eres  = new_create_array(inttype, 1, 0, &elen);
	store_int(eres, 0, NCB_KEY_PRESS);
	store_int(eres, 1, cb_vals[i].win);
	store_int(eres, 2, cb_vals[i].cb_data.cb_key_press.key);
	store_int(eres, 3, cb_vals[i].cb_data.cb_key_press.x);
	store_int(eres, 4, cb_vals[i].cb_data.cb_key_press.y);
	break;
      case NCB_SPECIAL_KEY:
	elen = 5;
	eres  = new_create_array(inttype, 1, 0, &elen);
	store_int(eres, 0, NCB_SPECIAL_KEY);
	store_int(eres, 1, cb_vals[i].win);
	store_int(eres, 2, cb_vals[i].cb_data.cb_key_press.key);
	store_int(eres, 3, cb_vals[i].cb_data.cb_key_press.x);
	store_int(eres, 4, cb_vals[i].cb_data.cb_key_press.y);
	break;
      case NCB_CLOSE_WINDOW: 
        elen = 2;
        eres = new_create_array(inttype, 1, 0, &elen);
	store_int(eres, 0, NCB_CLOSE_WINDOW);
	store_int(eres, 1, cb_vals[i].win);
	break;
      default:
	eres = createint(cb_vals[i].cb_type);
	break;
      }

      store_array(res, i, eres);
    }

    cb_count = 0;

    apush(res);
  } else {
    apush(Null);
  }

  freeup(x);
  return;
}
    

void inglRotate(void) {
  nialptr nx = apop();
  double *dptr;
  double angle, x, y, z;

  if (kind(nx) != realtype || tally(nx) != 4) {
    apush(makefault("?args"));
    freeup(nx);
    return;
  }

  dptr = pfirstreal(nx);
  angle = *dptr++;
  x     = *dptr++;
  y     = *dptr++;
  z     = *dptr++;

  n_glRotate(angle, x, y, z);
  
  apush(Null);
  freeup(nx);
  return;
}


void inglBegin (void) {
  nialptr x = apop();
  nialint idx;

  if (kind(x) != inttype || intval(x) < 0 || intval(x) >= 10) {
    apush(makefault("?args"));
    freeup(x);
    return;
  }

  n_glBegin(intval(x));
  
  apush(Null);
  freeup(x);
  return;
}


void inglEnd (void) {
  nialptr x = apop();

  n_glEnd();
  
  apush(Null);
  freeup(x);
  return;
}



void inglVertex(void) {
  nialptr nx = apop();
  double *dptr;
  double x, y, z;

  if (kind(nx) != realtype || tally(nx) != 3) {
    apush(makefault("?args"));
    freeup(nx);
    return;
  }

  dptr = pfirstreal(nx);
  x     = *dptr++;
  y     = *dptr++;
  z     = *dptr++;

  n_glVertex3(x, y, z);
  
  apush(Null);
  freeup(nx);
  return;
}


#ifdef NEXT_BIT
void init ()
{
	glEnable (GL_DEPTH_TEST);
	glMatrixMode (GL_PROJECTION);
	gluPerspective (45.0, 1.0, 1.0, 200.0);
	glMatrixMode (GL_MODELVIEW);
	gluLookAt( 0.0,   0.0, -100.0,
    		   0.0,   0.0,    0.0,
        	   0.0,   1.0,    1.0);
}
#endif 

