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

#include "internal.h"
#include "matricesFP.h"

typedef uint32_t FramebufferPixelFormat;

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)


#define floatToFix(f) ((GLfixed)(65536.0f * (f)))

#define MATRIX_STACK_CAPACITY 16

#define MIN(v1, v2) (( (v1) < (v2) ) ? (v1) : (v2) )
#define MAX(v1, v2) (( (v1) > (v2) ) ? (v1) : (v2) )

#define YRES_FRAMEBUFFER 300
#define XRES_FRAMEBUFFER 300

GLenum currentError = GL_NO_ERROR;

uint32_t clearColor;

uint8_t matrixStackTop = 0;

GLsizei vertexStride = 0;
GLenum vertexType = 0;
GLint vertexSize = 0;
const GLvoid* vertexPointer = NULL;
uint8_t vertexArrayEnabled = GL_FALSE;

uint16_t viewportX;
uint16_t viewportY;
uint16_t viewportWidth;
uint16_t viewportHeight;

GLenum matrixMode;

GLfixed projectionMatrix[16];
GLfixed modelViewMatrix[16];

GLfixed projectionMatrixStack[16][16];
GLfixed modelViewMatrixStack[16][16];

GLsizei colorStride = 0;
GLenum colorType = 0;
GLint colorSize = 0;
const GLvoid* colorPointer = NULL;
uint8_t colorArrayEnabled = GL_TRUE;

GLfixed pointSize = intToFix(1);

uint8_t clearDepth = 0;
uint8_t clearStencil = 0;

extern uint32_t framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

void fillTriangle(int *coords, FramebufferPixelFormat *colour);

static void notImplementedYet(char *funcName)
{
    puts("Not implemented yet");
    printf("Function called: %s\n", funcName);
    assert(0);
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
    notImplementedYet(__func__);
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

    if ((mask &  GL_DEPTH_BUFFER_BIT ) ==  GL_DEPTH_BUFFER_BIT )
    {
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            zBuffer[c] = clearDepth;
        }
    }

    if ((mask & GL_STENCIL_BUFFER_BIT) == GL_STENCIL_BUFFER_BIT)
    {
        for (c = 0; c < (XRES_FRAMEBUFFER * YRES_FRAMEBUFFER); ++c)
        {
            stencilBuffer[c] = clearStencil;
        }
    }
    ///TODO: check for error conditions
}

GLAPI void APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    clearColor = red << 24 | green << 16 | blue << 8 | alpha;
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

    if (size != 2 && size != 3 && size != 4) {
        currentError = GL_INVALID_VALUE;
    }

    if (stride < 0 ) {
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

GLAPI void APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDepthFunc(GLenum func)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDepthMask(GLboolean flag)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDepthRangex(GLclampx zNear, GLclampx zFar)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDisable(GLenum cap)
{
    notImplementedYet(__func__);
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
    }
    ///TODO: handle other client states
}

GLAPI void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    ///TODO: better place the mvp matrix computation
    GLfixed mvp[16];
    mat4x4_mul(&modelViewMatrix[0], &projectionMatrix[0], &mvp[0]);

    switch (mode)
    {
    case GL_TRIANGLES:
        {
            int c;
            int finalCount = count / 3;
            for (c = first; c < finalCount; ++c)
            {
                GLfixed vecs[16];
                GLfixed transformed[16];

                vecs[0] = *((GLfixed*)vertexPointer + 0);
                vecs[1] = *((GLfixed*)vertexPointer + 1);
                vecs[2] = *((GLfixed*)vertexPointer + 2);
                vecs[3] = intToFix(1);

                vecs[4] = *((GLfixed*)vertexPointer + 3);
                vecs[5] = *((GLfixed*)vertexPointer + 4);
                vecs[6] = *((GLfixed*)vertexPointer + 5);
                vecs[7] = intToFix(1);

                vecs[8] = *((GLfixed*)vertexPointer + 6);
                vecs[9] = *((GLfixed*)vertexPointer + 7);
                vecs[10] = *((GLfixed*)vertexPointer + 8);
                vecs[11] = intToFix(1);

                mat4x4_transformVec(&transformed[0], &mvp[0], &vecs[0]);
                mat4x4_transformVec(&transformed[4], &mvp[0], &vecs[4]);
                mat4x4_transformVec(&transformed[8], &mvp[0], &vecs[8]);

                GLfixed oneOverW0 = Div(intToFix(1), transformed[3]);
                GLfixed oneOverW1 = Div(intToFix(1), transformed[7]);
                GLfixed oneOverW2 = Div(intToFix(1), transformed[11]);

                GLfixed vertex[6] = {
                    Mul(oneOverW0, transformed[0]), Mul(oneOverW0, transformed[1]),
                    Mul(oneOverW1, transformed[4]), Mul(oneOverW1, transformed[5]),
                    Mul(oneOverW2, transformed[8]), Mul(oneOverW2, transformed[9]),
                };

                int coords[6] = {
                    (XRES_FRAMEBUFFER / 2) + fixToInt(Mul( intToFix(XRES_FRAMEBUFFER / 2), vertex[0])),
                    (YRES_FRAMEBUFFER / 2) - fixToInt(Mul( intToFix(YRES_FRAMEBUFFER / 2), vertex[1])),
                    (XRES_FRAMEBUFFER / 2) + fixToInt(Mul( intToFix(XRES_FRAMEBUFFER / 2), vertex[2])),
                    (YRES_FRAMEBUFFER / 2) - fixToInt(Mul( intToFix(YRES_FRAMEBUFFER / 2), vertex[3])),
                    (XRES_FRAMEBUFFER / 2) + fixToInt(Mul( intToFix(XRES_FRAMEBUFFER / 2), vertex[4])),
                    (YRES_FRAMEBUFFER / 2) - fixToInt(Mul( intToFix(YRES_FRAMEBUFFER / 2), vertex[5]))
                };

                uint32_t colours[3] = {0xFF0000FF, 0x00FF00FF, 0x0000FFFF};

                fillTriangle(&coords[0], &colours[0]);
            }
        }
        break;
    case GL_POINTS:
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
    notImplementedYet(__func__);
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
    }
    ///TODO: handle other client states
}

GLAPI void APIENTRY glFinish(void)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glFlush(void)
{
    notImplementedYet(__func__);
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
    GLfixed twoTimesN= Mul(intToFix(2), zNear);
    projectionMatrix[0] = Div(twoTimesN, (right - left ));
    projectionMatrix[5] = Div(twoTimesN, (top - bottom));
    projectionMatrix[8] = Div((right + left),  (right - left));
    projectionMatrix[9] = Div((top + bottom),  (top - bottom));
    projectionMatrix[10] = -Div((zFar + zNear), ( zFar - zNear ));
    projectionMatrix[11] = -intToFix(1);
    projectionMatrix[14] = - Div(Mul( twoTimesN, zFar), (zFar - zNear));
}

GLAPI void APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
    notImplementedYet(__func__);
}

GLAPI GLenum APIENTRY glGetError(void)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
    notImplementedYet(__func__);
}

GLAPI const GLubyte* APIENTRY glGetString(GLenum name)
{
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed* params)
{
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPixelStorei(GLenum pname, GLint param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPointSize(GLfloat size)
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
    --matrixStackTop;

    memcpy(&projectionMatrix[0], &projectionMatrixStack[matrixStackTop][0], sizeof(GLfixed) * 16);
    memcpy(&modelViewMatrix[0], &modelViewMatrixStack[matrixStackTop][0], sizeof(GLfixed) * 16);

    if (matrixStackTop < 0)
    {
        currentError = GL_STACK_UNDERFLOW;
    }
}

GLAPI void APIENTRY glPushMatrix(void)
{
    memcpy(&projectionMatrixStack[matrixStackTop][0], &projectionMatrix[0], sizeof(GLfixed) * 16);
    memcpy(&modelViewMatrixStack[matrixStackTop][0], &modelViewMatrix[0], sizeof(GLfixed) * 16);

    ++matrixStackTop;

    if (matrixStackTop >= MATRIX_STACK_CAPACITY)
    {
        currentError = GL_STACK_OVERFLOW;
    }
}

GLAPI void APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                                 GLvoid* pixels)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    /* Convert angle (degrees) to radians */
    float rad = fixToFloat(angle) * (float)M_PI / 180.0f;

    float fx = fixToFloat(x);
    float fy = fixToFloat(y);
    float fz = fixToFloat(z);

    /* Normalize axis */
    float len = sqrtf(fx * fx + fy * fy + fz * fz);
    if (len == 0.0f)
        return;

    fx /= len;
    fy /= len;
    fz /= len;

    float c = cosf(rad);
    float s = sinf(rad);
    float t = 1.0f - c;

    GLfixed R[16];

    /* First column */
    R[0] = floatToFix(t*fx*fx + c);
    R[1] = floatToFix(t*fx*fy + s*fz);
    R[2] = floatToFix(t*fx*fz - s*fy);
    R[3] = 0;

    /* Second column */
    R[4] = floatToFix(t*fx*fy - s*fz);
    R[5] = floatToFix(t*fy*fy + c);
    R[6] = floatToFix(t*fy*fz + s*fx);
    R[7] = 0;

    /* Third column */
    R[8] = floatToFix(t*fx*fz + s*fy);
    R[9] = floatToFix(t*fy*fz - s*fx);
    R[10] = floatToFix(t*fz*fz + c);
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
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
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
    memcpy( &modelViewMatrix[0], &tmp[0], sizeof(GLfixed) * 16 );
}

GLAPI void APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    vertexStride = stride;
    vertexType = type;
    vertexSize = size;
    vertexPointer = pointer;

    if (size != 2 && size != 3 && size != 4) {
        currentError = GL_INVALID_VALUE;
    }

    if (stride < 0 ) {
        currentError = GL_INVALID_VALUE;
    }
    ///TODO: handle type errors
}

GLAPI void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    viewportX = x;
    viewportY = y;
    viewportWidth = width;
    viewportHeight = height;
}
