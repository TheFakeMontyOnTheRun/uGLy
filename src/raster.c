//
// Created by Daniel Monteiro on 01/03/2026.
//
#include <math.h>
#include <stdint.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int GLfixed;

typedef uint32_t FramebufferPixelFormat;

#define kIntegerPart 16

#define fixToInt(fp)  ((GLfixed)((fp) >> kIntegerPart))

#define intToFix(v)  ((int32_t)((v) << kIntegerPart))

#define Mul(v1, v2) ((GLfixed)((((v1) >> 6) * ((v2) >> 6)) >> 4))

#define Div(v1, v2)  ((GLfixed)((((int64_t) (v1)) * (1 << kIntegerPart)) / (v2)))

#define fixToFloat(fp) (fixToInt(Mul((fp), intToFix(16))) / 16.0f)

#define MATRIX_STACK_CAPACITY 16

#define MIN(v1, v2) (( (v1) < (v2) ) ? (v1) : (v2) )
#define MAX(v1, v2) (( (v1) > (v2) ) ? (v1) : (v2) )

#define YRES_FRAMEBUFFER 300
#define XRES_FRAMEBUFFER 300


extern uint32_t framebuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t zBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];
extern uint8_t stencilBuffer[XRES_FRAMEBUFFER * YRES_FRAMEBUFFER];

static void fillBottomFlat(const int *coords, FramebufferPixelFormat *colour) {
	int y = coords[1];
	GLfixed dXDy2;
	GLfixed dXDy1;
	GLfixed fX0;
	GLfixed fX1;
	GLfixed dX1X0;
	GLfixed dX0X2;
	GLfixed dY1Y0;
	GLfixed dY2Y0;
	int yFinal = MIN(coords[3], coords[5]);

	GLfixed x0 = intToFix(coords[0]);
	GLfixed y0 = intToFix(coords[1]);
	GLfixed x1 = intToFix(coords[2]);
	GLfixed y1 = intToFix(coords[3]);
	GLfixed x2 = intToFix(coords[4]);
	GLfixed y2 = intToFix(coords[5]);


	dX1X0 = (x1 - x0);
	dX0X2 = (x0 - x2);
	dY1Y0 = (y1 - y0);
	dY2Y0 = (y2 - y0);

	if (dY2Y0 == 0 || dY1Y0 == 0) {
		return;
	}

	dXDy2 = Div(dX0X2, dY2Y0);
	dXDy1 = Div(dX1X0, dY1Y0);
	fX0 = x0;
	fX1 = x0;

	for (; y < yFinal; ++y) {
		if (y >= YRES_FRAMEBUFFER) {
			return;
		} else if (y >= 0) {
			int iFX1 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX1)), 0);
			int iFX0 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX0)), 0);
			FramebufferPixelFormat *destination = &framebuffer[
					(XRES_FRAMEBUFFER * y) + MIN(iFX0, iFX1)];

			unsigned int px;
			size_t length = abs(iFX1 - iFX0);
			FramebufferPixelFormat* ptr = destination;
			for (px = 0; px < length; ++px) {
				*ptr = *colour;
				ptr++;
			}
		}
		fX0 -= dXDy2;
		fX1 += dXDy1;


	}
}


static void fillTopFlat(int *coords, FramebufferPixelFormat *colour) {
	int y = coords[1];
	int yFinal = MAX(coords[3], coords[5]);

	GLfixed x0 = intToFix(coords[0]);
	GLfixed y0 = intToFix(coords[1]);
	GLfixed x1 = intToFix(coords[2]);
	GLfixed y1 = intToFix(coords[3]);
	GLfixed x2 = intToFix(coords[4]);
	GLfixed y2 = intToFix(coords[5]);

	GLfixed dXDy1;
	GLfixed dXDy2;
	GLfixed fX0;
	GLfixed fX1;


	GLfixed dX1X0 = (x1 - x0);
	GLfixed dX2X0 = (x2 - x0);
	GLfixed dY0Y1 = (y0 - y1);
	GLfixed dY0Y2 = (y0 - y2);

	if (dY0Y1 == 0 || dY0Y2 == 0) {
		return;
	}

	dXDy1 = Div(dX1X0, dY0Y1);
	dXDy2 = Div(dX2X0, dY0Y2);
	fX0 = x0; /* p1 */
	fX1 = x0; /* p2 */

	for (; y >= yFinal; --y) {
		if (y < 0) {
			return;
		} else if (y < YRES_FRAMEBUFFER) {
			int iFX1 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX1)), 0);
			int iFX0 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX0)), 0);
			FramebufferPixelFormat *destination = &framebuffer[
					(XRES_FRAMEBUFFER * y) + MIN(iFX0, iFX1)];

			unsigned int px;
			size_t length = abs(iFX1 - iFX0);
			FramebufferPixelFormat* ptr = destination;
			for (px = 0; px < length; ++px) {
				*ptr = *colour;
				ptr++;
			}
		}

		fX0 += dXDy1;
		fX1 += dXDy2;
	}
}

void fillTriangle(int *coords, FramebufferPixelFormat *colour) {
    int newCoors[6];
    int newCoors2[6];

    int upper = -1;
    int lower = -1;
    int other = 0;
    int c;

    for (c = 0; c < 3; ++c) {
        if (upper == -1 || coords[(2 * c) + 1] < coords[(2 * upper) + 1]) {
            upper = c;
        }

        if (lower == -1 || coords[(2 * c) + 1] > coords[(2 * lower) + 1]) {
            lower = c;
        }
    }

    if (lower == 0 || upper == 0) {
        other = 1;
    }

    if ((lower == 1 || upper == 1) && (other == 1)) {
        other = 2;
    }

    if ((lower == 2 || upper == 2) && (other == 2)) {
        other = 0;
    }

    newCoors[0] = coords[2 * upper];
    newCoors[1] = coords[(2 * upper) + 1];
    newCoors[2] = coords[2 * lower];
    newCoors[3] = coords[(2 * lower) + 1];
    newCoors[4] = coords[2 * other];
    newCoors[5] = coords[(2 * other) + 1];

    newCoors2[0] = coords[2 * lower];
    newCoors2[1] = coords[(2 * lower) + 1];
    newCoors2[2] = coords[2 * other];
    newCoors2[3] = coords[(2 * other) + 1];
    newCoors2[4] = coords[2 * upper];
    newCoors2[5] = coords[(2 * upper) + 1];


    fillBottomFlat(&newCoors[0], colour);
    fillTopFlat(&newCoors2[0], colour);
}