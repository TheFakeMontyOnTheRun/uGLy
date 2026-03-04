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
};

void initWindow( KeyCallback callback);
void swapBuffers(void);
struct Bitmap* loadBitmap(const char *filename);

void fillTriangle(int* coords, FramebufferPixelFormat* colour);

void drawTexturedTriangle(int *coords,
                          uint8_t *uvCoords,
                          struct Texture *texture,
                          int z);

#endif // INTERNAL_H
