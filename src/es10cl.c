//
// Created by Daniel Monteiro on 27/02/2026.
//

#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLES/gl.h>

#include "fpsqrt.h"
#include "internal.h"
#include "matricesFP.h"

GLfixed sinfp[91] =
{
    0,
    1143,
    2287,
    3429,
    4571,
    5711,
    6850,
    7986,
    9120,
    10252,
    11380,
    12504,
    13625,
    14742,
    15854,
    16961,
    18064,
    19160,
    20251,
    21336,
    22414,
    23486,
    24550,
    25606,
    26655,
    27696,
    28729,
    29752,
    30767,
    31772,
    32768,
    33753,
    34728,
    35693,
    36647,
    37589,
    38521,
    39440,
    40347,
    41243,
    42125,
    42995,
    43852,
    44695,
    45525,
    46340,
    47142,
    47929,
    48702,
    49460,
    50203,
    50931,
    51643,
    52339,
    53019,
    53683,
    54331,
    54963,
    55577,
    56175,
    56755,
    57319,
    57864,
    58393,
    58903,
    59395,
    59870,
    60326,
    60763,
    61183,
    61583,
    61965,
    62328,
    62672,
    62997,
    63302,
    63589,
    63856,
    64103,
    64331,
    64540,
    64729,
    64898,
    65047,
    65176,
    65286,
    65376,
    65446,
    65496,
    65526,
    65536,
};


static const GLfixed dummyColors[6][4] = {
    {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
    {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
    {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},

    {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
    {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
    {intToFix(1), intToFix(1), intToFix(1), intToFix(1)},
};

static const GLfixed dummyNormals[9] = {
    intToFix(1), intToFix(1), intToFix(1),
    intToFix(1), intToFix(1), intToFix(1),
    intToFix(1), intToFix(1), intToFix(1)
};

static const GLfixed dummyTexCoords[12] = {
    intToFix(0), intToFix(0),
    intToFix(0), intToFix(0),
    intToFix(0), intToFix(0),

    intToFix(0), intToFix(0),
    intToFix(0), intToFix(0),
    intToFix(0), intToFix(0),
};

uint32_t dummyTexels[1] = {0xFFFFFFFF};

struct Texture dummyTexture =  {
    .height =  1,
    .width = 1,
    .texels = &dummyTexels[0]
};


GLfixed sinfpx(GLfixed angle)
{
    angle %= intToFix(360);
    if (angle < 0)
        angle += intToFix(360);

    int deg = fixToInt(angle);   /* 0..359 */

    if (deg <= 90)
        return sinfp[deg];

    if (deg <= 180)
        return sinfp[180 - deg];

    if (deg <= 270)
        return -sinfp[deg - 180];

    return -sinfp[360 - deg];
}

GLfixed cosfpx(GLfixed angle)
{
    return sinfpx(angle + intToFix(90));
}

GLuint currentTexture = 0;

struct Texture textures[TOTAL_TEXTURES_SUPPORTED];

uint8_t textureMapping2DEnabled = 0;

GLenum currentError = GL_NO_ERROR;

uint8_t matrixStackTop = 0;

GLsizei vertexStride = 0;
GLenum vertexType = 0;
GLint vertexSize = 0;
const GLvoid* vertexPointer = NULL;
uint8_t vertexArrayEnabled = GL_FALSE;

GLsizei textureCoordStride = 0;
GLenum textureCoordType = 0;
GLint textureCoordSize = 0;
const GLvoid* textureCoordPointer = NULL;
uint8_t textureCoordsEnabled = GL_FALSE;


uint16_t viewportX;
uint16_t viewportY;
GLfixed halfViewportWidthx;
GLfixed halfViewportHeightx;

GLenum matrixMode;

GLfixed projectionMatrix[16] = {
    intToFix(1), 0, 0, 0,
    0, intToFix(1), 0, 0,
    0, 0, intToFix(1), 0,
    0, 0, 0, intToFix(1)
};
GLfixed modelViewMatrix[16] = {
    intToFix(1), 0, 0, 0,
    0, intToFix(1), 0, 0,
    0, 0, intToFix(1), 0,
    0, 0, 0, intToFix(1)
};

GLfixed projectionMatrixStack[16][16];
GLfixed modelViewMatrixStack[16][16];

GLsizei colorStride = 0;
GLenum colorType = 0;
GLint colorSize = 0;
const GLvoid* colorPointer = NULL;
uint8_t colorArrayEnabled = GL_FALSE;


GLsizei normalsStride = 0;
GLenum normalsType = 0;
const GLvoid* normalsPointer = NULL;
uint8_t normalsArrayEnabled = GL_FALSE;

GLfixed ambientColour[4];

GLfixed pointSize = intToFix(1);

#ifndef	DISABLE_DEPTH_BUFFER
uint8_t depthWritesEnabled = 1;
uint8_t depthTestEnabled = 0;
uint16_t clearDepth = 0xFFFF;
GLfixed zRange;
#endif

FramebufferPixelFormat clearColor;

#ifndef DISABLE_STENCIL_BUFFER
uint8_t clearStencil = 0;
#endif

uint8_t backfaceCullingEnabled;
uint8_t normalizeNormals;
uint8_t lightsEnabled;

struct Light lights[8] = {
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    },
    {
        .enabled = 0,
    }
};

static void notImplementedYet(const char* funcName)
{
    puts("Not implemented yet");
    printf("Function called: %s\n", funcName);
    assert(0);
}

void uGLyInit(void)
{
    currentTexture = 0;
    textureMapping2DEnabled = 0;
    currentError = GL_NO_ERROR;
    matrixStackTop = 0;
    vertexStride = 0;
    vertexType = 0;
    vertexSize = 0;
    vertexPointer = NULL;
    vertexArrayEnabled = GL_FALSE;
    textureCoordStride = 0;
    textureCoordType = 0;
    textureCoordSize = 0;
    textureCoordPointer = NULL;
    textureCoordsEnabled = GL_FALSE;
    mat4x4_identity(&projectionMatrix[0]);
    mat4x4_identity(&modelViewMatrix[0]);
    colorStride = 0;
    colorType = 0;
    colorSize = 0;
    colorPointer = NULL;
    colorArrayEnabled = GL_FALSE;
    normalsStride = 0;
    normalsType = 0;
    normalsPointer = NULL;
    normalsArrayEnabled = GL_FALSE;
    pointSize = intToFix(1);

#ifndef	DISABLE_DEPTH_BUFFER
    depthWritesEnabled = 1;
    depthTestEnabled = 0;
    clearDepth = 0xFFFF;
#endif

#ifndef DISABLE_STENCIL_BUFFER
    clearStencil = 0;
#endif
}

GLAPI void APIENTRY glActiveTexture(GLenum texture)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glAlphaFuncx(GLenum func, GLclampx ref)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glBindTexture(GLenum target, GLuint texture)
{
    if (target == GL_TEXTURE_2D)
    {
        currentTexture = texture;
    } else
    {
        currentError = GL_INVALID_ENUM;
    }
}

GLAPI void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glClear(GLbitfield mask)
{
    int c;
    if ((mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT)
    {
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            framebuffer[c] = clearColor;
        }
    }

#ifndef DISABLE_DEPTH_BUFFER
    if ((mask & GL_DEPTH_BUFFER_BIT) == GL_DEPTH_BUFFER_BIT)
    {
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            zBuffer[c] = clearDepth;
        }
    }
#endif

#ifndef DISABLE_STENCIL_BUFFER
    if ((mask & GL_STENCIL_BUFFER_BIT) == GL_STENCIL_BUFFER_BIT)
    {
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            stencilBuffer[c] = clearStencil;
        }
    }
#endif
    ///TODO: check for error conditions
}

GLAPI void APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    clearColor = MAKE_PIXEL(fixToInt(Mul(intToFix(0xFF), red)), fixToInt(Mul(intToFix(0xFF),green)), fixToInt(Mul(intToFix(0xFF),blue)), fixToInt(Mul(intToFix(0xFF),alpha)));
}

GLAPI void APIENTRY glClearDepthx(GLclampx depth)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glClearStencil(GLint s)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glClientActiveTexture(GLenum texture)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    colorStride = stride;
    colorType = type;
    colorSize = size;
    colorPointer = pointer;

    if (size != 2 && size != 3 && size != 4)
    {
        currentError = GL_INVALID_VALUE;
    }

    if (stride < 0)
    {
        currentError = GL_INVALID_VALUE;
    }
    ///TODO: handle errors on type
}

GLAPI void APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                                           GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                              GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width,
                                     GLsizei height, GLint border)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y,
                                        GLsizei width, GLsizei height)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glCullFace(GLenum mode)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDeleteTextures(GLsizei n, const GLuint* texturesIn)
{
    GLuint* ptr = texturesIn;

    for (int c = 0; c < n; ++c)
    {
        GLuint index = *ptr++;
        free(textures[index].texels);
        textures[index].texels = NULL;
        textures[index].inUse = 0;
    }
}

GLAPI void APIENTRY glDepthFunc(GLenum func)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDepthMask(GLboolean flag)
{
#ifndef	DISABLE_DEPTH_BUFFER
    depthWritesEnabled = flag;
#endif
}

GLAPI void APIENTRY glDepthRangex(GLclampx zNear, GLclampx zFar)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDisable(GLenum cap)
{
    switch (cap)
    {
    case GL_TEXTURE_2D:
        textureMapping2DEnabled = GL_FALSE;
        break;
#ifndef	DISABLE_DEPTH_BUFFER
    case GL_DEPTH_TEST:
        depthTestEnabled = GL_FALSE;
        break;
#endif
    case GL_CULL_FACE:
        backfaceCullingEnabled = GL_FALSE;
        break;
    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
    case GL_LIGHT4:
    case GL_LIGHT5:
    case GL_LIGHT6:
    case GL_LIGHT7:
        lights[cap - GL_LIGHT0].enabled = GL_FALSE;
        break;
    case GL_NORMALIZE:
        normalizeNormals = GL_FALSE;
        break;
    case GL_LIGHTING:
        lightsEnabled = GL_FALSE;
        break;
    default:
        notImplementedYet(__func__);
    }
}

GLAPI void APIENTRY glDisableClientState(GLenum array)
{
    switch (array)
    {
    case GL_COLOR_ARRAY:
        colorArrayEnabled = GL_FALSE;
        break;
    case GL_VERTEX_ARRAY:
        vertexArrayEnabled = GL_FALSE;
        break;
    case GL_TEXTURE_COORD_ARRAY:
        textureCoordsEnabled = GL_FALSE;
        break;
    case GL_NORMAL_ARRAY:
        normalsArrayEnabled = GL_FALSE;
        break;
    default:
        notImplementedYet(__func__);
    }
    ///TODO: handle other client states
}

GLAPI void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (!vertexArrayEnabled)
    {
        /* if this is disabled, what are we even doing here? */
        return;
    }

    ///TODO: better place the mvp matrix computation
    GLfixed mvp[16];
    mat4x4_mul(&modelViewMatrix[0], &projectionMatrix[0], &mvp[0]);

    switch (mode)
    {
    case GL_TRIANGLES:
        {
            int c;
            int finalCount = count / 3;
            int firstTrig = first / 3;
            GLfixed *vertexPtr;
            GLfixed *uvPtr;
            GLfixed *cPtr;
            GLfixed *nPtr;
	        struct Texture* texture;

            vertexPtr = (GLfixed*)vertexPointer;

            if (textureCoordsEnabled)
            {
                uvPtr = (GLfixed*)textureCoordPointer;
            } else
            {
                uvPtr = &dummyTexCoords[0];
            }

            if (colorArrayEnabled)
            {
                cPtr = (GLfixed*)colorPointer;
            } else
            {
                cPtr = (GLfixed*)&dummyColors[0];
            }

            if (normalsArrayEnabled)
            {
                nPtr = (GLfixed*)normalsPointer;
            } else
            {
                nPtr = (GLfixed*)&dummyNormals[0];
            }

            if (textureMapping2DEnabled)
            {
                texture = &textures[currentTexture];
            } else
            {
                texture = &dummyTexture;
            }


            for (c = 0; c < firstTrig; ++c)
            {
                uvPtr += 6;
                vertexPtr += 9;
                cPtr += 12;
                nPtr += 9;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[16];
                GLfixed transformed[16];
                GLfixed transformedNormals[16];

                vecs[0] = *(vertexPtr + 0);
                vecs[1] = *(vertexPtr + 1);
                vecs[2] = *(vertexPtr + 2);
                vecs[3] = intToFix(1);

                vecs[4] = *(vertexPtr + 3);
                vecs[5] = *(vertexPtr + 4);
                vecs[6] = *(vertexPtr + 5);
                vecs[7] = intToFix(1);

                vecs[8] = *(vertexPtr + 6);
                vecs[9] = *(vertexPtr + 7);
                vecs[10] = *(vertexPtr + 8);
                vecs[11] = intToFix(1);

                mat4x4_transformVec(&transformed[0], &mvp[0], &vecs[0]);
                mat4x4_transformVec(&transformed[4], &mvp[0], &vecs[4]);
                mat4x4_transformVec(&transformed[8], &mvp[0], &vecs[8]);

                vecs[0] = *(nPtr + 0);
                vecs[1] = *(nPtr + 1);
                vecs[2] = *(nPtr + 2);
                vecs[3] = intToFix(0);

                vecs[4] = *(nPtr + 3);
                vecs[5] = *(nPtr + 4);
                vecs[6] = *(nPtr + 5);
                vecs[7] = intToFix(0);

                vecs[8] = *(nPtr + 6);
                vecs[9] = *(nPtr + 7);
                vecs[10] = *(nPtr + 8);
                vecs[11] = intToFix(0);

                mat4x4_transformVec(&transformedNormals[0], &mvp[0], &vecs[0]);
                mat4x4_transformVec(&transformedNormals[4], &mvp[0], &vecs[4]);
                mat4x4_transformVec(&transformedNormals[8], &mvp[0], &vecs[8]);

                uint8_t lightsDot[24];

                for (int d = 0; d < 8; ++d )
                {
                    if ( lightsEnabled)
                    {
                        if (lights[d].enabled)
                        {
                            t_vec4 normalizedLight;

                            t_vec4 normalizedNormal0;
                            t_vec4 normalizedNormal1;
                            t_vec4 normalizedNormal2;

                            if (normalizeNormals)
                            {
                                normalizeVec(&lights[d].position[0], &normalizedLight[0]);

                                normalizeVec(&transformedNormals[0], &normalizedNormal0[0]);
                                normalizeVec(&transformedNormals[4], &normalizedNormal1[0]);
                                normalizeVec(&transformedNormals[8], &normalizedNormal2[0]);

                            } else
                            {
                                memcpy(&normalizedLight[0], &lights[d].position[0], sizeof(GLfixed) * 4);

                                memcpy(&normalizedNormal0[0], &transformedNormals[0], sizeof(GLfixed) * 4);
                                memcpy(&normalizedNormal1[0], &transformedNormals[4], sizeof(GLfixed) * 4);
                                memcpy(&normalizedNormal2[0], &transformedNormals[8], sizeof(GLfixed) * 4);
                            }

                            GLfixed dot0 = dotVec( &normalizedLight[0],  &normalizedNormal0[0]);
                            GLfixed dot1 = dotVec( &normalizedLight[0],  &normalizedNormal1[0]);
                            GLfixed dot2 = dotVec( &normalizedLight[0],  &normalizedNormal2[0]);

                            lightsDot[d * 3 + 0] = fixToInt(Mul(MAX(0, dot0), intToFix(256)));
                            lightsDot[d * 3 + 1] = fixToInt(Mul(MAX(0, dot1), intToFix(256)));
                            lightsDot[d * 3 + 2] = fixToInt(Mul(MAX(0, dot2), intToFix(256)));
                        } else
                        {
                            lightsDot[d * 3 + 0] = 0;
                            lightsDot[d * 3 + 1] = 0;
                            lightsDot[d * 3 + 2] = 0;
                        }
                    }else
                    {
                        ambientColour[0]=ambientColour[1]=ambientColour[2]=ambientColour[3]=intToFix(1);

                        lightsDot[d * 3 + 0] = 0;
                        lightsDot[d * 3 + 1] = 0;
                        lightsDot[d * 3 + 2] = 0;
                    }
                }

                if ((transformed[3] == 0) || (transformed[7] == 0) || (transformed[12] == 0))
                {
                    goto nextTriangle;
                }

                GLfixed oneOverW0 = Div(intToFix(1), transformed[3]);
                GLfixed oneOverW1 = Div(intToFix(1), transformed[7]);
                GLfixed oneOverW2 = Div(intToFix(1), transformed[11]);

                GLfixed vertex[6] = {
                    Mul(oneOverW0, transformed[0]), Mul(oneOverW0, transformed[1]),
                    Mul(oneOverW1, transformed[4]), Mul(oneOverW1, transformed[5]),
                    Mul(oneOverW2, transformed[8]), Mul(oneOverW2, transformed[9]),
                };

                GLfixed z0 = Mul(transformed[2], oneOverW0) + intToFix(1);
                GLfixed z1 = Mul(transformed[6], oneOverW1) + intToFix(1);
                GLfixed z2 = Mul(transformed[10], oneOverW2) + intToFix(1);

#ifndef	DISABLE_DEPTH_BUFFER
                uint16_t zValuesNormalized[3] ={
                    fixToInt(Mul(z0, zRange)),
                    fixToInt(Mul(z1, zRange)),
                    fixToInt(Mul(z2, zRange))
                };
#endif

                int coords[6] = {
                    viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[0])),
                    viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[1])),
                    viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[2])),
                    viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[3])),
                    viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[4])),
                    viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[5]))
                };
                uint8_t coloursArray[12] = {

                        fixToInt(Mul( *(cPtr +  0 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  1 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  2 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  3 ), intToFix(0xFF))),

                        fixToInt(Mul( *(cPtr +  4 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  5 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  6 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  7 ), intToFix(0xFF))),

                        fixToInt(Mul( *(cPtr +  8 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  9 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr + 10 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr + 11 ), intToFix(0xFF)))
                };

                uint8_t uvCoords[6] = {
                    fixToInt(Mul(*(uvPtr + 0), intToFix(texture->width ))),
                    fixToInt(Mul(*(uvPtr + 1), intToFix(texture->height))),
                    fixToInt(Mul(*(uvPtr + 2), intToFix(texture->width ))),
                    fixToInt(Mul(*(uvPtr + 3), intToFix(texture->height))),
                    fixToInt(Mul(*(uvPtr + 4), intToFix(texture->width ))),
                    fixToInt(Mul(*(uvPtr + 5), intToFix(texture->height))),
                };

                uint8_t ambientColourComponents[3] = {
                    fixToInt(Mul( ambientColour[0], intToFix(0xFF))),
                    fixToInt(Mul( ambientColour[1], intToFix(0xFF))),
                    fixToInt(Mul( ambientColour[2], intToFix(0xFF))),
                };

                drawTexturedTriangle(&coords[0], &uvCoords[0], &coloursArray[0], texture,
#ifndef	DISABLE_DEPTH_BUFFER
                    &zValuesNormalized[0],
#endif
                    &lightsDot[0], &ambientColourComponents[0]);

nextTriangle:

                vertexPtr += 9;

                if (normalsArrayEnabled)
                {
                    nPtr += 9;
                }
                if (textureCoordsEnabled)
                {
                    uvPtr += 6;
                }
                if (colorArrayEnabled)
                {
                    cPtr += 12;
                }
            }
        }
        break;
    case GL_POINTS:
 {
            int c;
            int finalCount = count / 3;
            int firstTrig = first / 3;
            GLfixed *vertexPtr;
            GLfixed *cPtr;

            vertexPtr = (GLfixed*)vertexPointer;

            if (colorArrayEnabled)
            {
                cPtr = (GLfixed*)colorPointer;
            } else
            {
                cPtr = (GLfixed*)&dummyColors[0];
            }

            for (c = 0; c < firstTrig; ++c)
            {
                vertexPtr += 9;
                cPtr += 12;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[16];
                GLfixed transformed[16];

                vecs[0] = *(vertexPtr + 0);
                vecs[1] = *(vertexPtr + 1);
                vecs[2] = *(vertexPtr + 2);
                vecs[3] = intToFix(1);

                vecs[4] = *(vertexPtr + 3);
                vecs[5] = *(vertexPtr + 4);
                vecs[6] = *(vertexPtr + 5);
                vecs[7] = intToFix(1);

                vecs[8] = *(vertexPtr + 6);
                vecs[9] = *(vertexPtr + 7);
                vecs[10] = *(vertexPtr + 8);
                vecs[11] = intToFix(1);

                mat4x4_transformVec(&transformed[0], &mvp[0], &vecs[0]);
                mat4x4_transformVec(&transformed[4], &mvp[0], &vecs[4]);
                mat4x4_transformVec(&transformed[8], &mvp[0], &vecs[8]);

                if ((transformed[3] == 0) || (transformed[7] == 0) || (transformed[12] == 0))
                {
                    goto nextPoint;
                }

                GLfixed oneOverW0 = Div(intToFix(1), transformed[3]);
                GLfixed oneOverW1 = Div(intToFix(1), transformed[7]);
                GLfixed oneOverW2 = Div(intToFix(1), transformed[11]);

                GLfixed vertex[6] = {
                    Mul(oneOverW0, transformed[0]), Mul(oneOverW0, transformed[1]),
                    Mul(oneOverW1, transformed[4]), Mul(oneOverW1, transformed[5]),
                    Mul(oneOverW2, transformed[8]), Mul(oneOverW2, transformed[9]),
                };

                GLfixed z0 = Mul(transformed[2], oneOverW0) + intToFix(1);
                GLfixed z1 = Mul(transformed[6], oneOverW1) + intToFix(1);
                GLfixed z2 = Mul(transformed[10], oneOverW2) + intToFix(1);
#ifndef	DISABLE_DEPTH_BUFFER
                uint16_t zValuesNormalized[3] ={
                    fixToInt(Mul(z0, zRange)),
                    fixToInt(Mul(z1, zRange)),
                    fixToInt(Mul(z2, zRange))
                };
#endif
                int coords[6] = {
                    viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[0])),
                    viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[1])),
                    viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[2])),
                    viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[3])),
                    viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[4])),
                    viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[5]))
                };
                uint8_t coloursArray[12] = {

                        fixToInt(Mul( *(cPtr +  0 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  1 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  2 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  3 ), intToFix(0xFF))),

                        fixToInt(Mul( *(cPtr +  4 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  5 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  6 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  7 ), intToFix(0xFF))),

                        fixToInt(Mul( *(cPtr +  8 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr +  9 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr + 10 ), intToFix(0xFF))),
                        fixToInt(Mul( *(cPtr + 11 ), intToFix(0xFF)))
                };

                drawPoint(&coords[0], &coloursArray[0],
#ifndef	DISABLE_DEPTH_BUFFER
                    zValuesNormalized[0],
#endif
                    fixToInt(pointSize));

                drawPoint(&coords[2], &coloursArray[4],
#ifndef	DISABLE_DEPTH_BUFFER
                    zValuesNormalized[1],
#endif
                    fixToInt(pointSize));

                drawPoint(&coords[4], &coloursArray[8],
#ifndef	DISABLE_DEPTH_BUFFER
                    zValuesNormalized[2],
#endif
                    fixToInt(pointSize));

nextPoint:
                vertexPtr += 9;

                if (colorArrayEnabled)
                {
                    cPtr += 12;
                }
            }
        }
        break;
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
    case GL_LINES:
    case GL_TRIANGLE_STRIP:
    case GL_TRIANGLE_FAN:
    default:
        notImplementedYet(__func__);
    }
}

GLAPI void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glEnable(GLenum cap)
{
    switch (cap)
    {
    case GL_TEXTURE_2D:
        textureMapping2DEnabled = GL_TRUE;
        break;
#ifndef	DISABLE_DEPTH_BUFFER
    case GL_DEPTH_TEST:
        depthTestEnabled = GL_TRUE;
        break;
#endif
    case GL_CULL_FACE:
        backfaceCullingEnabled = GL_TRUE;
        break;
    case GL_LIGHT0:
    case GL_LIGHT1:
    case GL_LIGHT2:
    case GL_LIGHT3:
    case GL_LIGHT4:
    case GL_LIGHT5:
    case GL_LIGHT6:
    case GL_LIGHT7:
        lights[cap - GL_LIGHT0].enabled = GL_TRUE;
        break;
    case GL_NORMALIZE:
        normalizeNormals = GL_TRUE;
        break;
    case GL_LIGHTING:
        lightsEnabled = GL_TRUE;
        break;
    default:
        notImplementedYet(__func__);
    }
}

GLAPI void APIENTRY glEnableClientState(GLenum array)
{
    switch (array)
    {
    case GL_COLOR_ARRAY:
        colorArrayEnabled = GL_TRUE;
        break;
    case GL_VERTEX_ARRAY:
        vertexArrayEnabled = GL_TRUE;
        break;
    case GL_TEXTURE_COORD_ARRAY:
        textureCoordsEnabled = GL_TRUE;
        break;
    case GL_NORMAL_ARRAY:
        normalsArrayEnabled = GL_TRUE;
        break;
    default:
        notImplementedYet(__func__);
    }
}

GLAPI void APIENTRY glFinish(void)
{
    /* no-op; might change in the future */
}

GLAPI void APIENTRY glFlush(void)
{
    /* no-op; might change in the future */
}

GLAPI void APIENTRY glFogx(GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glFogxv(GLenum pname, const GLfixed* params)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glFrontFace(GLenum mode)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    memset(&projectionMatrix[0], 0, sizeof(GLfixed) * 16);
    GLfixed twoTimesN = Mul(intToFix(2), zNear);
    projectionMatrix[0] = Div(twoTimesN, (right - left ));
    projectionMatrix[5] = Div(twoTimesN, (top - bottom));
    projectionMatrix[8] = Div((right + left), (right - left));
    projectionMatrix[9] = Div((top + bottom), (top - bottom));
    projectionMatrix[10] = -Div((zFar + zNear), ( zFar - zNear ));
    projectionMatrix[11] = -intToFix(1);
    projectionMatrix[14] = -Div(Mul( twoTimesN, zFar), (zFar - zNear));

#ifndef	DISABLE_DEPTH_BUFFER
    zRange = zFar - zNear;
#endif
}

GLuint reserveTexture(void)
{
    for (int c = 0; c < TOTAL_TEXTURES_SUPPORTED; ++c)
    {
        if (!textures[c].inUse)
        {
            textures[c].inUse = 1;
            return c;
        }
    }

    return 0xFFFF;
}

GLAPI void APIENTRY glGenTextures(GLsizei n, GLuint* texturesOut)
{
    GLuint* ptr = texturesOut;

    for (int c = 0; c < n; ++c)
    {
        *ptr++ = reserveTexture();
    }
}

GLAPI GLenum APIENTRY glGetError(void)
{
    GLenum previousError = currentError;
    currentError = GL_NO_ERROR;
    return previousError;
}

GLAPI void APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
    notImplementedYet(__func__);
}

GLAPI const GLubyte* APIENTRY glGetString(GLenum name)
{
    notImplementedYet(__func__);
    return NULL;
}

GLAPI void APIENTRY glHint(GLenum target, GLenum mode)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightModelx(GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightModelxv(GLenum pname, const GLfixed* params)
{
    switch (pname)
    {
    case GL_LIGHT_MODEL_AMBIENT:
        memcpy(ambientColour, params, sizeof(GLfixed) * 4);
        break;
    default:
        notImplementedYet(__func__);
    }
}

GLAPI void APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed* params)
{
    GLfixed *dir = params;

    if (pname != GL_POSITION)
    {
        notImplementedYet(__func__);
    }

    memcpy(&lights[light - GL_LIGHT0].position[0], dir, sizeof(GLfixed) * 4);
}

GLAPI void APIENTRY glLineWidthx(GLfixed width)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLoadIdentity(void)
{
    switch (matrixMode)
    {
    case GL_PROJECTION:
        mat4x4_identity(projectionMatrix);
        break;
    case GL_MODELVIEW:
        mat4x4_identity(modelViewMatrix);
        break;
    case GL_TEXTURE:
    default:
        notImplementedYet(__func__);
    }
}

GLAPI void APIENTRY glLoadMatrixx(const GLfixed* m)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLogicOp(GLenum opcode)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glMaterialxv(GLenum face, GLenum pname, const GLfixed* params)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glMatrixMode(GLenum mode)
{
    matrixMode = mode;
}

GLAPI void APIENTRY glMultMatrixx(const GLfixed* m)
{
    switch (matrixMode)
    {
        case GL_PROJECTION:
        memcpy(projectionMatrix, m, sizeof(GLfixed) * 16);
        break;
        case GL_MODELVIEW:
        memcpy(modelViewMatrix, m, sizeof(GLfixed) * 16);
        break;
    default:
        notImplementedYet(__func__);
    }

}

GLAPI void APIENTRY glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
    normalsStride = stride;
    normalsType = type;
    normalsPointer = pointer;
}

GLAPI void APIENTRY glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPixelStorei(GLenum pname, GLint param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPointSizex(GLfixed size)
{
    pointSize = size;
    if (size <= 0)
    {
        currentError = GL_INVALID_VALUE;
    }
}

GLAPI void APIENTRY glPolygonOffsetx(GLfixed factor, GLfixed units)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPopMatrix(void)
{
    if (matrixStackTop <= 0)
    {
        currentError = GL_STACK_UNDERFLOW;
        return;
    }

    --matrixStackTop;

    memcpy(&projectionMatrix[0], &projectionMatrixStack[matrixStackTop][0], sizeof(GLfixed) * 16);
    memcpy(&modelViewMatrix[0], &modelViewMatrixStack[matrixStackTop][0], sizeof(GLfixed) * 16);

}

GLAPI void APIENTRY glPushMatrix(void)
{
    if (matrixStackTop >= (MATRIX_STACK_CAPACITY))
    {
        currentError = GL_STACK_OVERFLOW;
        return;
    }

    memcpy(&projectionMatrixStack[matrixStackTop][0], &projectionMatrix[0], sizeof(GLfixed) * 16);
    memcpy(&modelViewMatrixStack[matrixStackTop][0], &modelViewMatrix[0], sizeof(GLfixed) * 16);

    ++matrixStackTop;
}

GLAPI void APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                                 GLvoid* pixels)
{
    (void)format;
    (void)type;

    ///TODO: handle formats and types

    FramebufferPixelFormat* ptrStr;
    FramebufferPixelFormat* pixelsOut = pixels;

    for (int _y = 0; _y < height; ++_y)
    {
        ptrStr = &framebuffer[ XRES_FRAMEBUFFER * (y + _y) + x];
        for (int _x = 0; _x < width; ++_x)
        {
            *pixelsOut++ = *ptrStr++;
        }
    }
}

GLAPI void APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    GLfixed fx;
    GLfixed fy;
    GLfixed fz;

    /* Normalize axis */
    GLfixed lenfx = sqrt_fx16_16_to_fx16_16( Mul(x, x) + Mul(y, y) + Mul(z, z) );

    if (lenfx == 0)
    {
        return;
    }

    fx = Div(x, lenfx);
    fy = Div(y, lenfx);
    fz = Div(z, lenfx);

    GLfixed c = cosfpx(angle);
    GLfixed s = sinfpx(angle);
    GLfixed t = intToFix(1) - c;

    GLfixed R[16];

    /* First column */
    R[0] = Mul(Mul(t, fx), fx) + c;
    R[1] = Mul(Mul(t, fx), fy) + Mul(s, fz);
    R[2] = Mul(Mul(t, fx), fz) - Mul(s, fy);
    R[3] = 0;

    /* Second column */
    R[4] = Mul(Mul(t, fx), fy) - Mul(s, fz);
    R[5] = Mul(Mul(t, fy), fy) + c;
    R[6] = Mul(Mul(t, fy), fz) + Mul(s, fx);
    R[7] = 0;

    /* Third column */
    R[8] = Mul(Mul(t, fx), fz) + Mul(s, fy);
    R[9] = Mul(Mul(t, fy), fz) - Mul(s, fx);
    R[10] = Mul(Mul(t, fz), fz) + c;
    R[11] = 0;

    /* Fourth column */
    R[12] = 0;
    R[13] = 0;
    R[14] = 0;
    R[15] = intToFix(1);

    /* Post-multiply current matrix */
    GLfixed result[16];
    mat4x4_mul(R, modelViewMatrix, result);
    memcpy(modelViewMatrix, result, sizeof(result));
}

GLAPI void APIENTRY glSampleCoveragex(GLclampx value, GLboolean invert)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glScalex(GLfixed x, GLfixed y, GLfixed z)
{
    GLfixed mat[16];
    GLfixed tmp[16];
    mat4x4_transform(&mat[0], intToFix(1), intToFix(1), intToFix(1), x, y, z );
    mat4x4_mul(&modelViewMatrix[0], &mat[0], &tmp[0]);
    memcpy(&modelViewMatrix[0], &tmp[0], sizeof(GLfixed) * 16);
}

GLAPI void APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glShadeModel(GLenum mode)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glStencilMask(GLuint mask)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    textureCoordPointer = pointer;
    textureCoordSize = size;
    textureCoordStride = stride;
    textureCoordType = type;
}

GLAPI void APIENTRY glTexEnvx(GLenum target, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glTexEnvxv(GLenum target, GLenum pname, const GLfixed* params)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                                 GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
    textures[currentTexture].width = width;
    textures[currentTexture].height = height;
    textures[currentTexture].texels = malloc(sizeof(uint32_t) * width * height);
    memcpy(textures[currentTexture].texels, pixels, sizeof(uint32_t) * width * height);
}

GLAPI void APIENTRY glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                    GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
    GLfixed mat[16];
    GLfixed tmp[16];
    mat4x4_transform(&mat[0], x, y, z, intToFix(1), intToFix(1), intToFix(1));
    mat4x4_mul(&modelViewMatrix[0], &mat[0], &tmp[0]);
    memcpy(&modelViewMatrix[0], &tmp[0], sizeof(GLfixed) * 16);
}

GLAPI void APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    vertexStride = stride;
    vertexType = type;
    vertexSize = size;
    vertexPointer = pointer;

    if (size != 2 && size != 3 && size != 4)
    {
        currentError = GL_INVALID_VALUE;
    }

    if (stride < 0)
    {
        currentError = GL_INVALID_VALUE;
    }
    ///TODO: handle type errors
}

GLAPI void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{

    if (width < 0 || height < 0)
    {
        currentError = GL_INVALID_VALUE;
        return;
    }

    viewportX = x;
    viewportY = y;
    halfViewportWidthx = Div( intToFix(width), intToFix(2));
    halfViewportHeightx = Div( intToFix(height), intToFix(2));
}

GLfixed *currentModelViewMatrix(void)
{
    return &modelViewMatrix[0];
}
