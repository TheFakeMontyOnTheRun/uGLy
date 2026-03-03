//
// Created by Daniel Monteiro on 27/02/2026.
//

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <GLES/gl.h>


static void invalidFunctionInvoked(char* funcName)
{
    puts("Only OpenGL ES 1.0 Common Lite is supported");
    printf("Function called: %s\n", funcName);
    assert(0);
}

GLAPI void APIENTRY glAlphaFunc(GLenum func, GLclampf ref)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glClearDepthf(GLclampf depth)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFogf(GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFogfv(GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightModelf(GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightModelfv(GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLineWidth(GLfloat width)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLoadMatrixf(const GLfloat* m)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMultMatrixf(const GLfloat* m)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glPointSize(GLfloat size)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    invalidFunctionInvoked(__func__);
}
