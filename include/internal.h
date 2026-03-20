//
// Created by Daniel Monteiro on 01/03/2026.
//

#ifndef INTERNAL_H
#define INTERNAL_H
typedef void ( *KeyCallback )(int charkey);
typedef uint32_t FramebufferPixelFormat;

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
    int32_t spotDirection[4]; /* So that we don't have to pull the GL header just yet */
    int32_t direction[4];
    int32_t position[4];
    uint8_t colour[4];
};

void initWindow( KeyCallback callback);
void swapBuffers(void);
struct Bitmap* loadBitmap(const char *filename);

void drawTexturedTriangle(int *coords,
                          uint8_t *uvCoords,
                          uint8_t *colourChannels,
                          struct Texture *texture,
                          uint16_t *z,
                          uint8_t* lightDot);

#define kIntegerPart 16

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)

#define floatToFix(f) ((GLfixed)(65536.0f * (f)))

#define MIN(v1, v2) (( (v1) < (v2) ) ? (v1) : (v2) )
#define MAX(v1, v2) (( (v1) > (v2) ) ? (v1) : (v2) )


#define XRES_FRAMEBUFFER 300
#define YRES_FRAMEBUFFER 300

extern uint32_t framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint16_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

#endif // INTERNAL_H
