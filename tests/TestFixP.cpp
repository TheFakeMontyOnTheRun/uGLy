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

TEST(TestFixP, theConversionFromFixedOneConvertToOne)
{
    GLfixed limit = intToFix(1);
    int32_t converted = fixToInt(limit);
    ASSERT_EQ(converted, 1);
}

TEST(TestFixP, theConversionFromFixed65535WillConvertToMinusOne)
{
    GLfixed limit = intToFix(65535);
    int32_t converted = fixToInt(limit);
    ASSERT_EQ(converted, -1);
}

TEST(TestFixP, goingBeyondTheLimitWillResultInOverflow)
{
    GLfixed limit = 0xFFFFFFFF;
    ASSERT_EQ(fixToInt(limit + 1), 0);
}

TEST(TestFixP, fixedPointHasGoodPrecisionForFractionalAddition)
{
    /* The classic */
    GLfixed dotOne = Div(GLfixed(1), GLfixed(10));
    GLfixed dotTwo = Div(GLfixed(2), GLfixed(10));
    GLfixed dotThree = Div(GLfixed(3), GLfixed(10));
    ASSERT_EQ(dotOne + dotTwo, dotThree);
}

TEST(TestFixP, negativeFixedPointResultsInNegativeInteger)
{
    GLfixed negative = -GLfixed(1);
    ASSERT_EQ(negative, -1);
}

TEST(TestFixP, fixedPointZeroIsRegularZero)
{
    GLfixed zero = GLfixed(0);
    ASSERT_EQ(zero, 0);
}