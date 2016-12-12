/**
 * nopengl_supp.c 
 * --------------
 *
 * Support code for OpenGL wrapper for Nial. OpenGL uses
 * C macros that conflict with Nial macros. This code
 * provides a layer of separation.  
 *
 */


#include "switches.h"

#define N_OPENGL

#ifdef N_OPENGL

#include <string.h>

#ifdef OSX
#include <GL/glut.h>
#include <GL/openglut.h>
#endif
#ifdef LINUX
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#endif


/**
 * Return the id of the current window
 */
int  n_glutGetWindow() {
  return glutGetWindow();
}


void n_glutDisplayFunc(void (*func)(void)) {
  glutDisplayFunc(func);
  return;
}


/**
 * Set the window close option to control termination
 */
void n_glutCloseFunc(void (*func)(void)) {
  glutCloseFunc(func);
  glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);
  return;
}


void n_glutSpecialFunc(void (*func)(int key, int x, int y)) {
  glutSpecialFunc(func);
  return;
}


extern void n_glutKeyboardFunc(void (*func)(unsigned char key, int x, int y)) {
  glutKeyboardFunc(func);
  return;
}


static GLbitfield glbfmasks[4] = {
  GL_COLOR_BUFFER_BIT,
  GL_DEPTH_BUFFER_BIT,
  GL_ACCUM_BUFFER_BIT,
  GL_STENCIL_BUFFER_BIT
};

void n_glClear(int mode) {
  GLbitfield mask = 0;
  int i, b = 1;

  for (i = 0, b = 1; i < 4; i++, b <<= 1) { 
    if (mode & b)
      mask |= glbfmasks[i];
  }

  glClear (mask);
  return;
}


void n_glFlush() {
  glFlush();
  return;
}

void n_glutSwapBuffers(void) {
  glutSwapBuffers();
}


void n_glutPostRedisplay(void) {
  glutPostRedisplay();
  return;
}
 

void n_glutInit(int nial_argc, char **nial_argv) {
  glutInit(&nial_argc, nial_argv);
}


#define NUM_GLUT_DISPLAY_MODES 12
unsigned int n_glut_display_modes[NUM_GLUT_DISPLAY_MODES] = {
  GLUT_RGBA,
  GLUT_RGB,
  GLUT_SINGLE,
  GLUT_SINGLE,
  GLUT_DOUBLE,
  GLUT_ACCUM,
  GLUT_ALPHA,
  GLUT_DEPTH,
  GLUT_STENCIL,
  GLUT_MULTISAMPLE,
  GLUT_STEREO,
  GLUT_LUMINANCE};


void n_glutInitDisplayMode(int nmode) {
  int i, mask = 1, mode = 0;

  for (i = 0; i < 12; i++) {
    if (nmode & mask)
      mode |= n_glut_display_modes[i];
    mask = mask << 1;
  }

  glutInitDisplayMode(mode);
  return;
}


int n_glutCreateWindow(char *ttl) {
  int win = glutCreateWindow(ttl);
  return win;
}


void n_glutMainLoopEvent(void) {
  glutMainLoopEvent();
  return;
}
    

void n_glRotate(double angle, double x, double y, double z) {
  glRotated(angle, x, y, z);
  return;
}

static GLenum glb_modes[10] = {
  GL_POINTS, 
  GL_LINES, 
  GL_LINE_STRIP, 
  GL_LINE_LOOP, 
  GL_TRIANGLES, 
  GL_TRIANGLE_STRIP, 
  GL_TRIANGLE_FAN, 
  GL_QUADS, 
  GL_QUAD_STRIP,
  GL_POLYGON};

  
void n_glBegin (int mode) {
  glBegin(glb_modes[mode%10]);
  return;
}


void n_glEnd (void) {
  glEnd();
  return;
}



void n_glVertex3(double x, double y, double z) {
  glVertex3d(x, y, z);
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

#endif /* N_OPENGL */
