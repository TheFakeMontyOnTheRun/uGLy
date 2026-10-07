//
// Created by Daniel Monteiro on 2026/06/14.
//
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <cassert>
#include <cstdint>

extern "C" {
#include "GLES/gl.h"
#include "internal.h"
}

class TestErrorHandling : public testing::Test {
protected:
	void SetUp() override { uGLyInit(); }

	void TearDown() override {}
};

TEST_F(TestErrorHandling, errorsAreSticky) {
	glPopMatrix();

	for (int c = 0; c < MATRIX_STACK_CAPACITY; ++c) {
		glPushMatrix();
	}

	glPushMatrix();

	ASSERT_EQ(glGetError(), GL_STACK_UNDERFLOW);
}

TEST_F(TestErrorHandling, errorFlagClearsAfterQuerying) {
	glPopMatrix();
	ASSERT_EQ(glGetError(), GL_STACK_UNDERFLOW);
	ASSERT_EQ(glGetError(), GL_NO_ERROR);
}
