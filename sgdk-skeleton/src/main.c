#include <genesis.h>
#include "resources.h"
#include "../../src/es10cl.c"
#include "../../src/raster.c"
#include "../../src/matricesFP.c"
#include "../../third_party/fpsqrt/fpsqrt.c"
#include "tex32x32xRGBA32_MD.h"
#include "tex64x64xRGBA32_IM.h"

#define FX_1 intToFix(1)
#define FX_5 intToFix(5)

void initWindow( KeyCallback callback);
void swapBuffers(void);
uint8_t demo = -1;
static GLfixed rx = 0, ry = -intToFix(360), rz = 0;

GLuint textureID[1];

static void
quads_draw(void)
{

    static const GLfixed verts[4][3] = {
        /*
         *      0    3
         *      |\---|
         *      | \  |
         *      |  \ |
         *      1---\2
         */

    { -intToFix(1),  intToFix(1),  intToFix(0) },
    { -intToFix(1), -intToFix(1),  intToFix(0) },
    {  intToFix(1), -intToFix(1),  intToFix(0) },
    {  intToFix(1),  intToFix(1),  intToFix(0) }
    };

    uint8_t indices[4] = {0, 1, 2, 3};

    static const GLfixed normals[12] = {
        intToFix(0), intToFix(0), -intToFix(1),
        intToFix(0), intToFix(0), -intToFix(1),
        intToFix(0), intToFix(0), -intToFix(1),
        intToFix(0), intToFix(0), -intToFix(1)
    };

    static const GLfixed colors[4][4] = {
        {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
        {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
        {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
        {intToFix(1), intToFix(1), intToFix(1), intToFix(1)}
    };

    static const GLfixed texCoords[8] = {
        intToFix(0), intToFix(1),
        intToFix(0), intToFix(0),
        intToFix(1), intToFix(0),
        intToFix(1), intToFix(1),
    };

    glClear(GL_COLOR_BUFFER_BIT
#ifndef DISABLE_DEPTH_BUFFER
    | GL_DEPTH_BUFFER_BIT
#endif
    );

    glPushMatrix();
    glRotatex(rx, intToFix(1), 0, 0);
    glRotatex(ry, 0, intToFix(1), 0);
    glRotatex(rz, 0, 0, intToFix(1));

    glEnable(GL_TEXTURE_2D);

#ifndef DISABLE_DEPTH_BUFFER
    glEnable(GL_DEPTH_TEST);
#endif

    glTexCoordPointer(2, GL_FIXED, 0, texCoords);
    glVertexPointer(3, GL_FIXED, 0, verts);
    glColorPointer(4, GL_FIXED, 0, colors);
    glNormalPointer(GL_FIXED, 0, normals);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    /* draw triangles */
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    glPopMatrix();
	glDisable(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, textureID[1]);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
}


/* new window size or exposure */
static void
quads_reshape(int width, int height)
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
quads_init(void)
{
    glDisable(GL_CULL_FACE);
    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glDisable(GL_NORMALIZE);

    const GLfixed grey = Div(intToFix(4), intToFix(10));
    const GLfixed fullAlpha = intToFix(1);
    glClearColorx(grey, grey, grey, fullAlpha);
}

static void
quad_draw(void)
{

    static const GLfixed verts[4][3] = {
        /*
         *      0
         *      |-- 2
         *      |  /|
         *      | / |
         *      |/--|
         *      1   3
         */


    { -FX_1, FX_1,  intToFix(0) },
    { -FX_1, -FX_1,  intToFix(0) },
    {  FX_1, FX_1,  intToFix(0) },
    {  FX_1, -FX_1,  intToFix(0) },
    };

    static const GLfixed normals[12] = {
        intToFix(0), intToFix(0), FX_1,
        intToFix(0), intToFix(0), FX_1,
        intToFix(0),  intToFix(0), FX_1,
        intToFix(0),  intToFix(0), FX_1,
    };

    static const GLfixed colors[4][4] = {
        {FX_1, FX_1, FX_1, FX_1},
        {FX_1, FX_1, FX_1, FX_1},
        {FX_1, FX_1, FX_1, FX_1},
        {FX_1, FX_1, FX_1, FX_1},
    };

    static const GLfixed texCoords[8] = {
        intToFix(0), FX_1,
        intToFix(0), intToFix(0),
        FX_1, FX_1,
        FX_1, intToFix(0),
    };



    glClear(GL_COLOR_BUFFER_BIT
#ifndef DISABLE_DEPTH_BUFFER
    | GL_DEPTH_BUFFER_BIT
#endif
    );

    glPushMatrix();
    glRotatex(rx, FX_1, 0, 0);

	glEnable(GL_TEXTURE_2D);

#ifndef DISABLE_DEPTH_BUFFER
    glEnable(GL_DEPTH_TEST);
#endif

    glTexCoordPointer(2, GL_FIXED, 0, texCoords);
    glVertexPointer(3, GL_FIXED, 0, verts);
    glColorPointer(4, GL_FIXED, 0, colors);
    glNormalPointer(GL_FIXED, 0, normals);

    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);

    /* draw triangles */
    glBindTexture(GL_TEXTURE_2D, textureID[0]);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glPopMatrix();
}


/* new window size or exposure */
static void
quad_reshape(int width, int height)
{
    const GLfixed ar = Div(intToFix(width), intToFix(height));

    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumx(-ar, ar, -FX_1, FX_1, intToFix(5), intToFix(60));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatex(0, 0, -intToFix(10));
}


static void
quad_init(void)
{
    static const GLfixed ambient[4] = { Div(FX_1, intToFix(5)), Div(FX_1, intToFix(5)), Div(FX_1, intToFix(5)), FX_1 };
    static const GLfixed pos[4] = { intToFix(0), intToFix(0), FX_1, 0 };

    glLightxv(GL_LIGHT0, GL_POSITION, pos);

    glLightModelxv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);

    const GLfixed grey = Div(intToFix(6), intToFix(10));
    const GLfixed fullAlpha = FX_1;
    glClearColorx(grey, grey, grey, fullAlpha);
}


static void
tri_draw(void)
{

    static const GLfixed verts[3][3] = {
    { -FX_1, -FX_1,  intToFix(0) },
    {  FX_1, -FX_1,  intToFix(0) },
    {      0,        FX_1,  intToFix(0) }
    };
    static const GLfixed colors[3][4] = {
    { 65536,     0,     0,    65536 },
    {     0, 65536,     0 ,   65536},
    {     0,     0, 65536 ,   65536}
    };

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glRotatex(ry, 0, FX_1, 0);
    glVertexPointer(3, GL_FIXED, 0, verts);
    glColorPointer(4, GL_FIXED, 0, colors);
    glDrawArrays(GL_TRIANGLES, 0, 3);
	glPopMatrix();
}


/* new window size or exposure */
static void
tri_reshape(int width, int height)
{
    GLfixed ar = Div(intToFix(width), intToFix(height));

    glViewport(0, 0, (GLint)width, (GLint)height);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustumx(-ar, ar, -FX_1, FX_1, intToFix(5), intToFix(60));

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatex(0, 0, -intToFix(10));
}


static void
tri_init(void)
{
    GLfixed fullAlpha = FX_1;
    GLfixed grey = Div(intToFix(6), intToFix(10));
    glClearColorx(grey, grey, grey, fullAlpha);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
}

static void
special_key(int special)
{
   switch (special) {
   case 'a':
      ry += intToFix(5);
      break;
   case 'd':
      ry -= intToFix(5);
      break;
   case 'w':
      rx += intToFix(5);
      break;
   case 's':
      rx -= intToFix(5);
      break;
   case 'z':
       rz -= intToFix(5);
       break;
   case 'x':
       rz += intToFix(5);
       break;

   default:
      break;
   }
}

void mainLoop(void)
{
	GLfixed fullAlpha = FX_1;
	GLfixed grey = Div(intToFix(4), intToFix(10));
	glGenTextures(1, &textureID[0]);
	glBindTexture(GL_TEXTURE_2D, textureID[0]);

	glTexImage2D(GL_TEXTURE_2D,
				 0,
				 GL_RGB,
				 32,
				 32,
				 0,
				 GL_RGB,
				 GL_UNSIGNED_BYTE,
				 &tex1[0]);


    while (1)
    {
    	rx -= intToFix(8);
    	ry -= intToFix(8);
    	rz -= intToFix(8);

    	if (ry < -intToFix(360))
    	{

    		rx = ry = rz = 0;
    		switch ((demo + 1) % 4)
    		{
    		case 0:
    			if (demo != -1) {
    				tri_draw();
					swapBuffers();
					tri_draw();
					swapBuffers();
    			}

    			tri_init();
    			tri_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    			break;
    		case 1:
    			glClearColorx(grey, grey, grey, fullAlpha);

    			tri_draw();
    			swapBuffers();
    			tri_draw();
    			swapBuffers();

    			quad_init();
    			quad_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    			break;
    		case 2:
    			glClearColorx(grey, grey, grey, fullAlpha);

    			quad_draw();
    			swapBuffers();
    			quad_draw();
    			swapBuffers();
    			quads_init();
    			quads_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    			break;
    		case 3:
    			glClearColorx(grey, grey, grey, fullAlpha);

    			quads_draw();
    			swapBuffers();
    			quads_draw();
    			swapBuffers();

    			tri_init();
    			tri_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    			break;
    		}
    		++demo;
    	}

    	switch (demo % 4)
    	{
    	case 0:
    		tri_draw();
    		break;
    	case 1:
    		quad_draw();
    		break;
    	case 2:
    		quads_draw();
    		break;
    	case 3:
			glScalex(intToFix(1), (-Div(ry , intToFix(360))) % intToFix(1), intToFix(1));
    		tri_draw();
    		break;
    	}

        swapBuffers();
    }
}

int
main(int argc, char* argv[]) {

    initWindow(special_key);

    mainLoop();

    return 0;
}


FramebufferPixelFormat framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#ifndef DISABLE_DEPTH_BUFFER
uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif

uint8_t latch = GL_NO_ERROR;

void swapBuffers(void)
{
	BMP_drawBitmapData(framebuffer, (demo % 4) * 64, 0, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER, XRES_FRAMEBUFFER);

	int error = glGetError();
	if (error != GL_NO_ERROR)
	{
		latch = error;
	}
	if (latch != GL_NO_ERROR) {
		char buffer[8];
		sprintf(&buffer[0], "%d", error);
		VDP_waitVSync();
		BMP_flip(1);

		BMP_drawText(&buffer[0], 0, 18);
		while (1);
	}

	if (latch == GL_NO_ERROR)
	{
		VDP_waitVSync();
		BMP_flip(1);
	}

	BMP_drawText("glClear(GL_COLOR_BUFFER_BIT |", 0, 9);
	BMP_drawText("GL_DEPTH_BUFFER_BIT);", 0, 10);
	BMP_drawText("glPushMatrix();", 0, 11);
	BMP_drawText("glVertexPointer(3, GL_FIXED, 0,", 0, 12);
	BMP_drawText("verts);", 0, 13);
	BMP_drawText("glColorPointer(4, GL_FIXED, 0,", 0, 14);
	BMP_drawText("colors);", 0, 15);
	BMP_drawText("glRotatex(FX_5, 0, FX_1, 0);", 0, 16);
	BMP_drawText("glDrawArrays(GL_TRIANGLES, 0, 3);", 0, 17);
	BMP_drawText(";", 0, 18);
	BMP_drawText("glPopMatrix();", 0, 19);
}

void initWindow( KeyCallback callback)
{
	VDP_setScreenWidth256();
	VDP_setHInterrupt(0);
	VDP_setHilightShadow(0);

	DMA_setBufferSize(2048);

	/* create virtual 256x160 framebuffer */
	BMP_init(TRUE, BG_B, PAL0, 1);

	PAL_setColor(0, RGB24_TO_VDPCOLOR( 0x000000));
	PAL_setColor(1, RGB24_TO_VDPCOLOR( 0x0000FF));
	PAL_setColor(2, RGB24_TO_VDPCOLOR( 0x005500));
	PAL_setColor(3, RGB24_TO_VDPCOLOR( 0x0055FF));
	PAL_setColor(4, RGB24_TO_VDPCOLOR( 0x00AA00));
	PAL_setColor(5, RGB24_TO_VDPCOLOR( 0x00AAFF));
	PAL_setColor(6, RGB24_TO_VDPCOLOR( 0x00FF00));
	PAL_setColor(7, RGB24_TO_VDPCOLOR( 0x00FFFF));
	PAL_setColor(8, RGB24_TO_VDPCOLOR( 0xFF0000));
	PAL_setColor(9, RGB24_TO_VDPCOLOR( 0xFF00FF));
	PAL_setColor(10, RGB24_TO_VDPCOLOR(0xFF5500));
	PAL_setColor(11, RGB24_TO_VDPCOLOR(0xFF55FF));
	PAL_setColor(12, RGB24_TO_VDPCOLOR(0xFFAA00));
	PAL_setColor(13, RGB24_TO_VDPCOLOR(0xFFAAFF));
	PAL_setColor(14, RGB24_TO_VDPCOLOR(0xFFFF00));
	PAL_setColor(15, RGB24_TO_VDPCOLOR(0xFFFFFF));


	glViewport(0, 0, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
	XGM_startPlay(music);
}
