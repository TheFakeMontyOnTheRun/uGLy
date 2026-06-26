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

#include "internal.h"
#include <GLES/gl.h>
#include <assert.h>

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

GLfixed sinfpx(GLfixed angle);

GLfixed cosfpx(GLfixed angle);

struct gear {
    GLfixed *vertices;
    GLsizei stride;

    GLint num_teeth;
};

static GLfixed view_rotx = intToFix(20), view_roty = intToFix(30), view_rotz = 0;
static struct gear gears[3];
static GLfixed angle = 0;
#define FPX_PI (Div(intToFix(31416), intToFix(100000)))
/*
 *  Initialize a gear wheel.
 *
 *  Input:  gear - gear to initialize
 *          inner_radius - radius of hole at center
 *          outer_radius - radius at center of teeth
 *          width - width of gear
 *          teeth - number of teeth
 *          tooth_depth - depth of tooth
 */
static void
init_gear(struct gear *gear, GLfixed inner_radius, GLfixed outer_radius,
          GLfixed width, GLint teeth, GLfixed tooth_depth)
{
    GLfixed r0, r1, r2;
    GLfixed a0, da;
    GLint verts_per_tooth, total_verts, total_size;
    GLint count, i;
    GLfixed *verts;

    r0 = inner_radius;
    r1 = outer_radius - Div(tooth_depth, intToFix(2));
    r2 = outer_radius + Div(tooth_depth, intToFix(2));

    a0 = Div( Mul( intToFix(2), FPX_PI), intToFix(teeth));
    da = Div( a0, intToFix(4));

    gear->vertices = NULL;
    gear->stride = sizeof(GLfixed) * 6; /* XYZ + normal */
    gear->num_teeth = teeth;

    verts_per_tooth = 10 + 4;
    total_verts = teeth * verts_per_tooth;
    total_size = total_verts * gear->stride;

    verts = (GLfixed *)(malloc(total_size));
    if (!verts) {
        printf("failed to allocate vertices\n");
        return;
    }

#define GEAR_VERT(r, n, sign)                      \
   do {                                            \
      verts[count * 6 + 0] = Mul((r), vx[n]);          \
      verts[count * 6 + 1] = Mul((r), vy[n]);          \
      verts[count * 6 + 2] = Div(  ((sign < 0) ? -width : width),  intToFix(2) ); \
      verts[count * 6 + 3] = normal[0];            \
      verts[count * 6 + 4] = normal[1];            \
      verts[count * 6 + 5] = normal[2];            \
      count++;                                     \
   } while (0)

    count = 0;
    for (i = 0; i < teeth; i++) {
        GLfixed normal[3];
        GLfixed vx[5], vy[5];
        GLfixed u, v;

        normal[0] = 0;
        normal[1] = 0;
        normal[2] = 0;

        GLfixed fi = intToFix(i);
        GLfixed fixa0 = Mul(fi, a0);

        GLfixed angle;
        GLfixed conversion = Div(intToFix(180), FPX_PI);

        GLfixed accDa;

        accDa = 0;

        for (int c = 0; c < 5; ++c) {
            angle = Mul((fixa0 + accDa), (conversion));
            vx[c] = cosfpx(angle);
            vy[c] = sinfpx(angle);
            accDa += da;
        }

        /* outward faces of a tooth, 10 verts */
        normal[0] = vx[0];
        normal[1] = vy[0];
        GEAR_VERT(r1, 0,  1);
        GEAR_VERT(r1, 0, -1);

        u = Mul(r2, vx[1]) - Mul(r1, vx[0]);
        v = Mul(r2, vy[1]) - Mul(r1, vy[0]);
        normal[0] = v;
        normal[1] = -u;
        GEAR_VERT(r2, 1,  1);
        GEAR_VERT(r2, 1, -1);

        normal[0] = vx[0];
        normal[1] = vy[0];
        GEAR_VERT(r2, 2,  1);
        GEAR_VERT(r2, 2, -1);

        u = Mul(r1, vx[3]) - Mul(r2, vx[2]);
        v = Mul(r1, vy[3]) - Mul(r2, vy[2]);
        normal[0] = v;
        normal[1] = -u;
        GEAR_VERT(r1, 3,  1);
        GEAR_VERT(r1, 3, -1);

        normal[0] = vx[0];
        normal[1] = vy[0];
        GEAR_VERT(r1, 4,  1);
        GEAR_VERT(r1, 4, -1);

        /* inside radius cylinder, 4 verts */
        normal[0] = -vx[4];
        normal[1] = -vy[4];
        GEAR_VERT(r0, 4,  1);
        GEAR_VERT(r0, 4, -1);

        normal[0] = -vx[0];
        normal[1] = -vy[0];
        GEAR_VERT(r0, 0,  1);
        GEAR_VERT(r0, 0, -1);

        assert(count % verts_per_tooth == 0);
    }
    assert(count == total_verts);
#undef GEAR_VERT

    gear->vertices = verts;
}


static void
draw_gear(const struct gear *gear)
{
    GLint i;

    if (!gear->vertices) {
        printf("nothing to be drawn\n");
        return;
    }

    glVertexPointer(3, GL_FIXED, gear->stride, gear->vertices);
    glNormalPointer(GL_FIXED, gear->stride, gear->vertices + 3);


    glEnableClientState(GL_VERTEX_ARRAY);

    for (i = 0; i < gear->num_teeth; i++) {
        const GLint base = (10 + 4) * i;
        GLushort indices[7];

        glShadeModel(GL_FLAT);

        /* front face */
        indices[0] = base + 12;
        indices[1] = base +  0;
        indices[2] = base +  2;
        indices[3] = base +  4;
        indices[4] = base +  6;
        indices[5] = base +  8;
        indices[6] = base + 10;

        glNormal3x(0, 0, intToFix(1));
        glDrawElements(GL_TRIANGLE_FAN, 7, GL_UNSIGNED_SHORT, indices);

        /* back face */
        indices[0] = base + 13;
        indices[1] = base + 11;
        indices[2] = base +  9;
        indices[3] = base +  7;
        indices[4] = base +  5;
        indices[5] = base +  3;
        indices[6] = base +  1;

        glNormal3x(0, 0, -intToFix(1));
        glDrawElements(GL_TRIANGLE_FAN, 7, GL_UNSIGNED_SHORT, indices);

        glEnableClientState(GL_NORMAL_ARRAY);

        /* outward face of a tooth */
        glDrawArrays(GL_TRIANGLE_STRIP, base, 10);

        /* inside radius cylinder */
        glShadeModel(GL_SMOOTH);
        glDrawArrays(GL_TRIANGLE_STRIP, base + 10, 4);

        glDisableClientState(GL_NORMAL_ARRAY);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
}


static void
draw(void)
{

    static const GLfixed red[4] = { intToFix(1), 0, 0, intToFix(1) };
    static const GLfixed green[4] = { 0, intToFix(1), 0, intToFix(1) };
    static const GLfixed blue[4] = { 0, 0, intToFix(1), intToFix(1) };

    glClearColorx(0, 0, 0, intToFix(1));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatex(view_rotx, intToFix(1), 0, 0);
    glRotatex(view_roty, 0, intToFix(1), 0);
    glRotatex(view_rotz, 0, 0, intToFix(1));

    glPushMatrix();
    glTranslatex(-intToFix(3), -intToFix(2), 0);
    glRotatex(angle, 0, 0, intToFix(1));


    glMaterialxv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, red);
    draw_gear(&gears[0]);

    glPopMatrix();

    glPushMatrix();
    glTranslatex(intToFix(3), -intToFix(2), 0);
    glRotatex(-2 * angle - intToFix(9), 0, 0, intToFix(1));

    glMaterialxv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, green);
    draw_gear(&gears[1]);

    glPopMatrix();

    glPushMatrix();
    glTranslatex(-intToFix(3), intToFix(4), 0);
    glRotatex(-2 * angle - intToFix(25), 0, 0, intToFix(1));

    glMaterialxv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, blue);
    draw_gear(&gears[2]);

    glPopMatrix();

    glPopMatrix();

    angle += intToFix(1);
    angle = (angle) % intToFix(360);
}


static void init(void)
{
    static const GLfixed pos[4] = { intToFix(5), intToFix(5), intToFix(10), 0 };

    glLightxv(GL_LIGHT0, GL_POSITION, pos);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);


    init_gear(&gears[0], intToFix(1), intToFix(4), intToFix(1), 20, Div(intToFix(7), intToFix(10)));
    init_gear(&gears[1], Div(intToFix(5), intToFix(10)), intToFix(2), intToFix(2), 10, Div(intToFix(7), intToFix(10)));
    init_gear(&gears[2], Div(intToFix(13), intToFix(10)), intToFix(2), Div(intToFix(5), intToFix(10)), 10, Div(intToFix(7), intToFix(10)));
}

/* new window size or exposure */
static void
reshape(int width, int height)
{
    GLfixed h = Div(intToFix(height), intToFix(width));

    glViewport(0, 0, (GLint) width, (GLint) height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumx(-intToFix(1), intToFix(1), -h, h, intToFix(6), intToFix(60));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatex(0, 0, -intToFix(40));
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
