//
// Created by Daniel Monteiro on 2026/06/07.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <cassert>
#include <cstdint>

extern "C" {
#include "internal.h"
#include "GLES/gl.h"
}

class TestMatricesStack : public testing::Test
{
protected:
    void SetUp() override {
        uGLyInit();
    }

    void TearDown() override {

    }
};

TEST_F(TestMatricesStack, canPushMatrices)
{
    glPushMatrix();
    ASSERT_EQ( glGetError(), GL_NO_ERROR);
}

TEST_F(TestMatricesStack, cantPopMatrixIf)
{
    glPopMatrix();
    ASSERT_EQ( glGetError(), GL_STACK_UNDERFLOW);
}