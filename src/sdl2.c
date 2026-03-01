//
// Created by Daniel Monteiro on 27/02/2026.
//
#include <stdint.h>

#include "SDL.h"

SDL_Window *window;
SDL_Renderer *renderer;
SDL_Texture *videoTexture;

#define YRES_FRAMEBUFFER 300
#define XRES_FRAMEBUFFER 300

uint32_t framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

void initWindow(void)
{

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

void graphicsShutdown(void) {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void swapBuffers(void)
{

    void *pixels;
    int pitch;
    int c;
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT) {
            graphicsShutdown();
            exit(0);
            return;
        }
    }

    SDL_LockTexture(videoTexture, NULL, &pixels, &pitch);

    uint32_t* dst = (uint32_t*)pixels;

    memcpy(dst, framebuffer, sizeof(uint32_t) * XRES_FRAMEBUFFER * YRES_FRAMEBUFFER);

    SDL_UnlockTexture(videoTexture);

    SDL_RenderCopy(renderer, videoTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
}
