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