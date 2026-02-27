//
// Created by Daniel Monteiro on 27/02/2026.
//

#include <assert.h>
#include <stdio.h>
#include <GLES/gl.h>


#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) (fixToInt(Mul((fp), intToFix(16))) / 16.0f)


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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glPushMatrix(void)
{
    notImplementedYet(__func__);
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
    notImplementedYet(__func__);
}

GLAPI void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    notImplementedYet(__func__);
}
