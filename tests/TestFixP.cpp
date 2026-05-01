//
// Created by Daniel Monteiro on 29/04/2026.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <assert.h>
#include <stdint.h>

extern "C" {
#include "GLES/gl.h"
}

TEST(TestFixP, fixedPointZeroIsRegularZero)
{
    GLfixed zero = GLfixed(0);
    ASSERT_EQ(zero, 0);
}