//
// Created by Daniel Monteiro on 27/02/2026.
//

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <GLES/gl.h>


static void invalidFunctionInvoked(const char* funcName)
{
    puts("Only OpenGL ES 1.0 Common Lite is supported");
    printf("Function called: %s\n", funcName);
    assert(0);
}

GLAPI void APIENTRY glAlphaFunc(GLenum func, GLclampf ref)
{
    (void)func;
    (void)ref;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glClearColor(GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha)
{
    (void)red;
    (void)green;
    (void)blue;
    (void)alpha;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glClearDepthf(GLclampf depth)
{
    (void)depth;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glColor4f(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
    (void)red;
    (void)green;
    (void)blue;
    (void)alpha;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glDepthRangef(GLclampf zNear, GLclampf zFar)
{
    (void)zNear;
    (void)zFar;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFogf(GLenum pname, GLfloat param)
{
    (void)pname;
    (void)param;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFogfv(GLenum pname, const GLfloat* params)
{
    (void)pname;
    (void)params;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glFrustumf(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    (void)left;
    (void)right;
    (void)bottom;
    (void)top;
    (void)zNear;
    (void)zFar;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightModelf(GLenum pname, GLfloat param)
{
    (void)pname;
    (void)param;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightModelfv(GLenum pname, const GLfloat* params)
{
    (void)pname;
    (void)params;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightf(GLenum light, GLenum pname, GLfloat param)
{
    (void)light;
    (void)pname;
    (void)param;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLightfv(GLenum light, GLenum pname, const GLfloat* params)
{
    (void)light;
    (void)pname;
    (void)params;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLineWidth(GLfloat width)
{
    (void)width;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glLoadMatrixf(const GLfloat* m)
{
    (void)m;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMaterialf(GLenum face, GLenum pname, GLfloat param)
{
    (void)face;
    (void)pname;
    (void)param;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMaterialfv(GLenum face, GLenum pname, const GLfloat* params)
{
    (void)face;
    (void)pname;
    (void)params;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMultMatrixf(const GLfloat* m)
{
    (void)m;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glMultiTexCoord4f(GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q)
{
    (void)target;
    (void)s;
    (void)t;
    (void)r;
    (void)q;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glNormal3f(GLfloat nx, GLfloat ny, GLfloat nz)
{
    (void)nx;
    (void)ny;
    (void)nz;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glPointSize(GLfloat size)
{
    (void)size;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glOrthof(GLfloat left, GLfloat right, GLfloat bottom, GLfloat top, GLfloat zNear, GLfloat zFar)
{
    (void)left;
    (void)right;
    (void)bottom;
    (void)top;
    (void)zNear;
    (void)zFar;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glPolygonOffset(GLfloat factor, GLfloat units)
{
    (void)factor;
    (void)units;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z)
{
    (void)angle;
    (void)x;
    (void)y;
    (void)z;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glSampleCoverage(GLclampf value, GLboolean invert)
{
    (void)value;
    (void)invert;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glScalef(GLfloat x, GLfloat y, GLfloat z)
{
    (void)x;
    (void)y;
    (void)z;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
    (void)target;
    (void)pname;
    (void)param;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexEnvfv(GLenum target, GLenum pname, const GLfloat* params)
{
    (void)target;
    (void)pname;
    (void)params;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTexParameterf(GLenum target, GLenum pname, GLfloat param)
{
    (void)target;
    (void)pname;
    (void)param;

    invalidFunctionInvoked(__func__);
}

GLAPI void APIENTRY glTranslatef(GLfloat x, GLfloat y, GLfloat z)
{
    (void)x;
    (void)y;
    (void)z;

    invalidFunctionInvoked(__func__);
}
