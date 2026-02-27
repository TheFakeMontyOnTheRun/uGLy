//
// Created by Daniel Monteiro on 27/02/2026.
//

#include <assert.h>
#include <stdio.h>
#include <GLES/gl.h>

void invalidFunctionInvoked(void)
{
    puts("Only OpenGL ES 1.0 Common Lite is supported");
    assert(0);
}

void notImplementedYet(void)
{
    puts("Not implemented yet");
    assert(0);
}

GLAPI void APIENTRY glActiveTexture(GLenum texture)
{
    notImplementedYet();
}

GLAPI void APIENTRY glAlphaFunc(GLenum func, GLclampf ref)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glAlphaFuncx(GLenum func, GLclampx ref)
{
    notImplementedYet();
}

GLAPI void APIENTRY glBindTexture(GLenum target, GLuint texture)
{
    notImplementedYet();
}

GLAPI void APIENTRY glBlendFunc(GLenum sfactor, GLenum dfactor)
{
    notImplementedYet();
}

GLAPI void APIENTRY glClear(GLbitfield mask)
{
    notImplementedYet();
}

GLAPI void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glClearColorx(GLclampx red, GLclampx green, GLclampx blue, GLclampx alpha)
{
    notImplementedYet();
}

GLAPI void APIENTRY glClearDepthf(GLclampf depth)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glClearDepthx(GLclampx depth)
{
    notImplementedYet();
}

GLAPI void APIENTRY glClearStencil(GLint s)
{
    notImplementedYet();
}

GLAPI void APIENTRY glClientActiveTexture(GLenum texture)
{
    notImplementedYet();
}

GLAPI void APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glColor4x(GLfixed red, GLfixed green, GLfixed blue, GLfixed alpha)
{
    notImplementedYet();
}

GLAPI void APIENTRY glColorMask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
    notImplementedYet();
}

GLAPI void APIENTRY glColorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    notImplementedYet();
}

GLAPI void APIENTRY glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width,
                                           GLsizei height, GLint border, GLsizei imageSize, const GLvoid* data)
{
    notImplementedYet();
}

GLAPI void APIENTRY glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                              GLsizei height, GLenum format, GLsizei imageSize, const GLvoid* data)
{
    notImplementedYet();
}

GLAPI void APIENTRY glCopyTexImage2D(GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width,
                                     GLsizei height, GLint border)
{
    notImplementedYet();
}

GLAPI void APIENTRY glCopyTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y,
                                        GLsizei width, GLsizei height)
{
    notImplementedYet();
}

GLAPI void APIENTRY glCullFace(GLenum mode)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDeleteTextures(GLsizei n, const GLuint* textures)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDepthFunc(GLenum func)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDepthMask(GLboolean flag)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glDepthRangex(GLclampx zNear, GLclampx zFar)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDisable(GLenum cap)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDisableClientState(GLenum array)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
    notImplementedYet();
}

GLAPI void APIENTRY glDrawElements(GLenum mode, GLsizei count, GLenum type, const GLvoid* indices)
{
    notImplementedYet();
}

GLAPI void APIENTRY glEnable(GLenum cap)
{
    notImplementedYet();
}

GLAPI void APIENTRY glEnableClientState(GLenum array)
{
    notImplementedYet();
}

GLAPI void APIENTRY glFinish(void)
{
    notImplementedYet();
}

GLAPI void APIENTRY glFlush(void)
{
    notImplementedYet();
}

GLAPI void APIENTRY glFogf(GLenum pname, GLfloat param)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glFogfv(GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glFogx(GLenum pname, GLfixed param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glFogxv(GLenum pname, const GLfixed* params)
{
    notImplementedYet();
}

GLAPI void APIENTRY glFrontFace(GLenum mode)
{
    notImplementedYet();
}

GLAPI void APIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glFrustumx(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    notImplementedYet();
}

GLAPI void APIENTRY glGenTextures(GLsizei n, GLuint* textures)
{
    notImplementedYet();
}

GLAPI GLenum APIENTRY glGetError(void)
{
    notImplementedYet();
}

GLAPI void APIENTRY glGetIntegerv(GLenum pname, GLint* params)
{
    notImplementedYet();
}

GLAPI const GLubyte* APIENTRY glGetString(GLenum name)
{
    notImplementedYet();
}

GLAPI void APIENTRY glHint(GLenum target, GLenum mode)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLightModelf(GLenum pname, GLfloat param)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glLightModelfv(GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glLightModelx(GLenum pname, GLfixed param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLightModelxv(GLenum pname, const GLfixed* params)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glLightx(GLenum light, GLenum pname, GLfixed param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLightxv(GLenum light, GLenum pname, const GLfixed* params)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLineWidth(GLfloat width)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glLineWidthx(GLfixed width)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLoadIdentity(void)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLoadMatrixf(const GLfloat* m)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glLoadMatrixx(const GLfixed* m)
{
    notImplementedYet();
}

GLAPI void APIENTRY glLogicOp(GLenum opcode)
{
    notImplementedYet();
}

GLAPI void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glMaterialx(GLenum face, GLenum pname, GLfixed param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glMaterialxv(GLenum face, GLenum pname, const GLfixed* params)
{
    notImplementedYet();
}

GLAPI void APIENTRY glMatrixMode(GLenum mode)
{
    notImplementedYet();
}

GLAPI void APIENTRY glMultMatrixf(const GLfloat* m)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glMultMatrixx(const GLfixed* m)
{
    notImplementedYet();
}

GLAPI void APIENTRY glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glMultiTexCoord4x(GLenum target, GLfixed s, GLfixed t, GLfixed r, GLfixed q)
{
    notImplementedYet();
}

GLAPI void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glNormal3x(GLfixed nx, GLfixed ny, GLfixed nz)
{
    notImplementedYet();
}

GLAPI void APIENTRY glNormalPointer(GLenum type, GLsizei stride, const GLvoid* pointer)
{
    notImplementedYet();
}

GLAPI void APIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glOrthox(GLfixed left, GLfixed right, GLfixed bottom, GLfixed top, GLfixed zNear, GLfixed zFar)
{
    notImplementedYet();
}

GLAPI void APIENTRY glPixelStorei(GLenum pname, GLint param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glPointSize(GLfloat size)
{
    notImplementedYet();
}

GLAPI void APIENTRY glPointSizex(GLfixed size)
{
    notImplementedYet();
}

GLAPI void APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glPolygonOffsetx(GLfixed factor, GLfixed units)
{
    notImplementedYet();
}

GLAPI void APIENTRY glPopMatrix(void)
{
    notImplementedYet();
}

GLAPI void APIENTRY glPushMatrix(void)
{
    notImplementedYet();
}

GLAPI void APIENTRY glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type,
                                 GLvoid* pixels)
{
    notImplementedYet();
}

GLAPI void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glRotatex(GLfixed angle, GLfixed x, GLfixed y, GLfixed z)
{
    notImplementedYet();
}

GLAPI void APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glSampleCoveragex(GLclampx value, GLboolean invert)
{
    notImplementedYet();
}

GLAPI void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glScalex(GLfixed x, GLfixed y, GLfixed z)
{
    notImplementedYet();
}

GLAPI void APIENTRY glScissor(GLint x, GLint y, GLsizei width, GLsizei height)
{
    notImplementedYet();
}

GLAPI void APIENTRY glShadeModel(GLenum mode)
{
    notImplementedYet();
}

GLAPI void APIENTRY glStencilFunc(GLenum func, GLint ref, GLuint mask)
{
    notImplementedYet();
}

GLAPI void APIENTRY glStencilMask(GLuint mask)
{
    notImplementedYet();
}

GLAPI void APIENTRY glStencilOp(GLenum fail, GLenum zfail, GLenum zpass)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTexCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glTexEnvx(GLenum target, GLenum pname, GLfixed param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTexEnvxv(GLenum target, GLenum pname, const GLfixed* params)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height,
                                 GLint border, GLenum format, GLenum type, const GLvoid* pixels)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glTexParameterx(GLenum target, GLenum pname, GLfixed param)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width,
                                    GLsizei height, GLenum format, GLenum type, const GLvoid* pixels)
{
    notImplementedYet();
}

GLAPI void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked();
}

GLAPI void APIENTRY glTranslatex(GLfixed x, GLfixed y, GLfixed z)
{
    notImplementedYet();
}

GLAPI void APIENTRY glVertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
    notImplementedYet();
}

GLAPI void APIENTRY glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
{
    notImplementedYet();
}
