#include "gba_video.h"
#include "gba_systemcalls.h"
#include "gba_input.h"
#include "gba_interrupt.h"
#include "fade.h"
#include <gba_dma.h>

#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "internal.h"
#include <GLES/gl.h>

#define VRAM_PAGE_A ((uint8_t*)0x6000000)
#define VRAM_PAGE_B ((uint8_t*)0x600A000)

FramebufferPixelFormat *framebuffer;

#ifndef DISABLE_DEPTH_BUFFER
uint8_t *zBuffer;
#endif

#ifndef DISABLE_STENCIL_BUFFER
uint8_t *stencilBuffer;
#endif

KeyCallback keyCallback;

static void VblankInterrupt(void) {
	scanKeys();
}

uint8_t getPaletteEntry(const uint32_t origin) {
	uint8_t shade;

	if (!(origin & 0xFF000000)) {
		return 119;
	}

	shade = 0;
	shade += (((((origin & 0x0000FF)) << 2) >> 8)) << 6;
	shade += (((((origin & 0x00FF00) >> 8) << 3) >> 8)) << 3;
	shade += (((((origin & 0xFF0000) >> 16) << 3) >> 8)) << 0;

	return shade;
}

void initWindow( KeyCallback callback)
{
	keyCallback = callback;
	int r, g, b;
	uint16_t palette[256];

	// Set up the interrupt handlers
	irqInit();

	irqSet(IRQ_VBLANK, VblankInterrupt);

	// Enable Vblank Interrupt to allow VblankIntrWait
	irqEnable(IRQ_VBLANK);

	// Allow Interrupts
	REG_IME = 1;

	SetMode(MODE_4 | BG2_ON);        // screen mode & background to display

	framebuffer = (FramebufferPixelFormat *) malloc(XRES_FRAMEBUFFER * YRES_FRAMEBUFFER * sizeof(FramebufferPixelFormat));

#ifndef DISABLE_DEPTH_BUFFER
	zBuffer = (uint8_t *) malloc(XRES_FRAMEBUFFER * YRES_FRAMEBUFFER);
#endif

#ifndef DISABLE_STENCIL_BUFFER
	stencilBuffer = (uint8_t *) malloc(XRES_FRAMEBUFFER * YRES_FRAMEBUFFER);
#endif


	memset(palette, 0, sizeof(uint16_t) * 256);

	for (b = 0; b < 256; b += 16) {
		for (g = 0; g < 256; g += 8) {
			for (r = 0; r < 256; r += 8) {
				uint32_t pixel = 0xFF000000 + (b << 16) + (g << 8) + (r);
				uint8_t paletteEntry = getPaletteEntry(pixel);
				palette[paletteEntry] = RGB8(r - 0x38, g - 0x18, b - 0x10);
			}
		}
	}

	FadeToPalette(palette, 60);

    // the viewport must, by default, be configured to the size of the surface
    glViewport(0, 0, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
}

void graphicsShutdown(void)
{
}

struct Bitmap* loadBitmap(const char *filename)
{
    /* TODO: implement this somehow */

    return NULL;
}

void swapBuffers(void) {
	dmaCopy(framebuffer, VRAM_PAGE_A, 240 * 160);
	VBlankIntrWait();
}
