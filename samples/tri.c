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

#define floatToFix(f) ((GLfixed)(65536.0f * (f)))

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <GLES/gl.h>  /* use OpenGL ES 1.x */

#include "internal.h"

static GLfixed view_rotx = 0, view_roty = 0, view_rotz = 0;

struct Bitmap* texture;



GLuint textureID[2];

static void
draw(void)
{

    static const GLfixed verts[6][3] = {
        /*
         *         1
         *         ^
         *        / |
         *       /  |
         *      0----2
         */


    { -intToFix(1), -intToFix(1),  intToFix(0) },
    {  intToFix(1),  intToFix(1),  intToFix(0) },
    {  intToFix(1), -intToFix(1),  intToFix(0) },

        /*
         *      4
         *      |-- 5
         *      |  /
         *      | /
         *      3
         */


    { -intToFix(1), -intToFix(1),  intToFix(0) },
    { -intToFix(1), intToFix(1),  intToFix(0) },
    {  intToFix(1), intToFix(1),  intToFix(0) },
    };

    static const GLfixed normals[18] = {
        -intToFix(1), intToFix(0), intToFix(0),
        intToFix(1), intToFix(0), intToFix(0),
        intToFix(0),  intToFix(1), intToFix(0),
        -intToFix(1), intToFix(0), intToFix(0),
        intToFix(1), intToFix(0), intToFix(0),
        intToFix(0),  intToFix(1), intToFix(0),
    };

    static const GLfixed colors[6][4] = {
        {intToFix(1), intToFix(0), intToFix(0), intToFix(1)},
        {intToFix(1), intToFix(0), intToFix(0), intToFix(1)},
        {intToFix(1), intToFix(0), intToFix(0), intToFix(1)},

        {intToFix(1), intToFix(0), intToFix(0), intToFix(1)},
        {intToFix(1), intToFix(0), intToFix(0), intToFix(1)},
        {intToFix(1), intToFix(0), intToFix(0), intToFix(1)},
    };

    static const GLfixed texCoords[12] = {
        intToFix(0), intToFix(0),
        intToFix(1), intToFix(1),
        intToFix(1), intToFix(0),

        intToFix(0), intToFix(0),
        intToFix(0), intToFix(1),
        intToFix(1), intToFix(1),
    };



    glClear(GL_COLOR_BUFFER_BIT
#ifndef DISABLE_DEPTH_BUFFER
    | GL_DEPTH_BUFFER_BIT
#endif
    );

    glPushMatrix();
    glRotatex(view_rotx, intToFix(1), 0, 0);
    glRotatex(view_roty, 0, intToFix(1), 0);
    glRotatex(view_rotz, 0, 0, intToFix(1));


    // glEnable(GL_TEXTURE_2D);

#ifndef DISABLE_DEPTH_BUFFER
    glEnable(GL_DEPTH_TEST);
#endif

    glTexCoordPointer(2, GL_FIXED, 0, texCoords);
    glVertexPointer(3, GL_FIXED, 0, verts);
    glColorPointer(4, GL_FIXED, 0, colors);
    glNormalPointer(GL_FIXED, 0, normals);

    // glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    // glEnableClientState(GL_NORMAL_ARRAY);

    /* draw triangles */
    // glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);


    ///TODO: try glLoadIdentity here..I dare you. I DOUBLE DARE YOU

    // glBindTexture(GL_TEXTURE_2D, textureID[1]);
    // glDrawArrays(GL_TRIANGLES, 0, 6);

    /* draw some points */
    glPointSizex(Div(intToFix(31), intToFix(2)));
    glDrawArrays(GL_POINTS, 0, 6);
    glPopMatrix();

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    // glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    // glDisableClientState(GL_NORMAL_ARRAY);
}


/* new window size or exposure */
static void
reshape(int width, int height)
{
    const GLfixed ar = Div(intToFix(width), intToFix(height));

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
    static const GLfixed ambient[4] = { Div(intToFix(1), intToFix(5)), Div(intToFix(1), intToFix(5)), Div(intToFix(1), intToFix(5)), intToFix(1) };
    static const GLfixed pos[4] = { -intToFix(1), -intToFix(1), intToFix(0), 0 };

    glLightxv(GL_LIGHT0, GL_POSITION, pos);

    glLightModelxv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    const GLfixed grey = Div(intToFix(4), intToFix(10));
    const GLfixed fullAlpha = intToFix(1);
    glClearColorx(grey, grey, grey, fullAlpha);

    // glGenTextures(2, &textureID[0]);


    // glBindTexture(GL_TEXTURE_2D, textureID[0]);
    // texture = loadBitmap("res/opengles.png");
    //
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_RGB,
    //              texture->width,
    //              texture->height,
    //              0,
    //              GL_RGB,
    //              GL_UNSIGNED_BYTE,
    //              texture->texels);
    // free(texture);
    //
    // glBindTexture(GL_TEXTURE_2D, textureID[1]);
    // texture = loadBitmap("res/bricks.png");
    //
    // glTexImage2D(GL_TEXTURE_2D,
    //              0,
    //              GL_RGB,
    //              texture->width,
    //              texture->height,
    //              0,
    //              GL_RGB,
    //              GL_UNSIGNED_BYTE,
    //              texture->texels);
    // free(texture);
}

static void
special_key(int special)
{
   switch (special) {
   case 'a':
      view_roty += intToFix(5);
      break;
   case 'd':
      view_roty -= intToFix(5);
      break;
   case 'w':
      view_rotx += intToFix(5);
      break;
   case 's':
      view_rotx -= intToFix(5);
      break;
   case 'z':
       view_rotz -= intToFix(5);
       break;
   case 'x':
       view_rotz += intToFix(5);
       break;

   case '0':
       view_rotx = 0;
       view_roty = 0;
       view_rotz = 0;
       break;

   default:
      break;
   }
}


void mainLoop(void)
{
    reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    while (1)
    {
        draw();

        view_rotx += Div(intToFix(2), intToFix(10));
        view_roty += Div(intToFix(3), intToFix(10));

        swapBuffers();
    }
}


int
main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    initWindow(special_key);

    init();

    mainLoop();

    return 0;
}
