#include <genesis.h>
#include "resources.h"
#include "../../src/es10cl.c"
#include "../../src/raster.c"
#include "../../src/matricesFP.c"
#include "../../third_party/fpsqrt/fpsqrt.c"

#define FX_1 intToFix(1)
#define FX_5 intToFix(5)

void initWindow( KeyCallback callback);
void swapBuffers(void);

static GLfixed rx = 0, ry = 0, rz = 0;

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
	glRotatex(FX_5, 0, FX_1, 0);
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
    GLfixed grey = Div(intToFix(4), intToFix(10));
    GLfixed fullAlpha = FX_1;
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

void tri_mainLoop(void)
{
    tri_reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
    while (1)
    {
        tri_draw();
        swapBuffers();
    }
}

int
main(int argc, char* argv[]) {

    initWindow(special_key);

    tri_init();

    tri_mainLoop();

    return 0;
}


FramebufferPixelFormat framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#ifndef DISABLE_DEPTH_BUFFER
uint16_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif

uint8_t latch = GL_NO_ERROR;

void swapBuffers(void)
{
	BMP_drawBitmapData(framebuffer, 0, 0, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER, XRES_FRAMEBUFFER);

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
