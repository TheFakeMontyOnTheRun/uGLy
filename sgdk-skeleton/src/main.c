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

uint16_t strlen ( const char * str );

void initWindow( KeyCallback callback);
void swapBuffers(void);
int8_t demo = -1;
static GLfixed rx = 0, ry = -intToFix(360), rz = 0;

GLuint textureID;

void typeStringDelay(const char* str, int x, int y, int margin, int delay)
{
	int c, d, e;
	c = x;
	d = y;
	e = 0;
	char buffer[2];
	buffer[1] = 0;
	size_t len = strlen(str);
	for (; e < len; ++c, ++e)
	{
		if (c >= margin || c >= 32)
		{
			c = x;
			++d;
		}

		if (str[e] == '\n')
		{
			c = x - 1; /* it will be incremented on the loop */
			++d;
			continue;
		}

		buffer[0] = str[e];
		BMP_drawText(&buffer[0], c, d);
		waitMs(delay);
	}
}

void typeString(const char* str, int x, int y, int margin)
{
	typeStringDelay(str, x, y, margin, 50);
}


void justDrawString(const char* str, int x, int y, int margin)
{
	typeStringDelay(str, x, y, margin, 0);
}

void justDrawStringDblBuf(const char* str, int x, int y, int margin)
{
	for (int c = 9; c < 20; ++c)
	{
		BMP_clearText(0, c, 32);
	}
	justDrawString(str, x, y, margin);
	swapBuffers();
	for (int c = 9; c < 20; ++c)
	{
		BMP_clearText(0, c, 32);
	}
	justDrawString(str, x, y, margin);
	swapBuffers();
}

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

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	glBindTexture(GL_TEXTURE_2D, textureID);
    glPushMatrix();
    glRotatex(rx, intToFix(1), 0, 0);
    glRotatex(ry, 0, intToFix(1), 0);
    glRotatex(rz, 0, 0, intToFix(1));
    glEnable(GL_TEXTURE_2D);
    glTexCoordPointer(2, GL_FIXED, 0, texCoords);
    glVertexPointer(3, GL_FIXED, 0, verts);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glPopMatrix();
	glDisable(GL_TEXTURE_2D);
    glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, indices);
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

	const GLfixed grey = Div(intToFix(6), intToFix(10));
	const GLfixed fullAlpha = FX_1;
	glClearColorx(grey, grey, grey, fullAlpha);

	glEnable(GL_DEPTH_TEST);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
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

    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glRotatex(rx, FX_1, 0, 0);
	glColorPointer(4, GL_FIXED, 0, colors);
    glTexCoordPointer(2, GL_FIXED, 0, texCoords);
    glVertexPointer(3, GL_FIXED, 0, verts);
    glNormalPointer(GL_FIXED, 0, normals);
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

	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glEnable(GL_TEXTURE_2D);
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

void clearTextScreen()
{
	for (int c = 9; c < 20; ++c)
	{
		BMP_clearText(0, c, 32);
	}
}

void intro(void)
{
	typeString("Have you ever wanted to run OpenGL ES on a SEGA Mega Drive/Genesis?", 0, 9, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();

	typeString("Yeah, neither have I...but hear me out.", 0, 9, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();


	typeString("With the recent progress on uGLy (WIP OpenGL ES 1.0 CL implementation), "
			"this suddenly became possible!", 0, 9, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();

	typeString("Buckle up, this is going to be...", 0, 9, 32);
	waitMs(1000);
	typeString("...slow?", 0, 10, 32);
	waitMs(1000);
	typeString("...underwhelming?", 0, 11, 32);
	waitMs(1000);
	typeString("...glitchy?", 0, 12, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();

	typeString("Welcome to...", 0, 9, 32);
	waitMs(1000);
	typeString("68K problems (but a blit ain't one)!", 0, 10, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();
}

void showOutro(void)
{
	swapBuffers();
	clearTextScreen();
	swapBuffers();
	clearTextScreen();
	swapBuffers();
	clearTextScreen();
	swapBuffers();

	typeString("I know, I know, this sucked badly, but it's just the start. There's a lot of work ahead!\nSlapped together in a week!", 0, 9, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();

	typeString("Code by MontyOnTheRun, Music by Jeenio", 0, 9, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();


	typeString("Greetz to BelleTiger, Acassis, Jannone, Leiradel and the fine folks at Retromania.pt and Inercia", 0, 9, 32);
	waitMs(2000);
	clearTextScreen();
	swapBuffers();

	typeString("I GOT BLISTERS ON MY FINGERS!", 0, 9, 32);
	while (1);
}

void mainLoop(void)
{

	intro();
	XGM_startPlay(music);
	GLfixed fullAlpha = FX_1;
	GLfixed grey = Div(intToFix(4), intToFix(10));
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

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
    		switch ((demo + 1))
    		{
    		case 0:
    			tri_init();
    			tri_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    			justDrawStringDblBuf("glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);\n"
    							"glPushMatrix();\n"
    							"glVertexPointer(3, GL_FIXED, 0, verts);\n"
    							"glColorPointer(4, GL_FIXED, 0, colors);\n"
    							"glRotatex(FX_5, 0, FX_1, 0);\n"
    							"glDrawArrays(GL_TRIANGLES, 0, 3);\n"
    							"glPopMatrix();\n", 0, 9, 32);
    			break;
    		case 1:
    			glClearColorx(grey, grey, grey, fullAlpha);

    			tri_draw();
    			swapBuffers();
    			tri_draw();
    			swapBuffers();

    			quad_init();
    			quad_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    			justDrawStringDblBuf("glClear(GL_COLOR_BUFFER_BIT);\n"
    									"glPushMatrix();\n"
										"glRotatex(FX_1, FX_1, 0, 0);\n"
										"glTexCoordPointer(2, GL_FIXED, 0, texCoords);\n"
										"glVertexPointer(3, GL_FIXED, 0, verts);\n"
										"glColorPointer(4, GL_FIXED, 0, colors);\n"
										"glNormalPointer(GL_FIXED, 0, normals);", 0, 9, 32);
    			waitMs(2000);
    			justDrawStringDblBuf("glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);\n"
										"glPopMatrix();", 0, 9, 32);

    			break;
    		case 2:
    			glClearColorx(grey, grey, grey, fullAlpha);

    			quad_draw();
    			swapBuffers();
    			quad_draw();
    			swapBuffers();

    			quads_init();
    			quads_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);



    			justDrawStringDblBuf("glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );\n"
										"glBindTexture(GL_TEXTURE_2D, textureID);\n"
										"glPushMatrix();\n"
										"glRotatex(rx, FX_1, 0, 0);\n"
										"glRotatex(ry, 0, FX_1, 0);\n"
										"glRotatex(rz, 0, 0, FX_1);\n"
										"glEnable(GL_TEXTURE_2D);\n"
										"glTexCoordPointer(2, GL_FIXED, 0, texCoords);", 0, 9, 32);
				waitMs(2000);
    			justDrawStringDblBuf("glTexCoordPointer(2, GL_FIXED, 0, texCoords);\n"
										"glVertexPointer(3, GL_FIXED, 0, verts);\n"
										"glDrawArrays(GL_TRIANGLE_FAN, 0, 4);\n"
										"glPopMatrix();\n"
										"glDisable(GL_TEXTURE_2D);\n"
										"glDrawElements(GL_TRIANGLE_FAN, 4, GL_UNSIGNED_BYTE, indices);", 0, 9, 32);
    			break;
    		case 3:
    			glClearColorx(grey, grey, grey, fullAlpha);

    			quads_draw();
    			swapBuffers();
    			quads_draw();
    			swapBuffers();
    			goto outro;
    		}
    		++demo;
    	}

    	switch (demo)
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
    	}

        swapBuffers();
    }

outro:
	showOutro();
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
	BMP_drawBitmapData(framebuffer, ( ((demo % 4) + 1) * 16) + ( (demo % 4) * 64), 0, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER, XRES_FRAMEBUFFER);

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
}
