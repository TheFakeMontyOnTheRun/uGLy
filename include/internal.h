//
// Created by Daniel Monteiro on 01/03/2026.
//
#include <GLES/gl.h>  /* use OpenGL ES 1.x */
#ifndef INTERNAL_H
#define INTERNAL_H

#ifndef SGDK
#include <stdint.h>
#else
#include <genesis.h>
#endif

typedef void ( *KeyCallback )(int charkey);

#define XRES_FRAMEBUFFER 320
#define YRES_FRAMEBUFFER 200

#ifdef BPP24
typedef uint32_t FramebufferPixelFormat;
#define MAKE_PIXEL(r, g, b, a) ((r) << 24 | (g) << 16 | (b) << 8 | (a))
#define EMIT(destination, x_hint, y_hint, fragment) do { (void)(x_hint); (void)(y_hint);*(destination) = (fragment);} while(0)
#define ADVANCE(fbptr, x_hint, y_hint) do{ (void)(x_hint); (void)(y_hint); ++(fbptr);} while(0)
#define SEEK(framebuffer_ptr, x_pos, y_pos, pitch) ((framebuffer_ptr) + ((pitch) * (y_pos)) + (x_pos))
#define FRAMEBUFFER_PITCH (XRES_FRAMEBUFFER)

#else
#ifdef BPP16
typedef uint16_t FramebufferPixelFormat;

static inline uint16_t swap16(uint16_t x) {
    return (x >> 8) | (x << 8);
}

#ifdef SWAP__FRAMEBUFFER_BYTES
#define MAKE_PIXEL(r,g,b, a) swap16((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)) )
#else
#define MAKE_PIXEL(r,g,b, a) ((((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3)) )
#endif

#define EMIT(destination, x_hint, y_hint, fragment) do { (void)(x_hint); (void)(y_hint);*(destination) = (fragment);} while(0)
#define ADVANCE(fbptr, x_hint, y_hint) do{ (void)(x_hint); (void)(y_hint); ++(fbptr);} while(0)
#define SEEK(framebuffer_ptr, x_pos, y_pos, pitch) ((framebuffer_ptr) + ((pitch) * (y_pos)) + (x_pos))
#define FRAMEBUFFER_PITCH (XRES_FRAMEBUFFER)

#else
#ifdef BPP8
typedef uint8_t FramebufferPixelFormat;

#define MAKE_PIXEL(r,g,b, a) ( ((((r << 2) >> 8)) << 6) | ((((g << 3) >> 8)) << 3) | ((((b << 3) >> 8)) << 0) )
#define EMIT(destination, x_hint, y_hint, fragment) do { (void)(x_hint); (void)(y_hint);*(destination) = (fragment);} while(0)
#define ADVANCE(fbptr, x_hint, y_hint) do{ (void)(x_hint); (void)(y_hint); ++(fbptr);} while(0)
#define SEEK(framebuffer_ptr, x_pos, y_pos, pitch) ((framebuffer_ptr) + ((pitch) * (y_pos)) + (x_pos))
#define FRAMEBUFFER_PITCH (XRES_FRAMEBUFFER)

#else
#ifdef BPP4
typedef uint8_t FramebufferPixelFormat;

#define MAKE_PIXEL(r,g,b, a) ((((r) >> 7) << 3) | (((g) >> 6) << 1) | ((b) >> 7))
#define EMIT(destination, x_hint, y_hint, fragment) do{ (void)(y_hint); if (((x_hint) & 1) == 0) { *(destination) &=15 ; *(destination) |= ((fragment) << 4); } else { *(destination) &=~15 ; *(destination) |= (fragment);}} while(0)
#define ADVANCE(fbptr, x_hint, y_hint) do{ (void)(y_hint); if ((x_hint) & 1) {++(fbptr);}} while(0)
#define SEEK(framebuffer_ptr, x_pos, y_pos, pitch) ((framebuffer_ptr) + ((pitch) * (y_pos)) + ((x_pos) / 2))
#define FRAMEBUFFER_PITCH (XRES_FRAMEBUFFER)

#else
#ifdef BPP1
#error "1 BPP TBD"
#else
#error "No bit depth for framebuffer defined"
#endif
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
                          const uint8_t *z,
#endif
                          const uint8_t* lightDot,
                          const uint8_t* ambientLight);

void uGLyInit(void);
void drawLine(uint16_t x0, uint8_t y0, uint16_t x1, uint8_t y1, uint8_t* colours, uint8_t *zValues);
void drawPoint(int* coords, uint8_t* colour,
#ifndef	DISABLE_DEPTH_BUFFER
    uint8_t zValue,
#endif
    uint16_t pointSize);

GLfixed *currentModelViewMatrix(void);

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)

#define floatToFix(f) ((GLfixed)(65536.0f * (f)))

#define MIN(v1, v2) (( (v1) < (v2) ) ? (v1) : (v2) )
#define MAX(v1, v2) (( (v1) > (v2) ) ? (v1) : (v2) )

#define TOTAL_TEXTURES_SUPPORTED 8
#define MAX_TEXTURE_SIZE_LOG2 8
#define MAX_TEXTURE_SIZE (1 << MAX_TEXTURE_SIZE_LOG2)



#define POT(x) ( (x) == 0 ? 0 : (((x) & ((x)-1)) == 0) )
#define MATRIX_STACK_CAPACITY 16

extern FramebufferPixelFormat framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#ifndef DISABLE_DEPTH_BUFFER
extern uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t depthTestEnabled;
extern uint8_t depthWritesEnabled;
#endif

#ifndef DISABLE_STENCIL_BUFFER
extern uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
#endif


#endif // INTERNAL_H
