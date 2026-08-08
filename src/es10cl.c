//
// Created by Daniel Monteiro on 27/02/2026.
//
#ifndef SGDK
#include <assert.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#else
typedef long long int64_t;
typedef unsigned long long uint64_t;
#endif
#include <string.h>
#include <GLES/gl.h>

#include "fpsqrt.h"
#include "internal.h"
#include "matricesFP.h"

const GLfixed sinfp[91] =
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
uint8_t smoothShadingModel = GL_FALSE;

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
GLfixed currentNormal[3];

GLfixed ambientColour[4];

GLfixed pointSize = intToFix(1);

#ifndef	DISABLE_DEPTH_BUFFER
uint8_t depthWritesEnabled = 1;
uint8_t depthTestEnabled = 0;
uint8_t clearDepth = 0xFF;
GLfixed zRange;
#endif

uint8_t clearColorR;
uint8_t clearColorG;
uint8_t clearColorB;
uint8_t clearColorA;

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
#ifndef SGDK
    puts("Not implemented yet");
    printf("Function called: %s\n", funcName);
    assert(0);
#endif
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
    clearDepth = 0xFF;
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
    } else {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
    }
}

GLAPI void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glClear(GLbitfield mask)
{
    if ((mask & ~(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT)) != 0) {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if ((mask & GL_COLOR_BUFFER_BIT) == GL_COLOR_BUFFER_BIT)
    {
        FramebufferPixelFormat fragment = MAKE_PIXEL(clearColorR, clearColorG, clearColorB, clearColorA);

        for (int y = 0; y < YRES_FRAMEBUFFER; y++)
        {
            FramebufferPixelFormat *ptr = SEEK(framebuffer, 0, y, FRAMEBUFFER_PITCH);

            for (int x = 0; x < XRES_FRAMEBUFFER ; ++x)
            {
                EMIT(ptr, x, y, fragment);
                ADVANCE(ptr, x, y);
            }
        }
    }

#ifndef DISABLE_DEPTH_BUFFER
    if ((mask & GL_DEPTH_BUFFER_BIT) == GL_DEPTH_BUFFER_BIT)
    {
        int c;
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            zBuffer[c] = clearDepth;
        }
    }
#endif

#ifndef DISABLE_STENCIL_BUFFER
    if ((mask & GL_STENCIL_BUFFER_BIT) == GL_STENCIL_BUFFER_BIT)
    {
        int c;
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            stencilBuffer[c] = clearStencil;
        }
    }
#endif
}

GLAPI void APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    clearColorR = fixToInt(Mul(intToFix(0xFF), red));
    clearColorG = fixToInt(Mul(intToFix(0xFF),green));
    clearColorB = fixToInt(Mul(intToFix(0xFF),blue));
    clearColorA = fixToInt(Mul(intToFix(0xFF),alpha));
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
    if (size != 2 && size != 3 && size != 4)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (stride < 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (type != GL_FIXED && size != GL_UNSIGNED_BYTE)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    colorStride = stride;
    colorType = type;
    colorSize = size;
    colorPointer = pointer;
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

    if (n < 0)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }
        return;
    }

    if (texturesIn == NULL)
    {
        //TODO: CRASH!
        return;
    }

    for (int c = 0; c < n; ++c)
    {
        GLuint index = *ptr++;

        /* per the standard, if the active texture is being deleted, the bound texture returns to texture 0 */
        if (index == currentTexture)
        {
            currentTexture = 0;
        }

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
    case GL_ALPHA_TEST:
    case GL_BLEND:
    case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL:
    case GL_FOG:
    case GL_LINE_SMOOTH:
    case GL_MULTISAMPLE:
    case GL_POINT_SMOOTH:
    case GL_POLYGON_OFFSET_FILL:
    case GL_RESCALE_NORMAL:
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
    case GL_SAMPLE_ALPHA_TO_ONE:
    case GL_SAMPLE_COVERAGE:
    case GL_SCISSOR_TEST:
    case GL_STENCIL_TEST:
        notImplementedYet(__func__);
        break;
    default:
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
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
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
    }
}

void processLine(GLfixed mv[16], GLfixed mvp[16], GLfixed* vertexPtr, GLfixed* cPtr, GLfixed vecs[8], GLfixed transformed[8])
{
    vecs[0] = *(vertexPtr + 0);
    vecs[1] = *(vertexPtr + 1);
    vecs[2] = *(vertexPtr + 2);
    vecs[3] = intToFix(1);

    vecs[4] = *(vertexPtr + 3);
    vecs[5] = *(vertexPtr + 4);
    vecs[6] = *(vertexPtr + 5);
    vecs[7] = intToFix(1);

    mat4x4_transformVec(&transformed[0], &mvp[0], &vecs[0]);
    mat4x4_transformVec(&transformed[4], &mvp[0], &vecs[4]);

    if ((transformed[3] == 0) || (transformed[7] == 0))
    {
        return;
    }

    GLfixed oneOverW0 = Div(intToFix(1), transformed[3]);
    GLfixed oneOverW1 = Div(intToFix(1), transformed[7]);

    GLfixed vertex[6] = {
        Mul(oneOverW0, transformed[0]), Mul(oneOverW0, transformed[1]),
        Mul(oneOverW1, transformed[4]), Mul(oneOverW1, transformed[5]),
    };

    GLfixed z0 = Mul(transformed[2], oneOverW0) + intToFix(1);
    GLfixed z1 = Mul(transformed[6], oneOverW1) + intToFix(1);

#ifndef	DISABLE_DEPTH_BUFFER
    uint8_t zValuesNormalized[2] ={
        fixToInt(Mul(z0, zRange)),
        fixToInt(Mul(z1, zRange))
    };
#endif

    int coords[4] = {
        viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[0])),
        viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[1])),
        viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[2])),
        viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[3]))
    };
    uint8_t coloursArray[8] = {

        fixToInt(Mul( *(cPtr +  0 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  1 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  2 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  3 ), intToFix(0xFF))),

        fixToInt(Mul( *(cPtr +  4 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  5 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  6 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  7 ), intToFix(0xFF)))
    };

    drawLine(coords[0], coords[1], coords[2], coords[3], &coloursArray[0], &zValuesNormalized[0]);
}

void processTriangle(GLfixed mv[16], GLfixed mvp[16], GLfixed* vertexPtr, int effeticeStride, GLfixed* uvPtr, GLfixed* cPtr, GLfixed* nPtr, struct Texture* texture, GLfixed vecs[12], GLfixed transformed[12], GLfixed transformedNormals[12])
{
    vecs[0] = *(vertexPtr + 0);
    vecs[1] = *(vertexPtr + 1);
    vecs[2] = *(vertexPtr + 2);
    vecs[3] = intToFix(1);

    vecs[4] = *(vertexPtr + effeticeStride);
    vecs[5] = *(vertexPtr + effeticeStride + 1);
    vecs[6] = *(vertexPtr + effeticeStride + 2);
    vecs[7] = intToFix(1);

    vecs[8] = *(vertexPtr + 2 * effeticeStride);
    vecs[9] = *(vertexPtr + 2 * effeticeStride + 1);
    vecs[10] = *(vertexPtr + 2 * effeticeStride + 2);
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

    mat4x4_transformVec(&transformedNormals[0], &mv[0], &vecs[0]);
    mat4x4_transformVec(&transformedNormals[4], &mv[0], &vecs[4]);
    mat4x4_transformVec(&transformedNormals[8], &mv[0], &vecs[8]);

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

                lightsDot[d * 3 + 0] = fixToInt(Mul(MAX(0, dot0), intToFix(255)));
                lightsDot[d * 3 + 1] = fixToInt(Mul(MAX(0, dot1), intToFix(255)));
                lightsDot[d * 3 + 2] = fixToInt(Mul(MAX(0, dot2), intToFix(255)));
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

    if ((transformed[3] == 0) || (transformed[7] == 0) || (transformed[11] == 0))
    {
        return;
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
    uint8_t zValuesNormalized[3] ={
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
}

void processPoints(GLfixed mvp[16], GLfixed* vertexPtr, GLfixed* cPtr, GLfixed vecs[4], GLfixed transformed[4])
{
    vecs[0] = *(vertexPtr + 0);
    vecs[1] = *(vertexPtr + 1);
    vecs[2] = *(vertexPtr + 2);
    vecs[3] = intToFix(1);

    mat4x4_transformVec(&transformed[0], &mvp[0], &vecs[0]);

    if (transformed[3] == 0)
    {
        return;
    }

    GLfixed oneOverW0 = Div(intToFix(1), transformed[3]);

    GLfixed vertex[2] = {
        Mul(oneOverW0, transformed[0]), Mul(oneOverW0, transformed[1])
    };

    GLfixed z0 = Mul(transformed[2], oneOverW0) + intToFix(1);

#ifndef	DISABLE_DEPTH_BUFFER
    uint8_t zValuesNormalized[1] ={
        fixToInt(Mul(z0, zRange))
    };
#endif

    int coords[2] = {
        viewportX + fixToInt(halfViewportWidthx + Mul( halfViewportWidthx, vertex[0])),
        viewportY + fixToInt(halfViewportHeightx - Mul( halfViewportHeightx, vertex[1]))
    };

    uint8_t coloursArray[4] = {
        fixToInt(Mul( *(cPtr +  0 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  1 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  2 ), intToFix(0xFF))),
        fixToInt(Mul( *(cPtr +  3 ), intToFix(0xFF)))
    };

    drawPoint(&coords[0], &coloursArray[0],
#ifndef	DISABLE_DEPTH_BUFFER
              zValuesNormalized[0],
#endif
              fixToInt(pointSize));
}

GLAPI void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    if (count < 0)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

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
                uvPtr += 2 * textureCoordSize;
                vertexPtr += 3 * vertexSize;
                cPtr += 4 * colorSize;
                nPtr += 9;
            }

            for (c = 0; c < (first % 3); ++c)
            {
                uvPtr += textureCoordSize;
                vertexPtr += vertexSize;
                cPtr += colorSize;
                nPtr += 3;
            }


            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[12];
                GLfixed transformed[12];
                GLfixed transformedNormals[12];

                processTriangle(modelViewMatrix, mvp, vertexPtr, vertexSize, uvPtr, cPtr, nPtr, texture, vecs, transformed, transformedNormals);

                vertexPtr += 3 * vertexSize;

                if (normalsArrayEnabled)
                {
                    nPtr += 9;
                }
                if (textureCoordsEnabled)
                {
                    uvPtr += 2 * textureCoordSize;
                }
                if (colorArrayEnabled)
                {
                    cPtr += 4 * colorSize;
                }
            }
        }
        break;
    case GL_POINTS:
        {
            int c;
            int finalCount = count;
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

            for (c = 0; c < first; ++c)
            {
                vertexPtr += vertexSize;
                cPtr += colorSize;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[4];
                GLfixed transformed[4];

                processPoints(mvp, vertexPtr, cPtr, vecs, transformed);
                vertexPtr += vertexSize;

                if (colorArrayEnabled)
                {
                    cPtr += colorSize;
                }
            }
        }
        break;
    case GL_TRIANGLE_STRIP:
        {
            int c;
            int finalCount = count - 2;
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


            for (c = 0; c < first; ++c)
            {
                uvPtr += textureCoordSize;
                vertexPtr += vertexSize;
                cPtr += colorSize;
                nPtr += 3;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[12];
                GLfixed transformed[12];
                GLfixed transformedNormals[12];

                processTriangle(modelViewMatrix, mvp, vertexPtr, vertexSize, uvPtr, cPtr, nPtr, texture, vecs, transformed, transformedNormals);

                vertexPtr += vertexSize;

                if (normalsArrayEnabled)
                {
                    nPtr += 3; /* temporary */
                }
                if (textureCoordsEnabled)
                {
                    uvPtr += textureCoordSize;
                }
                if (colorArrayEnabled)
                {
                    cPtr += colorSize;
                }
            }
        }
        break;
    case GL_TRIANGLE_FAN:
        {
            int c = 0;
            int finalCount = count - 1;
            struct Texture* texture;
            GLfixed triangleVerts[3 * 3];
            GLfixed triangleNormals[3 * 3];
            GLfixed triangleUv[3 * 2];
            GLfixed triangleColours[3 * 4];

            if (textureMapping2DEnabled)
            {
                texture = &textures[currentTexture];
            } else
            {
                texture = &dummyTexture;
            }

            triangleVerts[0] = ((GLfixed*)vertexPointer)[vertexSize * first];
            triangleVerts[1] = ((GLfixed*)vertexPointer)[vertexSize * first + 1];
            triangleVerts[2] = ((GLfixed*)vertexPointer)[vertexSize * first + 2];

            if (textureCoordsEnabled)
            {
                triangleUv[0] = ((GLfixed*)textureCoordPointer)[2 * first];
                triangleUv[1] = ((GLfixed*)textureCoordPointer)[2 * first + 1];
            } else
            {
                triangleUv[0] = ((GLfixed*)dummyTexCoords)[2 * first];
                triangleUv[1] = ((GLfixed*)dummyTexCoords)[2 * first + 1];
            }

            if (colorArrayEnabled)
            {
                triangleColours[0] = ((GLfixed*)colorPointer)[4 * first];
                triangleColours[1] = ((GLfixed*)colorPointer)[4 * first + 1];
                triangleColours[2] = ((GLfixed*)colorPointer)[4 * first + 2];
                triangleColours[3] = ((GLfixed*)colorPointer)[4 * first + 3];
            } else
            {
                triangleColours[0] = ((GLfixed*)dummyColors)[4 * first];
                triangleColours[1] = ((GLfixed*)dummyColors)[4 * first + 1];
                triangleColours[2] = ((GLfixed*)dummyColors)[4 * first + 2];
                triangleColours[3] = ((GLfixed*)dummyColors)[4 * first + 3];
            }

            if (normalsArrayEnabled)
            {
                triangleNormals[0] = ((GLfixed*)normalsPointer)[3 * first];
                triangleNormals[1] = ((GLfixed*)normalsPointer)[3 * first + 1];
                triangleNormals[2] = ((GLfixed*)normalsPointer)[3 * first + 2];
            } else
            {
                triangleNormals[0] = ((GLfixed*)dummyNormals)[3 * first];
                triangleNormals[1] = ((GLfixed*)dummyNormals)[3 * first + 1];
                triangleNormals[2] = ((GLfixed*)dummyNormals)[3 * first + 2];
            }

            for (c = (first + 1); c < finalCount; ++c)
            {
                GLfixed vecs[12];
                GLfixed transformed[12];
                GLfixed transformedNormals[12];

                triangleVerts[3] = ((GLfixed*)vertexPointer)[vertexSize * c];
                triangleVerts[4] = ((GLfixed*)vertexPointer)[vertexSize * c + 1];
                triangleVerts[5] = ((GLfixed*)vertexPointer)[vertexSize * c + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[2] = ((GLfixed*)textureCoordPointer)[2 * c];
                    triangleUv[3] = ((GLfixed*)textureCoordPointer)[2 * c + 1];
                } else
                {
                    triangleUv[2] = ((GLfixed*)dummyTexCoords)[2 * c];
                    triangleUv[3] = ((GLfixed*)dummyTexCoords)[2 * c + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[4] = ((GLfixed*)colorPointer)[4 * c];
                    triangleColours[5] = ((GLfixed*)colorPointer)[4 * c + 1];
                    triangleColours[6] = ((GLfixed*)colorPointer)[4 * c + 2];
                    triangleColours[7] = ((GLfixed*)colorPointer)[4 * c + 3];
                } else
                {
                    triangleColours[4] = ((GLfixed*)dummyColors)[4 * c];
                    triangleColours[5] = ((GLfixed*)dummyColors)[4 * c + 1];
                    triangleColours[6] = ((GLfixed*)dummyColors)[4 * c + 2];
                    triangleColours[7] = ((GLfixed*)dummyColors)[4 * c + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[3] = ((GLfixed*)normalsPointer)[3 * c];
                    triangleNormals[4] = ((GLfixed*)normalsPointer)[3 * c + 1];
                    triangleNormals[5] = ((GLfixed*)normalsPointer)[3 * c + 2];
                } else
                {
                    triangleNormals[3] = ((GLfixed*)dummyNormals)[3 * c];
                    triangleNormals[4] = ((GLfixed*)dummyNormals)[3 * c + 1];
                    triangleNormals[5] = ((GLfixed*)dummyNormals)[3 * c + 2];
                }

                int d = c + 1;
                triangleVerts[6] = ((GLfixed*)vertexPointer)[vertexSize * d];
                triangleVerts[7] = ((GLfixed*)vertexPointer)[vertexSize * d + 1];
                triangleVerts[8] = ((GLfixed*)vertexPointer)[vertexSize * d + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[4] = ((GLfixed*)textureCoordPointer)[2 * d];
                    triangleUv[5] = ((GLfixed*)textureCoordPointer)[2 * d + 1];
                } else
                {
                    triangleUv[4] = ((GLfixed*)dummyTexCoords)[2 * d];
                    triangleUv[5] = ((GLfixed*)dummyTexCoords)[2 * d + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[8] = ((GLfixed*)colorPointer)[4 * d];
                    triangleColours[9] = ((GLfixed*)colorPointer)[4 * d + 1];
                    triangleColours[10] = ((GLfixed*)colorPointer)[4 * d + 2];
                    triangleColours[11] = ((GLfixed*)colorPointer)[4 * d + 3];
                } else
                {
                    triangleColours[8] = ((GLfixed*)dummyColors)[4 * d];
                    triangleColours[9] = ((GLfixed*)dummyColors)[4 * d + 1];
                    triangleColours[10] = ((GLfixed*)dummyColors)[4 * d + 2];
                    triangleColours[11] = ((GLfixed*)dummyColors)[4 * d + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[6] = ((GLfixed*)normalsPointer)[3 * d];
                    triangleNormals[7] = ((GLfixed*)normalsPointer)[3 * d + 1];
                    triangleNormals[8] = ((GLfixed*)normalsPointer)[3 * d + 2];
                } else
                {
                    triangleNormals[6] = ((GLfixed*)dummyNormals)[3 * d];
                    triangleNormals[7] = ((GLfixed*)dummyNormals)[3 * d + 1];
                    triangleNormals[8] = ((GLfixed*)dummyNormals)[3 * d + 2];
                }

                processTriangle(modelViewMatrix, mvp, triangleVerts, 3, triangleUv, triangleColours, triangleNormals, texture, vecs, transformed, transformedNormals);
            }
        }
        break;
    case GL_LINES:
        {
            int c;
            int finalCount = count / 2;
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

            for (c = 0; c < first; ++c)
            {
                vertexPtr += vertexSize;
                cPtr += colorSize;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[8];
                GLfixed transformed[8];

                processLine(modelViewMatrix, mvp, vertexPtr, cPtr, vecs, transformed);

                vertexPtr += 2 * vertexSize;

                if (colorArrayEnabled)
                {
                    cPtr += 2 * colorSize;
                }
            }
        }
        break;
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
        {
            int c;
            int finalCount = count - 1;
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

            for (c = 0; c < first; ++c)
            {
                vertexPtr += vertexSize;
                cPtr += colorSize;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[8];
                GLfixed transformed[8];

                processLine(modelViewMatrix, mvp, vertexPtr, cPtr, vecs, transformed);

                vertexPtr += vertexSize;

                if (colorArrayEnabled)
                {
                    cPtr += colorSize;
                }
            }
            if (mode != GL_LINE_STRIP) {
                GLfixed *verts = alloca(2 * vertexSize * sizeof(GLfixed));
                GLfixed *colours = alloca(2 * colorSize * sizeof(GLfixed));
                GLfixed vecs[8];
                GLfixed transformed[8];

                vertexPtr -= vertexSize;

                if (colorArrayEnabled)
                {
                    cPtr -= colorSize;
                }

                for (c = 0; c < vertexSize; ++c)
                {
                    verts[c] = ((GLfixed*)vertexPointer)[c];
                    if (colorArrayEnabled)
                    {
                        colours[c] = ((GLfixed*)colorPointer)[c];
                    } else
                    {
                        colours[c] = dummyColors[0][0];
                    }


                    verts[vertexSize + c] = ((GLfixed*)vertexPointer)[(vertexSize * (count - 1)) + c];
                    if (colorArrayEnabled)
                    {
                        colours[colorSize + c] = ((GLfixed*)colorPointer)[(colorSize * (count - 1)) + c];
                    } else
                    {
                        colours[colorSize + c] = dummyColors[0][0];
                    }
                }
                processLine(modelViewMatrix, mvp, verts, colours, vecs, transformed);
            }
        }
        break;
    default:
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }
    }
}

GLAPI void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    if (count < 0)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

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
    case GL_TRIANGLE_FAN:
        {
            int c;
            int finalCount = count - 1;
            struct Texture* texture;
            GLfixed triangleVerts[3 * 3];
            GLfixed triangleNormals[3 * 3];
            GLfixed triangleUv[3 * 2];
            GLfixed triangleColours[3 * 4];

            int index;

            if (textureMapping2DEnabled)
            {
                texture = &textures[currentTexture];
            } else
            {
                texture = &dummyTexture;
            }

            if (type == GL_UNSIGNED_SHORT) {
                index = ((uint16_t*)indices)[0];
            } else {
                index = ((uint8_t*)indices)[0];
            }

            triangleVerts[0] = ((GLfixed*)vertexPointer)[vertexSize * index];
            triangleVerts[1] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
            triangleVerts[2] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

            if (textureCoordsEnabled)
            {
                triangleUv[0] = ((GLfixed*)textureCoordPointer)[2 * index];
                triangleUv[1] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
            } else
            {
                triangleUv[0] = ((GLfixed*)dummyTexCoords)[0];
                triangleUv[1] = ((GLfixed*)dummyTexCoords)[1];
            }

            if (colorArrayEnabled)
            {
                triangleColours[0] = ((GLfixed*)colorPointer)[4 * index];
                triangleColours[1] = ((GLfixed*)colorPointer)[4 * index + 1];
                triangleColours[2] = ((GLfixed*)colorPointer)[4 * index + 2];
                triangleColours[3] = ((GLfixed*)colorPointer)[4 * index + 3];
            } else
            {
                triangleColours[0] = ((GLfixed*)dummyColors)[0];
                triangleColours[1] = ((GLfixed*)dummyColors)[1];
                triangleColours[2] = ((GLfixed*)dummyColors)[2];
                triangleColours[3] = ((GLfixed*)dummyColors)[3];
            }

            if (normalsArrayEnabled)
            {
                triangleNormals[0] = ((GLfixed*)normalsPointer)[3 * index];
                triangleNormals[1] = ((GLfixed*)normalsPointer)[3 * index + 1];
                triangleNormals[2] = ((GLfixed*)normalsPointer)[3 * index + 2];
            } else
            {
                triangleNormals[0] = ((GLfixed*)dummyNormals)[0];
                triangleNormals[1] = ((GLfixed*)dummyNormals)[1];
                triangleNormals[2] = ((GLfixed*)dummyNormals)[2];
            }

            for (c = 1; c < finalCount; ++c)
            {
                GLfixed vecs[12];
                GLfixed transformed[12];
                GLfixed transformedNormals[12];


                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c];
                } else {
                    index = ((uint8_t*)indices)[c];
                }


                triangleVerts[3] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[4] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[5] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[2] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[3] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[2] = ((GLfixed*)dummyTexCoords)[0];
                    triangleUv[3] = ((GLfixed*)dummyTexCoords)[1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[4] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[5] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[6] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[7] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[4] = ((GLfixed*)dummyColors)[0];
                    triangleColours[5] = ((GLfixed*)dummyColors)[1];
                    triangleColours[6] = ((GLfixed*)dummyColors)[2];
                    triangleColours[7] = ((GLfixed*)dummyColors)[3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[3] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[4] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[5] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[3] = ((GLfixed*)dummyNormals)[0];
                    triangleNormals[4] = ((GLfixed*)dummyNormals)[1];
                    triangleNormals[5] = ((GLfixed*)dummyNormals)[2];
                }

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 1];
                } else {
                    index = ((uint8_t*)indices)[c + 1];
                }

                triangleVerts[6] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[7] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[8] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[4] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[5] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[4] = ((GLfixed*)dummyTexCoords)[0];
                    triangleUv[5] = ((GLfixed*)dummyTexCoords)[1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[8] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[9] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[10] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[11] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[8] = ((GLfixed*)dummyColors)[0];
                    triangleColours[9] = ((GLfixed*)dummyColors)[1];
                    triangleColours[10] = ((GLfixed*)dummyColors)[2];
                    triangleColours[11] = ((GLfixed*)dummyColors)[3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[6] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[7] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[8] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[6] = ((GLfixed*)dummyNormals)[0];
                    triangleNormals[7] = ((GLfixed*)dummyNormals)[1];
                    triangleNormals[8] = ((GLfixed*)dummyNormals)[2];
                }

                processTriangle(modelViewMatrix, mvp, triangleVerts, 3, triangleUv, triangleColours, triangleNormals, texture, vecs, transformed, transformedNormals);
            }
        }
        break;
    case GL_TRIANGLES:
        {
            int c;
            int finalCount = count;
            struct Texture* texture;
            GLfixed triangleVerts[3 * 3];
            GLfixed triangleNormals[3 * 3];
            GLfixed triangleUv[3 * 2];
            GLfixed triangleColours[3 * 4];

            int index;

            if (textureMapping2DEnabled)
            {
                texture = &textures[currentTexture];
            } else
            {
                texture = &dummyTexture;
            }

            for (c = 0; c < finalCount; c += 3)
            {
                GLfixed vecs[16];
                GLfixed transformed[16];
                GLfixed transformedNormals[16];


                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c];
                } else {
                    index = ((uint8_t*)indices)[c];
                }

                triangleVerts[0] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[1] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[2] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[0] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[1] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[0] = ((GLfixed*)dummyTexCoords)[2 * index];
                    triangleUv[1] = ((GLfixed*)dummyTexCoords)[2 * index + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[0] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[1] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[2] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[3] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[0] = ((GLfixed*)dummyColors)[4 * index];
                    triangleColours[1] = ((GLfixed*)dummyColors)[4 * index + 1];
                    triangleColours[2] = ((GLfixed*)dummyColors)[4 * index + 2];
                    triangleColours[3] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[0] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[1] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[2] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[0] = ((GLfixed*)dummyNormals)[3 * index];
                    triangleNormals[1] = ((GLfixed*)dummyNormals)[3 * index + 1];
                    triangleNormals[2] = ((GLfixed*)dummyNormals)[3 * index + 2];
                }


                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 1];
                } else {
                    index = ((uint8_t*)indices)[c + 1];
                }


                triangleVerts[3] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[4] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[5] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[2] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[3] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[2] = ((GLfixed*)dummyTexCoords)[2 * index];
                    triangleUv[3] = ((GLfixed*)dummyTexCoords)[2 * index + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[4] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[5] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[6] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[7] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[4] = ((GLfixed*)dummyColors)[4 * index];
                    triangleColours[5] = ((GLfixed*)dummyColors)[4 * index + 1];
                    triangleColours[6] = ((GLfixed*)dummyColors)[4 * index + 2];
                    triangleColours[7] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[3] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[4] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[5] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[3] = ((GLfixed*)dummyNormals)[3 * index];
                    triangleNormals[4] = ((GLfixed*)dummyNormals)[3 * index + 1];
                    triangleNormals[5] = ((GLfixed*)dummyNormals)[3 * index + 2];
                }

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 2];
                } else {
                    index = ((uint8_t*)indices)[c + 2];
                }

                triangleVerts[6] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[7] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[8] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[4] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[5] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[4] = ((GLfixed*)dummyTexCoords)[2 * index];
                    triangleUv[5] = ((GLfixed*)dummyTexCoords)[2 * index + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[8] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[9] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[10] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[11] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[8] = ((GLfixed*)dummyColors)[4 * index];
                    triangleColours[9] = ((GLfixed*)dummyColors)[4 * index + 1];
                    triangleColours[10] = ((GLfixed*)dummyColors)[4 * index + 2];
                    triangleColours[11] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[6] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[7] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[8] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[6] = ((GLfixed*)dummyNormals)[3 * index];
                    triangleNormals[7] = ((GLfixed*)dummyNormals)[3 * index + 1];
                    triangleNormals[8] = ((GLfixed*)dummyNormals)[3 * index + 2];
                }

                processTriangle(modelViewMatrix, mvp, triangleVerts, 3, triangleUv, triangleColours, triangleNormals, texture, vecs, transformed, transformedNormals);
            }
        }
        break;
    case GL_POINTS:
        {
            int c;
            int index;
            int finalCount = count;

            GLfixed* cPtr = (GLfixed*)&dummyColors[0];

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[16];
                GLfixed transformed[16];

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c];
                } else {
                    index = ((uint8_t*)indices)[c];
                }

                GLfixed* vertexPtr = (GLfixed*)vertexPointer + (vertexSize * index);

                if (colorArrayEnabled)
                {
                    cPtr = (GLfixed*)colorPointer + ( 2 * index);
                }

                processPoints(mvp, vertexPtr, cPtr, vecs, transformed);
            }
        }
        break;
    case GL_TRIANGLE_STRIP:
        {
            int c;
            int index;
            int finalCount = count - 2;
            GLfixed triangleVerts[3 * 3];
            GLfixed triangleNormals[3 * 3];
            GLfixed triangleUv[3 * 2];
            GLfixed triangleColours[3 * 4];
            struct Texture* texture;

            if (textureMapping2DEnabled)
            {
                texture = &textures[currentTexture];
            } else
            {
                texture = &dummyTexture;
            }

            for (c = 0; c < finalCount; ++c)
            {
                GLfixed vecs[12];
                GLfixed transformed[12];
                GLfixed transformedNormals[12];

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c];
                } else {
                    index = ((uint8_t*)indices)[c];
                }

                triangleVerts[0] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[1] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[2] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[0] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[1] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[0] = ((GLfixed*)dummyTexCoords)[2 * index];
                    triangleUv[1] = ((GLfixed*)dummyTexCoords)[2 * index + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[0] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[1] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[2] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[3] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[0] = ((GLfixed*)dummyColors)[4 * index];
                    triangleColours[1] = ((GLfixed*)dummyColors)[4 * index + 1];
                    triangleColours[2] = ((GLfixed*)dummyColors)[4 * index + 2];
                    triangleColours[3] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[0] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[1] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[2] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[0] = ((GLfixed*)dummyNormals)[3 * index];
                    triangleNormals[1] = ((GLfixed*)dummyNormals)[3 * index + 1];
                    triangleNormals[2] = ((GLfixed*)dummyNormals)[3 * index + 2];
                }

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 1];
                } else {
                    index = ((uint8_t*)indices)[c + 1];
                }

                triangleVerts[3] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[4] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[5] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[2] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[3] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[2] = ((GLfixed*)dummyTexCoords)[2 * index];
                    triangleUv[3] = ((GLfixed*)dummyTexCoords)[2 * index + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[4] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[5] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[6] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[7] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[4] = ((GLfixed*)dummyColors)[4 * index];
                    triangleColours[5] = ((GLfixed*)dummyColors)[4 * index + 1];
                    triangleColours[6] = ((GLfixed*)dummyColors)[4 * index + 2];
                    triangleColours[7] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[3] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[4] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[5] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[3] = ((GLfixed*)dummyNormals)[3 * index];
                    triangleNormals[4] = ((GLfixed*)dummyNormals)[3 * index + 1];
                    triangleNormals[5] = ((GLfixed*)dummyNormals)[3 * index + 2];
                }

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 2];
                } else {
                    index = ((uint8_t*)indices)[c + 2];
                }

                triangleVerts[6] = ((GLfixed*)vertexPointer)[vertexSize * index];
                triangleVerts[7] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                triangleVerts[8] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (textureCoordsEnabled)
                {
                    triangleUv[4] = ((GLfixed*)textureCoordPointer)[2 * index];
                    triangleUv[5] = ((GLfixed*)textureCoordPointer)[2 * index + 1];
                } else
                {
                    triangleUv[4] = ((GLfixed*)dummyTexCoords)[2 * index];
                    triangleUv[5] = ((GLfixed*)dummyTexCoords)[2 * index + 1];
                }

                if (colorArrayEnabled)
                {
                    triangleColours[8] = ((GLfixed*)colorPointer)[4 * index];
                    triangleColours[9] = ((GLfixed*)colorPointer)[4 * index + 1];
                    triangleColours[10] = ((GLfixed*)colorPointer)[4 * index + 2];
                    triangleColours[11] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    triangleColours[8] = ((GLfixed*)dummyColors)[4 * index];
                    triangleColours[9] = ((GLfixed*)dummyColors)[4 * index + 1];
                    triangleColours[10] = ((GLfixed*)dummyColors)[4 * index + 2];
                    triangleColours[11] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (normalsArrayEnabled)
                {
                    triangleNormals[6] = ((GLfixed*)normalsPointer)[3 * index];
                    triangleNormals[7] = ((GLfixed*)normalsPointer)[3 * index + 1];
                    triangleNormals[8] = ((GLfixed*)normalsPointer)[3 * index + 2];
                } else
                {
                    triangleNormals[6] = ((GLfixed*)dummyNormals)[3 * index];
                    triangleNormals[7] = ((GLfixed*)dummyNormals)[3 * index + 1];
                    triangleNormals[8] = ((GLfixed*)dummyNormals)[3 * index + 2];
                }

                processTriangle(modelViewMatrix, mvp, triangleVerts, 3, triangleUv, triangleColours, triangleNormals, texture, vecs, transformed, transformedNormals);
            }
        }
        break;
    case GL_LINES:
        {
            int c;
            int index;
            int finalCount = count;
            GLfixed verts[2 * 3];
            GLfixed colours[2 * 4];


            for (c = 0; c < finalCount; c += 2)
            {
                GLfixed vecs[8];
                GLfixed transformed[8];

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c];
                } else {
                    index = ((uint8_t*)indices)[c];
                }

                verts[0] = ((GLfixed*)vertexPointer)[vertexSize * index];
                verts[1] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                verts[2] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (colorArrayEnabled)
                {
                    colours[0] = ((GLfixed*)colorPointer)[4 * index];
                    colours[1] = ((GLfixed*)colorPointer)[4 * index + 1];
                    colours[2] = ((GLfixed*)colorPointer)[4 * index + 2];
                    colours[3] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    colours[0] = ((GLfixed*)dummyColors)[4 * index];
                    colours[1] = ((GLfixed*)dummyColors)[4 * index + 1];
                    colours[2] = ((GLfixed*)dummyColors)[4 * index + 2];
                    colours[3] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 1];
                } else {
                    index = ((uint8_t*)indices)[c + 1];
                }

                verts[3] = ((GLfixed*)vertexPointer)[3 * index];
                verts[4] = ((GLfixed*)vertexPointer)[3 * index + 1];
                verts[5] = ((GLfixed*)vertexPointer)[3 * index + 2];

                if (colorArrayEnabled)
                {
                    colours[4] = ((GLfixed*)colorPointer)[4 * index];
                    colours[5] = ((GLfixed*)colorPointer)[4 * index + 1];
                    colours[6] = ((GLfixed*)colorPointer)[4 * index + 2];
                    colours[7] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    colours[4] = ((GLfixed*)dummyColors)[4 * index];
                    colours[5] = ((GLfixed*)dummyColors)[4 * index + 1];
                    colours[6] = ((GLfixed*)dummyColors)[4 * index + 2];
                    colours[7] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                processLine(modelViewMatrix, mvp, verts, colours, vecs, transformed);
            }
        }
        break;
    case GL_LINE_STRIP:
    case GL_LINE_LOOP:
                {
            int c;
            int index;
            int finalCount = count - 1;
            GLfixed verts[2 * 3];
            GLfixed colours[2 * 4];
            GLfixed vecs[8];
            GLfixed transformed[8];


            for (c = 0; c < finalCount; ++c)
            {

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c];
                } else {
                    index = ((uint8_t*)indices)[c];
                }

                verts[0] = ((GLfixed*)vertexPointer)[vertexSize * index];
                verts[1] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                verts[2] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (colorArrayEnabled)
                {
                    colours[0] = ((GLfixed*)colorPointer)[4 * index];
                    colours[1] = ((GLfixed*)colorPointer)[4 * index + 1];
                    colours[2] = ((GLfixed*)colorPointer)[4 * index + 2];
                    colours[3] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    colours[0] = ((GLfixed*)dummyColors)[4 * index];
                    colours[1] = ((GLfixed*)dummyColors)[4 * index + 1];
                    colours[2] = ((GLfixed*)dummyColors)[4 * index + 2];
                    colours[3] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[c + 1];
                } else {
                    index = ((uint8_t*)indices)[c + 1];
                }

                verts[3] = ((GLfixed*)vertexPointer)[vertexSize * index];
                verts[4] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                verts[5] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                if (colorArrayEnabled)
                {
                    colours[4] = ((GLfixed*)colorPointer)[4 * index];
                    colours[5] = ((GLfixed*)colorPointer)[4 * index + 1];
                    colours[6] = ((GLfixed*)colorPointer)[4 * index + 2];
                    colours[7] = ((GLfixed*)colorPointer)[4 * index + 3];
                } else
                {
                    colours[4] = ((GLfixed*)dummyColors)[4 * index];
                    colours[5] = ((GLfixed*)dummyColors)[4 * index + 1];
                    colours[6] = ((GLfixed*)dummyColors)[4 * index + 2];
                    colours[7] = ((GLfixed*)dummyColors)[4 * index + 3];
                }

                processLine(modelViewMatrix, mvp, verts, colours, vecs, transformed);
            }
            if (mode != GL_LINE_STRIP)
            {
                if (type == GL_UNSIGNED_SHORT) {
                    index = ((uint16_t*)indices)[0];
                } else {
                    index = ((uint8_t*)indices)[0];
                }

                verts[0] = ((GLfixed*)vertexPointer)[vertexSize * index];
                verts[1] = ((GLfixed*)vertexPointer)[vertexSize * index + 1];
                verts[2] = ((GLfixed*)vertexPointer)[vertexSize * index + 2];

                processLine(modelViewMatrix, mvp, verts, colours, vecs, transformed);
            }
        }
        break;
    default:
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }
    }
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
    case GL_ALPHA_TEST:
    case GL_BLEND:
    case GL_COLOR_LOGIC_OP:
    case GL_COLOR_MATERIAL:
    case GL_FOG:
    case GL_LINE_SMOOTH:
    case GL_MULTISAMPLE:
    case GL_POINT_SMOOTH:
    case GL_POLYGON_OFFSET_FILL:
    case GL_RESCALE_NORMAL:
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
    case GL_SAMPLE_ALPHA_TO_ONE:
    case GL_SAMPLE_COVERAGE:
    case GL_SCISSOR_TEST:
    case GL_STENCIL_TEST:
        notImplementedYet(__func__);
        break;
    default:
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
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
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
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
    if ((zNear < 0) || (zFar < 0) || (left == right) || (bottom == top) || (zNear == zFar))
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

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
    if (n < 0)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (texturesOut == NULL)
    {
        //TODO: CRASH!
        return;
    }

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
    case GL_LIGHT_MODEL_TWO_SIDE:
        notImplementedYet(__func__);
        break;
    default:
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
    }
}

GLAPI void APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed* params)
{
    //TODO: GL_INVALID_VALUE is generated if a spot exponent value is specified outside the range [0, 128], or if spot
    // cutoff is specified outside the range [0, 90] (except for the special value 180), or if a negative attenuation
    // factor is specified.

    switch (pname)
    {
    case GL_POSITION:
        memcpy(&lights[light - GL_LIGHT0].position[0], params, sizeof(GLfixed) * 4);
        break;
    case GL_AMBIENT:
    case GL_DIFFUSE:
    case GL_SPECULAR:
    case GL_SPOT_DIRECTION:
    case GL_SPOT_EXPONENT:
    case GL_SPOT_CUTOFF:
    case GL_CONSTANT_ATTENUATION:
    case GL_LINEAR_ATTENUATION:
    case GL_QUADRATIC_ATTENUATION:
        notImplementedYet(__func__);
        break;
    default:
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
    }
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
        notImplementedYet(__func__);
        break;
#ifndef SGDK
    default:
        assert(0);
#endif
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

    if (face != GL_FRONT_AND_BACK)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    if (pname != GL_AMBIENT && pname != GL_DIFFUSE && pname != GL_SPECULAR && pname != GL_EMISSION && pname != GL_SHININESS && pname != GL_AMBIENT_AND_DIFFUSE)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }
    ///TODO: implement this properly
}

GLAPI void APIENTRY glMatrixMode(GLenum mode)
{
    switch (mode)
    {
    case GL_PROJECTION:
    case GL_MODELVIEW:
    case GL_TEXTURE:
        matrixMode = mode;
        return;
    default:
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }
    }
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
    case GL_TEXTURE:
        notImplementedYet(__func__);
        break;
#ifndef SGDK
    default:
         assert(0);
#endif
    }
}

GLAPI void APIENTRY glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
    currentNormal[0] = nx;
    currentNormal[1] = ny;
    currentNormal[2] = nz;
}

GLAPI void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
    if (type != GL_SHORT && type != GL_BYTE && type != GL_FIXED)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    if (stride < 0)
    {
        if (currentError == GL_NO_ERROR) {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

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
    if (size <= 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    pointSize = size;
}

GLAPI void APIENTRY glPolygonOffsetx(GLfixed factor, GLfixed units)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPopMatrix(void)
{
    if (matrixStackTop <= 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_STACK_UNDERFLOW;
        }

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
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_STACK_OVERFLOW;
        }
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

    if (format != GL_RGBA)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }
        return;
    }

    if (type != GL_UNSIGNED_BYTE)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }
        return;
    }

    if (width < 0 || height < 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }
        return;
    }

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
    if (mode != GL_FLAT && mode != GL_SMOOTH)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }
        return;
    }

    smoothShadingModel = (mode == GL_SMOOTH);
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
    if (size != 2 && size != 3 && size != 4)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }
        return;
    }

    if (type != GL_BYTE && type != GL_SHORT && type != GL_FIXED)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }
        return;
    }

    if (stride < 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }
        return;
    }

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
    if (target != GL_TEXTURE_2D)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    if (format != GL_ALPHA && format != GL_RGB && format != GL_RGBA && format != GL_LUMINANCE && format != GL_LUMINANCE_ALPHA)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    if (type !=  GL_UNSIGNED_BYTE && type !=  GL_UNSIGNED_SHORT_5_6_5 && type !=  GL_UNSIGNED_SHORT_4_4_4_4 && type !=  GL_UNSIGNED_SHORT_5_5_5_1)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    if (level < 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (level > MAX_TEXTURE_SIZE_LOG2)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (internalformat != GL_ALPHA && internalformat != GL_RGB && internalformat != GL_RGBA && internalformat != GL_LUMINANCE && internalformat != GL_LUMINANCE_ALPHA)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (width < 0 || height < 0 || width > MAX_TEXTURE_SIZE || height > MAX_TEXTURE_SIZE || !POT(width) || !POT(height))
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (border != 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (format != internalformat)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_OPERATION;
        }

        return;
    }

    if (type == GL_UNSIGNED_SHORT_5_6_5 && format != GL_RGB)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_OPERATION;
        }

        return;
    }

    if ((type ==  GL_UNSIGNED_SHORT_4_4_4_4 || type ==  GL_UNSIGNED_SHORT_5_5_5_1) && format !=  GL_RGBA)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_OPERATION;
        }

        return;
    }

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
    if (size != 2 && size != 3 && size != 4)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (stride < 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }

        return;
    }

    if (type != GL_BYTE && type != GL_SHORT && type != GL_FIXED)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_ENUM;
        }

        return;
    }

    vertexStride = stride;
    if (stride == 0)
    {
        vertexSize = size;
    } else
    {
        vertexSize = stride / sizeof(GLfixed);
    }

    vertexType = type;
    vertexPointer = pointer;
}

GLAPI void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{

    if (width < 0 || height < 0)
    {
        if (currentError == GL_NO_ERROR)
        {
            currentError = GL_INVALID_VALUE;
        }
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
