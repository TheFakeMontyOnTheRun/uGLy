#ifndef SGDK
#include <math.h>
#include <stdint.h>
#endif
#include <GLES/gl.h>
#include "matricesFP.h"
#include "internal.h"
#include "fpsqrt.h"
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

GLfixed dotVec( t_vec4 v1,  t_vec4 v2)
{
    return Mul(v1[0], v2[0]) + Mul(v1[1], v2[1]) + Mul(v1[2], v2[2]) + Mul(v1[3], v2[3]);
}

GLfixed lengthVec(t_vec4 v)
{
    return sqrt_fx16_16_to_fx16_16(Mul(v[0], v[0]) +
                                   Mul(v[1], v[1]) +
                                   Mul(v[2], v[2]) +
                                   Mul(v[3], v[3]));
}

uint8_t normalizeVec( t_vec4 v,  t_vec4 out)
{
    const GLfixed len = lengthVec(v);
    if (len == 0)
    {
        return 0U;
    }

    GLfixed oneOverLen = Div( intToFix(1), len);
    out[0] = Mul( oneOverLen, v[0]);
    out[1] = Mul( oneOverLen, v[1]);
    out[2] = Mul( oneOverLen, v[2]);
    out[3] = Mul( oneOverLen, v[3]);

    return 1U;
}
