//
// Created by Daniel Monteiro on 29/04/2026.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <assert.h>
#include <stdint.h>

#include "internal.h"

extern "C" {
#include "matricesFP.h"

#include "GLES/gl.h"
}

TEST(TestMatricesFP, transposedMultiplicationsIsEqualToTransposedReverseOrder)
{
    GLfixed mat1[16];
    GLfixed mat2[16];
    GLfixed result1[16];
    GLfixed result2[16];
    GLfixed mat1trans[16];
    GLfixed mat2trans[16];

    for (int c = 0; c < 16; ++c)
    {
        mat1[c] = c;
        mat1trans[c] = c;
        mat2[15 - c] = c;
        mat2trans[15 - c] = c;
    }

    mat4x4_transpose(&mat1trans[0]);
    mat4x4_transpose(&mat2trans[0]);

    mat4x4_mul(&mat1[0], &mat2[0], &result1[0]);
    mat4x4_mul(&mat2trans[0], &mat1trans[0], &result2[0]);

    mat4x4_transpose(&result1[0]);

    for (int c = 0; c < 16; ++c)
    {
        ASSERT_EQ(result1[c], result2[c]);
    }
}

TEST(TestMatricesFP, multiplicationWithInverseMatrixProducesIdentity)
{
    GLfixed mat[16];
    GLfixed inverse[16];
    GLfixed ident[16];
    GLfixed result[16];

    mat4x4_identity(&ident[0]);

    memset(&mat[0], 0, sizeof(GLfixed) * 16);
    memset(&inverse[0], 0, sizeof(GLfixed) * 16);
    memset(&result[0], 0, sizeof(GLfixed) * 16);
    mat[0] = intToFix(1);
    mat[1] = intToFix(2);
    mat[4] = intToFix(3);
    mat[5] = intToFix(4);

    mat[10] = intToFix(1);
    mat[11] = intToFix(2);
    mat[14] = intToFix(3);
    mat[15] = intToFix(4);

    inverse[0] = -intToFix(2);
    inverse[1] = intToFix(1);
    inverse[4] = Div(intToFix(3), intToFix(2));
    inverse[5] = -Div(intToFix(1), intToFix(2));

    inverse[10] = -intToFix(2);
    inverse[11] = intToFix(1);
    inverse[14] = Div(intToFix(3), intToFix(2));
    inverse[15] = -Div(intToFix(1), intToFix(2));

    mat4x4_mul(&mat[0], &inverse[0], &result[0]);

    for (int c = 0; c < 16; ++c)
    {
        ASSERT_EQ(result[c], ident[c]);
    }
}

TEST(TestMatricesFP, multiplicationWithIdentityResultsInTheMatrix)
{
    GLfixed mat[16];
    GLfixed result[16];
    GLfixed ident[16];

    mat4x4_identity(&ident[0]);

    for (int c = 0; c < 16; ++c)
    {
        mat[c] = intToFix(c);
    }

    mat4x4_mul(&mat[0], &ident[0], &result[0]);

    for (int c = 0; c < 16; ++c)
    {
        ASSERT_EQ(result[c], mat[c]);
    }
}