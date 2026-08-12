//
// Created by Daniel Monteiro on 27/02/2026.
//
#include <stdint.h>

extern "C" {
#include "internal.h"
#include <GLES/gl.h>
}

FramebufferPixelFormat *framebuffer;

#ifndef DISABLE_DEPTH_BUFFER
uint8_t *zBuffer;
#endif

#ifndef DISABLE_STENCIL_BUFFER
uint8_t *stencilBuffer;
#endif
