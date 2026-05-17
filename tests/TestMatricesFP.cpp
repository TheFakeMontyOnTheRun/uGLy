//
// Created by Daniel Monteiro on 29/04/2026.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <assert.h>
#include <stdint.h>

#include "internal.h"

extern "C" {
#include "GLES/gl.h"
}

TEST(TestMatricesFP, dummy)
{
    GLfixed limit = intToFix(1);
    int32_t converted = fixToInt(limit);
    ASSERT_EQ(converted, 1);
}