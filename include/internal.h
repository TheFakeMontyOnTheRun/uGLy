//
// Created by Daniel Monteiro on 01/03/2026.
//
#include <GLES/gl.h>  /* use OpenGL ES 1.x */
#ifndef INTERNAL_H
#define INTERNAL_H
typedef void ( *KeyCallback )(int charkey);


#ifdef BPP24
typedef uint32_t FramebufferPixelFormat;
#define MAKE_PIXEL(r, g, b, a) ((r) << 24 | (g) << 16 | (b) << 8 | (a))
#else
#ifdef BPP16
typedef uint16_t FramebufferPixelFormat;

static inline uint16_t swap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

#ifdef SDL_DISABLE_IMMINTRIN_H
#define MAKE_PIXEL(r,g,b, a) ((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)) )
#else
#define MAKE_PIXEL(r,g,b, a) swap16((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)) )
#endif

#else
#ifdef BPP8
typedef uint8_t FramebufferPixelFormat;
#error "8 BPP TBD"
#else
#ifdef BPP1
#error "1 BPP TBD"
#else
#error "No bit depth for framebuffer defined"
#endif
#endif
#endif
#endif

struct Bitmap
{
    uint32_t *texels;
    int width;
    int height;
};

struct Texture
{
    int width;
    int height;
    uint32_t *texels;
    uint8_t inUse;
};

struct Light
{
    uint8_t enabled;
    GLfixed spotDirection[4];
    GLfixed direction[4];
    GLfixed position[4];
    uint8_t colour[4];
};

void initWindow( KeyCallback callback);
void swapBuffers(void);
struct Bitmap* loadBitmap(const char *filename);

void drawTexturedTriangle(const int *coords,
                          const uint8_t *uvCoords,
                          const uint8_t *colourChannels,
                          const struct Texture *texture,
#ifndef	DISABLE_DEPTH_BUFFER
                          const uint16_t *z,
#endif
                          const uint8_t* lightDot,
                          const uint8_t* ambientLight);

void drawPoint(int* coords, uint8_t* colour,
#ifndef	DISABLE_DEPTH_BUFFER
    uint16_t zValue,
#endif
    uint16_t pointSize);

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)

#define floatToFix(f) ((GLfixed)(65536.0f * (f)))

#define MIN(v1, v2) (( (v1) < (v2) ) ? (v1) : (v2) )
#define MAX(v1, v2) (( (v1) > (v2) ) ? (v1) : (v2) )


#define XRES_FRAMEBUFFER 240
#define YRES_FRAMEBUFFER 240

extern FramebufferPixelFormat framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#ifndef DISABLE_DEPTH_BUFFER
extern uint16_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t depthTestEnabled;
extern uint8_t depthWritesEnabled;
#endif

#ifndef DISABLE_STENCIL_BUFFER
extern uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif


#endif // INTERNAL_H
