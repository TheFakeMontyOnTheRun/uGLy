//
// Created by Daniel Monteiro on 27/02/2026.
//
#include <stdint.h>

#include "SDL.h"
#include "SDL_image.h"
#include "internal.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* videoTexture;

uint32_t framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

KeyCallback keyCallback;

void initWindow( KeyCallback callback)
{
    keyCallback = callback;
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");

    window =
        SDL_CreateWindow("OpenGL ES 1.0 Common Lite example",
                         SDL_WINDOWPOS_CENTERED,
                         SDL_WINDOWPOS_CENTERED, XRES_FRAMEBUFFER, YRES_FRAMEBUFFER,
                         SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(window, -1, 0);
    videoTexture = SDL_CreateTexture(renderer,
                                     SDL_PIXELFORMAT_RGBA8888,
                                     SDL_TEXTUREACCESS_STREAMING,
                                     XRES_FRAMEBUFFER, YRES_FRAMEBUFFER
    );
}

void graphicsShutdown(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

struct Bitmap* loadBitmap(const char *filename)
{
    struct Bitmap* toReturn;

    toReturn = calloc(1, sizeof( struct Bitmap));

    SDL_Surface *image = IMG_Load(filename);

    toReturn->width = image->w;
    toReturn->height = image->h;
    toReturn->texels = calloc( sizeof(uint32_t), 128 * 128);

    int pitch = image->pitch;
    int bpp = image->format->BytesPerPixel;

    if (SDL_MUSTLOCK(image)) {
        SDL_LockSurface(image);
    }

    for (int y = 0; y < image->h; y++) {
        for (int x = 0; x < image->w; x++) {
            uint8_t *pixel_ptr = image->pixels + y * pitch + x * bpp;

            uint8_t r, g, b, a;
            SDL_GetRGBA(*(uint32_t *)pixel_ptr, image->format, &r, &g, &b, &a);

            toReturn->texels[toReturn->width * y + x] = r << 24 | g << 16 | b << 8 | a;
        }
    }

    if (SDL_MUSTLOCK(image)) {
        SDL_UnlockSurface(image);
    }

    SDL_FreeSurface(image);

    return toReturn;
}

void swapBuffers(void)
{
    void* pixels;
    int pitch;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
        {
            graphicsShutdown();
            exit(0);
        }

        if (event.type == SDL_KEYDOWN)
        {
            keyCallback(event.key.keysym.sym);
        }
    }

    SDL_LockTexture(videoTexture, NULL, &pixels, &pitch);

    uint32_t* dst = (uint32_t*)pixels;

    memcpy(dst, framebuffer, sizeof(uint32_t) * XRES_FRAMEBUFFER * YRES_FRAMEBUFFER);

    SDL_UnlockTexture(videoTexture);

    SDL_RenderCopy(renderer, videoTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
