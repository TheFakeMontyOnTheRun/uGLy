#include <conio.h>
#include <dpmi.h>
#include <go32.h>
#include <pc.h>
#include <bios.h>
#include <sys/movedata.h>
#include <sys/farptr.h>
#include <sys/nearptr.h>

#include <stdint.h>
#include <stdlib.h>

#include "internal.h"
#include <GLES/gl.h>



FramebufferPixelFormat framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#ifndef DISABLE_DEPTH_BUFFER
uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif

#ifndef DISABLE_STENCIL_BUFFER
uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif

KeyCallback keyCallback;

void initWindow( KeyCallback callback)
{
    keyCallback = callback;
    __dpmi_regs regs;

    // Initialize mouse (INT 33h, AX = 0)
    regs.x.ax = 0;
    __dpmi_int(0x33, &regs);

    textmode(C80);
    clrscr();

    __dpmi_regs reg;

    reg.x.ax = 0x13;
    __dpmi_int(0x10, &reg);

    outp(0x03c8, 0);

    for (int r = 0; r < 4; ++r) {
        for (int g = 0; g < 8; ++g) {
            for (int b = 0; b < 8; ++b) {
                outp(0x03c9, (r * (16)));
                outp(0x03c9, (g * (8)));
                outp(0x03c9, (b * (8)));
            }
        }
    }

    // the viewport must, by default, be configured to the size of the surface
    glViewport(0, 0, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
}

void graphicsShutdown(void)
{
    textmode(C80);
    clrscr();
}

struct Bitmap* loadBitmap(const char *filename)
{
    /* TODO: implement this somehow */

    return NULL;
}

void swapBuffers(void)
{
    dosmemput(&framebuffer[0], XRES_FRAMEBUFFER * YRES_FRAMEBUFFER, 0xa0000);

    if (kbhit())
    {
        char getched = getch();
        switch (getched)
        {
        case 27:
        case 'q':
            graphicsShutdown();
            exit(0);
            break;

        case 'a':
        case 'd':
        case 's':
        case 'w':
        case 'z':
        case 'x':
            keyCallback(getched);
            break;
        }
    }
}
