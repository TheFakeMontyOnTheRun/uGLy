//
// Created by Daniel Monteiro on 2026/06/14.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cassert>
#include <cstdint>

extern "C" {
#include "internal.h"
#include "GLES/gl.h"
}

class TestErrorHandling : public testing::Test
{
protected:
    void SetUp() override {
        uGLyInit();
    }

    void TearDown() override {

    }
};

TEST_F(TestErrorHandling, errorFlagClearsAfterQuerying)
{
    glPopMatrix();
    ASSERT_EQ( glGetError(), GL_STACK_UNDERFLOW);
    ASSERT_EQ( glGetError(), GL_NO_ERROR);
}
