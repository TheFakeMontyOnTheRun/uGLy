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


TEST_F(TestMatricesStack, cantPushMatricesBeyondLimits)
{
    for (int c = 0; c < MATRIX_STACK_CAPACITY; ++c)
    {
        glPushMatrix();
    }

    ASSERT_EQ( glGetError(), GL_NO_ERROR);

    glPushMatrix();

    ASSERT_EQ( glGetError(), GL_STACK_OVERFLOW);
}

TEST_F(TestMatricesStack, canPushAndPopMatrixButNotBeyond)
{
    glPushMatrix();
    glPopMatrix();
    ASSERT_EQ( glGetError(), GL_NO_ERROR);

    glPopMatrix();
    ASSERT_EQ( glGetError(), GL_STACK_UNDERFLOW);
}


TEST_F(TestMatricesStack, pushingMatrixPreservesCurrentMatrix)
{
    GLfixed *modelView = currentModelViewMatrix(); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatex(intToFix(4), intToFix(3), intToFix(2));

    glPushMatrix();
    ASSERT_EQ( modelView[12], intToFix(4));
    ASSERT_EQ( modelView[13], intToFix(3));
    ASSERT_EQ( modelView[14], intToFix(2));
    ASSERT_EQ( modelView[15], intToFix(1));
}


TEST_F(TestMatricesStack, poppingMatrixWillRestoreMatrix)
{
    GLfixed *modelView = currentModelViewMatrix();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatex(intToFix(4), intToFix(3), intToFix(2));
    glPushMatrix();
    glLoadIdentity();
    ASSERT_EQ( modelView[12], intToFix(0));
    ASSERT_EQ( modelView[13], intToFix(0));
    ASSERT_EQ( modelView[14], intToFix(0));
    ASSERT_EQ( modelView[15], intToFix(1));

    glPopMatrix();

    ASSERT_EQ( modelView[12], intToFix(4));
    ASSERT_EQ( modelView[13], intToFix(3));
    ASSERT_EQ( modelView[14], intToFix(2));
    ASSERT_EQ( modelView[15], intToFix(1));
}