#include <math.h>
#include <stdint.h>
#include <GLES/gl.h>
#include "matricesFP.h"

///TODO: add LUT for cos, sin and tan in fixed point


#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)

#define floatToFix(f) (65536.0f * (f))

/**
 *
 * @param out
 * @param left
 * @param right
 * @param bottom
 * @param top
 * @param znear
 * @param zfar
 */
void
mat4x4_ortho(t_mat4x4 out, GLfixed left, GLfixed right, GLfixed bottom, GLfixed top,
             GLfixed znear,
             GLfixed zfar)
{
#define T(a, b) (a * 4 + b)

    out[T(0, 0)] = Div(intToFix(2), (right - left));
    out[T(0, 1)] = 0;
    out[T(0, 2)] = 0;
    out[T(0, 3)] = 0;

    out[T(1, 1)] = Div(intToFix(2), (top - bottom));
    out[T(1, 0)] = 0;
    out[T(1, 2)] = 0;
    out[T(1, 3)] = 0;

    out[T(2, 2)] = -Div(intToFix(2), (zfar - znear));
    out[T(2, 0)] = 0;
    out[T(2, 1)] = 0;
    out[T(2, 3)] = 0;

    out[T(3, 0)] = -Div((right + left), (right - left));
    out[T(3, 1)] = -Div((top + bottom), (top - bottom));
    out[T(3, 2)] = -Div((zfar + znear), (zfar - znear));
    out[T(3, 3)] = intToFix(1);

#undef T
}

/**
 *
 * @param out
 * @param fov
 * @param ratio
 * @param znear
 * @param zfar
 */
void mat4x4_perspective(t_mat4x4 out, GLfixed fov, GLfixed ratio, GLfixed znear,
                        GLfixed zfar)
{
    GLfixed rad = floatToFix(M_PI / 180.0f);
    GLfixed oneOverTanFovDiv2 = Div(intToFix(1), floatToFix(tan(fixToFloat(fov) * rad / 2.0f)));

    out[0] = Div(oneOverTanFovDiv2, ratio);
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;

    out[4] = 0;
    out[5] = oneOverTanFovDiv2;
    out[6] = 0;
    out[7] = 0;

    out[8] = 0;
    out[9] = 0;
    out[10] = -Div((zfar + znear), (zfar - znear));
    out[11] = -1;

    out[12] = 0;
    out[13] = 0;
    out[14] = -Div(Mul(Mul(2 , zfar), znear), (zfar - znear));
    out[15] = 0;
}

/**
 *
 * @param out
 * @param cx
 * @param cy
 * @param cz
 * @param tx
 * @param ty
 * @param tz
 * @param ux
 * @param uy
 * @param uz
 */
void mat4x4_view(t_mat4x4 out,
                 GLfixed cx, GLfixed cy, GLfixed cz,
                 GLfixed tx, GLfixed ty, GLfixed tz,
                 GLfixed ux, GLfixed uy, GLfixed uz)
{
    out[0] = intToFix(1);
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = intToFix(1);
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = intToFix(1);
    out[11] = 0;
    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = intToFix(1);
}

/**
 *
 * @param out
 * @param ox
 * @param oy
 * @param oz
 * @param sx
 * @param sy
 * @param sz
 */
void
mat4x4_transform(t_mat4x4 out,
                 GLfixed ox, GLfixed oy, GLfixed oz,
                 GLfixed sx, GLfixed sy, GLfixed sz)
{
    out[0] = sx;
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = sy;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = sz;
    out[11] = 0;
    out[12] = ox;
    out[13] = oy;
    out[14] = oz;
    out[15] = intToFix(1);
}

void
mat4x4_transformVec(t_vec4 out,
                    t_mat4x4 m,
                    t_vec4 v)
{
    GLfixed x =
        Mul(m[0], v[0]) +
        Mul(m[4], v[1]) +
        Mul(m[8], v[2]) +
        Mul(m[12], v[3]);

    GLfixed y =
        Mul(m[1], v[0]) +
        Mul(m[5], v[1]) +
        Mul(m[9], v[2]) +
        Mul(m[13], v[3]);

    GLfixed z =
        Mul(m[2], v[0]) +
        Mul(m[6], v[1]) +
        Mul(m[10], v[2]) +
        Mul(m[14], v[3]);

    GLfixed w =
        Mul(m[3], v[0]) +
        Mul(m[7], v[1]) +
        Mul(m[11], v[2]) +
        Mul(m[15], v[3]);

    out[0] = x;
    out[1] = y;
    out[2] = z;
    out[3] = w;
}

/**
 *
 * @param out
 * @param deg
 */
void mat4x4_rotateX(t_mat4x4 out, GLfixed deg)
{
    GLfixed ca = floatToFix(cosf(fixToFloat(deg) * M_PI / 180.0f));
    GLfixed sa = floatToFix(sinf(fixToFloat(deg) * M_PI / 180.0f));

    out[0] = intToFix(1);
    out[1] = 0;
    out[2] = 0;
    out[3] = 0;
    out[4] = 0;
    out[5] = ca;
    out[6] = -sa;
    out[7] = 0;
    out[8] = 0;
    out[9] = sa;
    out[10] = ca;
    out[11] = 0;
    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = intToFix(1);
}

/**
 *
 * @param out
 * @param deg
 */
void mat4x4_rotateY(t_mat4x4 out, GLfixed deg)
{
    GLfixed ca = floatToFix(cosf(fixToFloat(deg) * M_PI / 180.0f));
    GLfixed sa = floatToFix(sinf(fixToFloat(deg) * M_PI / 180.0f));

    out[0] = ca;
    out[1] = 0;
    out[2] = sa;
    out[3] = 0;
    out[4] = 0;
    out[5] = 1;
    out[6] = 0;
    out[7] = 0;
    out[8] = -sa;
    out[9] = 0;
    out[10] = ca;
    out[11] = 0;
    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = 1;
}

/**
 *
 * @param out
 * @param deg
 */
void mat4x4_rotateZ(t_mat4x4 out, GLfixed deg)
{
    GLfixed ca = floatToFix(cosf(fixToFloat(deg) * M_PI / 180.0f));
    GLfixed sa = floatToFix(sinf(fixToFloat(deg) * M_PI / 180.0f));
    out[0] = ca;
    out[1] = -sa;
    out[2] = 0;
    out[3] = 0;
    out[4] = sa;
    out[5] = ca;
    out[6] = 0;
    out[7] = 0;
    out[8] = 0;
    out[9] = 0;
    out[10] = intToFix(1);
    out[11] = 0;
    out[12] = 0;
    out[13] = 0;
    out[14] = 0;
    out[15] = intToFix(1);
}

/**
 *
 * @param mat
 */
void mat4x4_transpose(t_mat4x4 mat)
{
    GLfixed tmp[16];
    /*
    a b c d
    e f g h
    i j k l
    m n o p


    a e i m
    b f j n
    c g k p
    d h l q
     */
    for (int y = 0; y < 4; ++y)
    {
        for (int x = 0; x < 4; ++x)
        {
            tmp[y * 4 + x] = mat[x * 4 + y];
        }
    }

    for (int c = 0; c < 16; ++c)
    {
        mat[c] = tmp[c];
    }
}

/**
 *
 * @param m1
 * @param m2
 * @param out
 */
void mat4x4_mul(t_mat4x4 m1, t_mat4x4 m2, t_mat4x4 out)
{
    out[0] = Mul(m1[(0 * 4) + 0], m2[(0 * 4) + 0]) +
        Mul(m1[(0 * 4) + 1], m2[(1 * 4) + 0]) +
        Mul(m1[(0 * 4) + 2], m2[(2 * 4) + 0]) +
        Mul(m1[(0 * 4) + 3], m2[(3 * 4) + 0]);
    out[1] = Mul(m1[(0 * 4) + 0], m2[(0 * 4) + 1]) +
        Mul(m1[(0 * 4) + 1], m2[(1 * 4) + 1]) +
        Mul(m1[(0 * 4) + 2], m2[(2 * 4) + 1]) +
        Mul(m1[(0 * 4) + 3], m2[(3 * 4) + 1]);
    out[2] = Mul(m1[(0 * 4) + 0], m2[(0 * 4) + 2]) +
        Mul(m1[(0 * 4) + 1], m2[(1 * 4) + 2]) +
        Mul(m1[(0 * 4) + 2], m2[(2 * 4) + 2]) +
        Mul(m1[(0 * 4) + 3], m2[(3 * 4) + 2]);
    out[3] = Mul(m1[(0 * 4) + 0], m2[(0 * 4) + 3]) +
        Mul(m1[(0 * 4) + 1], m2[(1 * 4) + 3]) +
        Mul(m1[(0 * 4) + 2], m2[(2 * 4) + 3]) +
        Mul(m1[(0 * 4) + 3], m2[(3 * 4) + 3]);

    out[4] = Mul(m1[(1 * 4) + 0], m2[(0 * 4) + 0]) +
        Mul(m1[(1 * 4) + 1], m2[(1 * 4) + 0]) +
        Mul(m1[(1 * 4) + 2], m2[(2 * 4) + 0]) +
        Mul(m1[(1 * 4) + 3], m2[(3 * 4) + 0]);
    out[5] = Mul(m1[(1 * 4) + 0], m2[(0 * 4) + 1]) +
        Mul(m1[(1 * 4) + 1], m2[(1 * 4) + 1]) +
        Mul(m1[(1 * 4) + 2], m2[(2 * 4) + 1]) +
        Mul(m1[(1 * 4) + 3], m2[(3 * 4) + 1]);
    out[6] = Mul(m1[(1 * 4) + 0], m2[(0 * 4) + 2]) +
        Mul(m1[(1 * 4) + 1], m2[(1 * 4) + 2]) +
        Mul(m1[(1 * 4) + 2], m2[(2 * 4) + 2]) +
        Mul(m1[(1 * 4) + 3], m2[(3 * 4) + 2]);
    out[7] = Mul(m1[(1 * 4) + 0], m2[(0 * 4) + 3]) +
        Mul(m1[(1 * 4) + 1], m2[(1 * 4) + 3]) +
        Mul(m1[(1 * 4) + 2], m2[(2 * 4) + 3]) +
        Mul(m1[(1 * 4) + 3], m2[(3 * 4) + 3]);

    out[8] = Mul(m1[(2 * 4) + 0], m2[(0 * 4) + 0]) +
        Mul(m1[(2 * 4) + 1], m2[(1 * 4) + 0]) +
        Mul(m1[(2 * 4) + 2], m2[(2 * 4) + 0]) +
        Mul(m1[(2 * 4) + 3], m2[(3 * 4) + 0]);
    out[9] = Mul(m1[(2 * 4) + 0], m2[(0 * 4) + 1]) +
        Mul(m1[(2 * 4) + 1], m2[(1 * 4) + 1]) +
        Mul(m1[(2 * 4) + 2], m2[(2 * 4) + 1]) +
        Mul(m1[(2 * 4) + 3], m2[(3 * 4) + 1]);
    out[10] = Mul(m1[(2 * 4) + 0], m2[(0 * 4) + 2]) +
        Mul(m1[(2 * 4) + 1], m2[(1 * 4) + 2]) +
        Mul(m1[(2 * 4) + 2], m2[(2 * 4) + 2]) +
        Mul(m1[(2 * 4) + 3], m2[(3 * 4) + 2]);
    out[11] = Mul(m1[(2 * 4) + 0], m2[(0 * 4) + 3]) +
        Mul(m1[(2 * 4) + 1], m2[(1 * 4) + 3]) +
        Mul(m1[(2 * 4) + 2], m2[(2 * 4) + 3]) +
        Mul(m1[(2 * 4) + 3], m2[(3 * 4) + 3]);

    out[12] = Mul(m1[(3 * 4) + 0], m2[(0 * 4) + 0]) +
        Mul(m1[(3 * 4) + 1], m2[(1 * 4) + 0]) +
        Mul(m1[(3 * 4) + 2], m2[(2 * 4) + 0]) +
        Mul(m1[(3 * 4) + 3], m2[(3 * 4) + 0]);
    out[13] = Mul(m1[(3 * 4) + 0], m2[(0 * 4) + 1]) +
        Mul(m1[(3 * 4) + 1], m2[(1 * 4) + 1]) +
        Mul(m1[(3 * 4) + 2], m2[(2 * 4) + 1]) +
        Mul(m1[(3 * 4) + 3], m2[(3 * 4) + 1]);
    out[14] = Mul(m1[(3 * 4) + 0], m2[(0 * 4) + 2]) +
        Mul(m1[(3 * 4) + 1], m2[(1 * 4) + 2]) +
        Mul(m1[(3 * 4) + 2], m2[(2 * 4) + 2]) +
        Mul(m1[(3 * 4) + 3], m2[(3 * 4) + 2]);
    out[15] = Mul(m1[(3 * 4) + 0], m2[(0 * 4) + 3]) +
        Mul(m1[(3 * 4) + 1], m2[(1 * 4) + 3]) +
        Mul(m1[(3 * 4) + 2], m2[(2 * 4) + 3]) +
        Mul(m1[(3 * 4) + 3], m2[(3 * 4) + 3]);
}

/**
 *
 * @param mat
 */
void mat4x4_identity(t_mat4x4 mat)
{
    mat[1] = mat[2] = mat[3] = mat[4] = mat[6] = mat[7] = mat[8] = mat[9] = mat[11] = mat[12] = mat[13] = mat[14] = 0;
    mat[0] = mat[5] = mat[10] = mat[15] = intToFix(1);
}
