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

#include <stdint.h>
#define USE_FIXED_POINT 1

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)


#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <GLES/gl.h>  /* use OpenGL ES 1.x */

void initWindow(void);
void swapBuffers(void);

static GLfixed view_rotx = 0, view_roty = 0, view_rotz = 0;

static void
draw(void)
{

    static const GLfixed verts[3][3] = {
    { -intToFix(1), -intToFix(1),  intToFix(0) },
    {  intToFix(1), -intToFix(1),  intToFix(0) },
    {      0,        intToFix(1),  intToFix(0) }
    };
    static const GLfixed colors[3][4] = {
    { 65536,     0,     0,    65536 },
    {     0, 65536,     0 ,   65536},
    {     0,     0, 65536 ,   65536}
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatex(view_rotx, 1, 0, 0);
    glRotatex(view_roty, 0, 1, 0);
    glRotatex(view_rotz, 0, 0, 1);

    {
        glVertexPointer(3, GL_FIXED, 0, verts);
        glColorPointer(4, GL_FIXED, 0, colors);

        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        /* draw triangle */
        glDrawArrays(GL_TRIANGLES, 0, 3);

        /* draw some points */
        // glPointSizex(Div(intToFix(31), intToFix(2)));
        // glDrawArrays(GL_POINTS, 0, 3);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }


    glPopMatrix();
}


/* new window size or exposure */
static void
reshape(int width, int height)
{
    GLfixed ar = Div(intToFix(width), intToFix(height));

    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumx(-ar, ar, -intToFix(1), intToFix(1), intToFix(5), intToFix(60));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatex(0, 0, -intToFix(10));
}


static void
init(void)
{
    GLfixed grey = Div(intToFix(4), intToFix(10));
    GLfixed fullAlpha = intToFix(1);
    glClearColorx(grey, grey, grey, fullAlpha);
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
    reshape(300, 300);
    while (1)
    {
        draw();
        swapBuffers();
    }
}


int
main(int argc, char* argv[])
{
    initWindow();

    init();

    mainLoop();

    return 0;
}
