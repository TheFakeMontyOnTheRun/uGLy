#include <genesis.h>
#include "resources.h"
#include "../../src/es10cl.c"
#include "../../src/raster.c"
#include "../../src/matricesFP.c"
#include "../../third_party/fpsqrt/fpsqrt.c"
#include "../../samples/tri.c"

FramebufferPixelFormat framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#ifndef DISABLE_DEPTH_BUFFER
uint16_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif

uint8_t latch = GL_NO_ERROR;

void swapBuffers(void)
{
	uint8_t x, y;

	for (y = 0; y < YRES_FRAMEBUFFER; ++y)
	{
		for (x = 0; x < XRES_FRAMEBUFFER; ++x)
		{
			int col = framebuffer[(y * XRES_FRAMEBUFFER) + x];
			BMP_setPixelFast(x, y, col | (col << 4));
		}
	}

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
	XGM_startPlay(music);
}
