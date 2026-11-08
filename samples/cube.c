/*
 * Copyright (C) 2026  Daniel Monteiro   All Rights Reserved.
 *
 * SPDX-License-Identifier: Apache 2.0
 */

#include <stdint.h>

#include "tex64x64xRGBA32.h"

#define kIntegerPart 16

#define fixToInt(fp) ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v) ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2) ((GLfixed)((((int64_t)(v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) ((fp) / 65536.0f)

#define floatToFix(f) ((GLfixed)(65536.0f * (f)))

#include <GLES/gl.h>
#include <stdlib.h>

#include "internal.h"


GLfixed cubeRotX = 0;
GLfixed cubeRotY = 0;
GLfixed cubeRotZ = 0;
GLuint textureID;

void draw(void) {

	struct Vertex {
		GLbyte coords[3];
		uint8_t pad[4];
	};

	static const struct Vertex verts[8] = {
	    /*
	     *
	     *    0 |-- 2
	     *      |  /|
	     *      | / |
	     *      |/--|
	     *      1   3
	     */

	    {-1, 1, 1},  {-1, -1, 1},  {1, 1, 1},  {1, -1, 1},
	    {-1, 1, -1}, {-1, -1, -1}, {1, 1, -1}, {1, -1, -1},
	};

	static const uint8_t indices[36] = {
	    0, 1, 2, /* 3 */
	    1, 2, 3, /* 6 */

	    4, 5, 6, /* 9 */
	    5, 6, 7, /* 12 */

	    2, 6, 3, /* 15 */
	    3, 6, 7, /* 18 */

	    0, 4, 1, /* 21 */
	    1, 4, 5, /* 24 */

	    0, 4, 6, /* 27 */
	    0, 6, 2, /* 30 */

	    1, 5, 7, /* 33 */
	    1, 7, 3  /* 36 */
	};

	static const GLfixed texCoords[16] = {
	    intToFix(0), intToFix(1), intToFix(0), intToFix(0), intToFix(1), intToFix(1),
	    intToFix(1), intToFix(0), intToFix(0), intToFix(1), intToFix(0), intToFix(0),
	    intToFix(1), intToFix(1), intToFix(1), intToFix(0),
	};

	glClear(GL_COLOR_BUFFER_BIT
#ifndef DISABLE_DEPTH_BUFFER
	        | GL_DEPTH_BUFFER_BIT
#endif
	);

	glPushMatrix();
	glRotatex(cubeRotX, intToFix(1), 0, 0);
	glRotatex(cubeRotY, 0, intToFix(1), 0);
	glRotatex(cubeRotZ, 0, 0, intToFix(1));

	glEnable(GL_TEXTURE_2D);

#ifndef DISABLE_DEPTH_BUFFER
	glEnable(GL_DEPTH_TEST);
#endif

	glTexCoordPointer(2, GL_FIXED, 0, texCoords);
	glVertexPointer(3, GL_BYTE, sizeof(struct Vertex), verts);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_2D, textureID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, indices);
	glPopMatrix();
}


void reshape(int width, int height) {
	const GLfixed ar = Div(intToFix(width), intToFix(height));

	glViewport(0, 0, (GLint)width, (GLint)height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glFrustumx(-ar, ar, -intToFix(1), intToFix(1), intToFix(5), intToFix(60));

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatex(0, 0, -intToFix(10));
}

void init(void) {
	static const GLfixed ambient[4] = {Div(intToFix(1), intToFix(5)), Div(intToFix(1), intToFix(5)),
	                                   Div(intToFix(1), intToFix(5)), intToFix(1)};
	static const GLfixed pos[4] = {intToFix(0), intToFix(0), intToFix(1), 0};


	const GLfixed grey = Div(intToFix(4), intToFix(10));
	const GLfixed fullAlpha = intToFix(1);
	glClearColorx(grey, grey, grey, fullAlpha);

	glGenTextures(1, &textureID);

	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGB, GL_UNSIGNED_BYTE, &tex[0]);
}

void handleKeys(int code) {
	switch (code) {
	case 'a':
		cubeRotY += intToFix(5);
		break;
	case 'd':
		cubeRotY -= intToFix(5);
		break;
	case 'w':
		cubeRotX += intToFix(5);
		break;
	case 's':
		cubeRotX -= intToFix(5);
		break;
	case 'z':
		cubeRotZ -= intToFix(5);
		break;
	case 'x':
		cubeRotZ += intToFix(5);
		break;

	case '0':
		cubeRotX = 0;
		cubeRotY = 0;
		cubeRotZ = 0;
		break;

	default:
		break;
	}
}

void mainLoop(void) {
	reshape(XRES_FRAMEBUFFER, YRES_FRAMEBUFFER);
	while (1) {
		draw();
		swapBuffers();
	}
}

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	initWindow(handleKeys);

	init();

	mainLoop();

	return 0;
}
