//
// Created by Daniel Monteiro on 27/02/2026.
//

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLES/gl.h>

typedef uint32_t FramebufferPixelFormat;

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) (fixToInt(Mul((fp), intToFix(16))) / 16.0f)

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

void invalidFunctionInvoked(char *funcName)
{
    puts("Only OpenGL ES 1.0 Common Lite is supported");
    printf("Function called: %s\n", funcName);
    assert(0);
}

void notImplementedYet(char *funcName)
{
    puts("Not implemented yet");
    printf("Function called: %s\n", funcName);
    assert(0);
}

static void fillBottomFlat(const int *coords, FramebufferPixelFormat *colour) {
	int y = coords[1];
	GLfixed dXDy2;
	GLfixed dXDy1;
	GLfixed fX0;
	GLfixed fX1;
	GLfixed dX1X0;
	GLfixed dX0X2;
	GLfixed dY1Y0;
	GLfixed dY2Y0;
	int yFinal = MIN(coords[3], coords[5]);

	GLfixed x0 = intToFix(coords[0]);
	GLfixed y0 = intToFix(coords[1]);
	GLfixed x1 = intToFix(coords[2]);
	GLfixed y1 = intToFix(coords[3]);
	GLfixed x2 = intToFix(coords[4]);
	GLfixed y2 = intToFix(coords[5]);


	dX1X0 = (x1 - x0);
	dX0X2 = (x0 - x2);
	dY1Y0 = (y1 - y0);
	dY2Y0 = (y2 - y0);

	if (dY2Y0 == 0 || dY1Y0 == 0) {
		return;
	}

	dXDy2 = Div(dX0X2, dY2Y0);
	dXDy1 = Div(dX1X0, dY1Y0);
	fX0 = x0;
	fX1 = x0;

	for (; y < yFinal; ++y) {
		if (y >= YRES_FRAMEBUFFER) {
			return;
		} else if (y >= 0) {
			int iFX1 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX1)), 0);
			int iFX0 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX0)), 0);
			FramebufferPixelFormat *destination = &framebuffer[
					(XRES_FRAMEBUFFER * y) + MIN(iFX0, iFX1)];

			unsigned int px;
			size_t length = abs(iFX1 - iFX0);
			FramebufferPixelFormat* ptr = destination;
			for (px = 0; px < length; ++px) {
				*ptr = *colour;
				ptr++;
			}
		}
		fX0 -= dXDy2;
		fX1 += dXDy1;


	}
}


static void fillTopFlat(int *coords, FramebufferPixelFormat *colour) {
	int y = coords[1];
	int yFinal = MAX(coords[3], coords[5]);

	GLfixed x0 = intToFix(coords[0]);
	GLfixed y0 = intToFix(coords[1]);
	GLfixed x1 = intToFix(coords[2]);
	GLfixed y1 = intToFix(coords[3]);
	GLfixed x2 = intToFix(coords[4]);
	GLfixed y2 = intToFix(coords[5]);

	GLfixed dXDy1;
	GLfixed dXDy2;
	GLfixed fX0;
	GLfixed fX1;


	GLfixed dX1X0 = (x1 - x0);
	GLfixed dX2X0 = (x2 - x0);
	GLfixed dY0Y1 = (y0 - y1);
	GLfixed dY0Y2 = (y0 - y2);

	if (dY0Y1 == 0 || dY0Y2 == 0) {
		return;
	}

	dXDy1 = Div(dX1X0, dY0Y1);
	dXDy2 = Div(dX2X0, dY0Y2);
	fX0 = x0; /* p1 */
	fX1 = x0; /* p2 */

	for (; y >= yFinal; --y) {
		if (y < 0) {
			return;
		} else if (y < YRES_FRAMEBUFFER) {
			int iFX1 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX1)), 0);
			int iFX0 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX0)), 0);
			FramebufferPixelFormat *destination = &framebuffer[
					(XRES_FRAMEBUFFER * y) + MIN(iFX0, iFX1)];

			unsigned int px;
			size_t length = abs(iFX1 - iFX0);
			FramebufferPixelFormat* ptr = destination;
			for (px = 0; px < length; ++px) {
				*ptr = *colour;
				ptr++;
			}
		}

		fX0 += dXDy1;
		fX1 += dXDy2;
	}
}

static void fillTriangle(int *coords, FramebufferPixelFormat *colour) {
    int newCoors[6];
    int newCoors2[6];

    int upper = -1;
    int lower = -1;
    int other = 0;
    int c;

    for (c = 0; c < 3; ++c) {
        if (upper == -1 || coords[(2 * c) + 1] < coords[(2 * upper) + 1]) {
            upper = c;
        }

        if (lower == -1 || coords[(2 * c) + 1] > coords[(2 * lower) + 1]) {
            lower = c;
        }
    }

    if (lower == 0 || upper == 0) {
        other = 1;
    }

    if ((lower == 1 || upper == 1) && (other == 1)) {
        other = 2;
    }

    if ((lower == 2 || upper == 2) && (other == 2)) {
        other = 0;
    }

    newCoors[0] = coords[2 * upper];
    newCoors[1] = coords[(2 * upper) + 1];
    newCoors[2] = coords[2 * lower];
    newCoors[3] = coords[(2 * lower) + 1];
    newCoors[4] = coords[2 * other];
    newCoors[5] = coords[(2 * other) + 1];

    newCoors2[0] = coords[2 * lower];
    newCoors2[1] = coords[(2 * lower) + 1];
    newCoors2[2] = coords[2 * other];
    newCoors2[3] = coords[(2 * other) + 1];
    newCoors2[4] = coords[2 * upper];
    newCoors2[5] = coords[(2 * upper) + 1];


    fillBottomFlat(&newCoors[0], colour);
    fillTopFlat(&newCoors2[0], colour);
}

GLAPI void APIENTRY glActiveTexture(GLenum texture)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glAlphaFunc(GLenum func, GLclampf ref)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    clearColor = red << 24 | green << 16 | blue << 8 | alpha;
}

GLAPI void APIENTRY glClearDepthf(GLclampf depth)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
    invalidFunctionInvoked(__func__);
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
    switch (mode)
    {
    case GL_TRIANGLES:
        {
            int c;
            int finalCount = count / 3;
            for (c = first; c < finalCount; ++c)
            {
                GLfixed vertex[6] = {
                    *((GLfixed*)vertexPointer + 0), *((GLfixed*)vertexPointer + 1),
                    *((GLfixed*)vertexPointer + 2), *((GLfixed*)vertexPointer + 3),
                    *((GLfixed*)vertexPointer + 4), *((GLfixed*)vertexPointer + 5)
                };



                int coords[6] = {
                    (XRES_FRAMEBUFFER / 2) + fixToInt( Mul( intToFix(XRES_FRAMEBUFFER / 2), vertex[0])), (YRES_FRAMEBUFFER / 2) - fixToInt( Mul( intToFix(YRES_FRAMEBUFFER / 2), vertex[1])),
                    (XRES_FRAMEBUFFER / 2) + fixToInt( Mul( intToFix(XRES_FRAMEBUFFER / 2), vertex[2])), (YRES_FRAMEBUFFER / 2) - fixToInt( Mul( intToFix(YRES_FRAMEBUFFER / 2), vertex[3])),
                    (XRES_FRAMEBUFFER / 2) + fixToInt( Mul( intToFix(XRES_FRAMEBUFFER / 2), vertex[4])), (YRES_FRAMEBUFFER / 2) - fixToInt( Mul( intToFix(YRES_FRAMEBUFFER / 2), vertex[5]))
                };

                uint32_t colours[3] = { 0xFF0000FF, 0x00FF00FF, 0x0000FFFF};

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

GLAPI void APIENTRY glFogf(GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFogfv(GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    notImplementedYet(__func__);
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

GLAPI void APIENTRY glLightModelf(GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightModelfv(GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightModelx(GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightModelxv(GLenum pname, const GLfixed* params)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed* params)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLineWidth(GLfloat width)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLineWidthx(GLfixed width)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLoadIdentity(void)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLoadMatrixf(const GLfloat* m)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLoadMatrixx(const GLfixed* m)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glLogicOp(GLenum opcode)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glMultMatrixf(const GLfloat* m)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMultMatrixx(const GLfixed* m)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glPolygonOffsetx(GLfixed factor, GLfixed units)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPopMatrix(void)
{
    --matrixStackTop;
    if (matrixStackTop < 0)
    {
        currentError = GL_STACK_UNDERFLOW;
    }
}

GLAPI void APIENTRY glPushMatrix(void)
{
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

GLAPI void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glSampleCoveragex(GLclampx value, GLboolean invert)
{
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
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

GLAPI void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
}
