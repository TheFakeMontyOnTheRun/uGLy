//
// Created by Daniel Monteiro on 2026/06/07.
//
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <assert.h>
#include <stdint.h>

#include "internal.h"

extern "C" {
#include "GLES/gl.h"
}

TEST(TestMatricesStack, canPushMatrices)
{
    glPushMatrix();
}