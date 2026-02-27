/* File based on https://gitlab.freedesktop.org/mesa/demos/-/blob/main/src/egl/opengles1/tri.c?ref_type=heads
 * Please visit the original to see some proper code ;)
 */

/*
 * Copyright (C) 2008  Brian Paul   All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 */

/*
 * Draw a triangle with X/EGL and OpenGL ES 1.x
 * Brian Paul
 * 5 June 2008
 */

#define USE_FIXED_POINT 1


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <GLES/gl.h>  /* use OpenGL ES 1.x */

void initWindow(void);
void swapBuffers(void);

#define FLOAT_TO_FIXED(X)   ((X) * 65535.0)

static GLfloat view_rotx = 0.0, view_roty = 0.0, view_rotz = 0.0;

static void
draw(void)
{

   static const GLfixed verts[3][2] = {
      { -65536, -65536 },
      {  65536, -65536 },
      {      0,  65536 }
   };
   static const GLfixed colors[3][4] = {
      { 65536,     0,     0,    65536 },
      {     0, 65536,     0 ,   65536},
      {     0,     0, 65536 ,   65536}
   };

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glPushMatrix();
   glRotatef(view_rotx, 1, 0, 0);
   glRotatef(view_roty, 0, 1, 0);
   glRotatef(view_rotz, 0, 0, 1);

   {

      glVertexPointer(2, GL_FIXED, 0, verts);
      glColorPointer(4, GL_FIXED, 0, colors);

      glEnableClientState(GL_VERTEX_ARRAY);
      glEnableClientState(GL_COLOR_ARRAY);

      /* draw triangle */
      glDrawArrays(GL_TRIANGLES, 0, 3);

      /* draw some points */
      glPointSizex(FLOAT_TO_FIXED(15.5));
      glDrawArrays(GL_POINTS, 0, 3);

      glDisableClientState(GL_VERTEX_ARRAY);
      glDisableClientState(GL_COLOR_ARRAY);
   }


   glPopMatrix();
}


/* new window size or exposure */
static void
reshape(int width, int height)
{
   GLfloat ar = (GLfloat) width / (GLfloat) height;

   glViewport(0, 0, (GLint) width, (GLint) height);

   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustumx(-ar, ar, -1, 1, 5.0, 60.0);

   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   glTranslatef(0.0, 0.0, -10.0);

   swapBuffers();
}


static void
init(void)
{
   glClearColor(0.4, 0.4, 0.4, 0.0);
}

// static void
// special_key(int special)
// {
//    switch (special) {
//    case EGLUT_KEY_LEFT:
//       view_roty += 5.0;
//       break;
//    case EGLUT_KEY_RIGHT:
//       view_roty -= 5.0;
//       break;
//    case EGLUT_KEY_UP:
//       view_rotx += 5.0;
//       break;
//    case EGLUT_KEY_DOWN:
//       view_rotx -= 5.0;
//       break;
//    default:
//       break;
//    }
//    eglutPostRedisplay();
// }


void mainLoop(void)
{
   while (1)
   {
      draw();
   }
}


int
main(int argc, char *argv[])
{
   initWindow();

   init();

   mainLoop();

   return 0;
}
