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

#include "internal.h"

static void fillBottomFlat(const int* coords, FramebufferPixelFormat* colour)
{
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

    if (dY2Y0 == 0 || dY1Y0 == 0)
    {
        return;
    }

    dXDy2 = Div(dX0X2, dY2Y0);
    dXDy1 = Div(dX1X0, dY1Y0);
    fX0 = x0;
    fX1 = x0;

    for (; y < yFinal; ++y)
    {
        if (y >= YRES_FRAMEBUFFER)
        {
            return;
        }
        else if (y >= 0)
        {
            int iFX1 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX1)), 0);
            int iFX0 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX0)), 0);
            FramebufferPixelFormat* destination = &framebuffer[
                (XRES_FRAMEBUFFER * y) + MIN(iFX0, iFX1)];

            unsigned int px;
            size_t length = abs(iFX1 - iFX0);
            FramebufferPixelFormat* ptr = destination;
            for (px = 0; px < length; ++px)
            {
                *ptr = *colour;
                ptr++;
            }
        }
        fX0 -= dXDy2;
        fX1 += dXDy1;
    }
}


static void fillTopFlat(int* coords, FramebufferPixelFormat* colour)
{
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

    if (dY0Y1 == 0 || dY0Y2 == 0)
    {
        return;
    }

    dXDy1 = Div(dX1X0, dY0Y1);
    dXDy2 = Div(dX2X0, dY0Y2);
    fX0 = x0; /* p1 */
    fX1 = x0; /* p2 */

    for (; y >= yFinal; --y)
    {
        if (y < 0)
        {
            return;
        }
        else if (y < YRES_FRAMEBUFFER)
        {
            int iFX1 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX1)), 0);
            int iFX0 = MAX(MIN((XRES_FRAMEBUFFER - 1), fixToInt(fX0)), 0);
            FramebufferPixelFormat* destination = &framebuffer[
                (XRES_FRAMEBUFFER * y) + MIN(iFX0, iFX1)];

            unsigned int px;
            size_t length = abs(iFX1 - iFX0);
            FramebufferPixelFormat* ptr = destination;
            for (px = 0; px < length; ++px)
            {
                *ptr = *colour;
                ptr++;
            }
        }

        fX0 += dXDy1;
        fX1 += dXDy2;
    }
}

void fillTriangle(int* coords, FramebufferPixelFormat* colour)
{
    int newCoors[6];
    int newCoors2[6];

    int upper = -1;
    int lower = -1;
    int other = 0;
    int c;

    for (c = 0; c < 3; ++c)
    {
        if (upper == -1 || coords[(2 * c) + 1] < coords[(2 * upper) + 1])
        {
            upper = c;
        }

        if (lower == -1 || coords[(2 * c) + 1] > coords[(2 * lower) + 1])
        {
            lower = c;
        }
    }

    if (lower == 0 || upper == 0)
    {
        other = 1;
    }

    if ((lower == 1 || upper == 1) && (other == 1))
    {
        other = 2;
    }

    if ((lower == 2 || upper == 2) && (other == 2))
    {
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

static void drawTexturedBottomFlatTriangle(int *coords,
											uint8_t *uvCoords,
											struct Texture *texture,
											int z) {
	int y = coords[1];
	int u, v;
	GLfixed fU1, fU2, fV1, fV2;
	GLfixed fDU1;
	GLfixed fDU2;
	GLfixed fDV1;
	GLfixed fDV2;
	int yFinal = coords[5]; /* not the lowest, neither the topmost */

	GLfixed x0 = intToFix(coords[0]);
	GLfixed y0 = intToFix(coords[1]);
	GLfixed x1 = intToFix(coords[2]);
	GLfixed y1 = intToFix(coords[3]);
	GLfixed x2 = intToFix(coords[4]);
	GLfixed y2 = intToFix(coords[5]);

	GLfixed u0 = intToFix(uvCoords[0]);
	GLfixed v0 = intToFix(uvCoords[1]);
	GLfixed u1 = intToFix(uvCoords[2]);
	GLfixed v1 = intToFix(uvCoords[3]);
	GLfixed u2 = intToFix(uvCoords[4]);
	GLfixed v2 = intToFix(uvCoords[5]);
	GLfixed effectiveDelta;

	GLfixed dX1X0 = (x1 - x0);
	GLfixed dX0X2 = (x0 - x2);
	GLfixed dY1Y0 = (y1 - y0);
	GLfixed dY2Y0 = (y2 - y0);

	GLfixed dXDy2;
	GLfixed dXDy1;
	GLfixed fX0;
	GLfixed fX1;
	///TODO: consider the fact that dY2Y0 == dY1Y0.
	if (dY2Y0 == 0 || dY1Y0 == 0) {
		return;
	}

	dXDy2 = Div(dX0X2, dY2Y0);
	dXDy1 = Div(dX1X0, dY1Y0);
	fX0 = x0;
	fX1 = x0;

	fV1 = fV2 = v0;
	fU1 = fU2 = u0;

	effectiveDelta = intToFix(coords[5] - y);
	fDU1 = Div((u2 - u0), effectiveDelta);
	fDV1 = Div((v2 - v0), effectiveDelta);

	effectiveDelta = intToFix(coords[3] - y);
	fDU2 = Div((u1 - u0), effectiveDelta);
	fDV2 = Div((v1 - v0), effectiveDelta);

	for (; y < yFinal; ++y) {

		int iFX1;
		int iFX0;
		int flipped;
		GLfixed texelLineX;
		GLfixed texelLineY;
		GLfixed texelLineDX;
		GLfixed texelLineDY;
		GLfixed oneOverLimit;
		int limit;

		if (y >= YRES_FRAMEBUFFER) {
			return;
		}

		flipped = (fX0 > fX1);

		if (flipped) {
			iFX1 = fixToInt(fX0);
			iFX0 = fixToInt(fX1);
		} else {
			iFX1 = fixToInt(fX1);
			iFX0 = fixToInt(fX0);
		}

		limit = iFX1 - iFX0;

		if (limit) {
			FramebufferPixelFormat *destination;
			///TODO: bring in the Div LUT
			{
				oneOverLimit = Div(intToFix(1), intToFix(limit));
			}


			destination = &framebuffer[(XRES_FRAMEBUFFER * y) + iFX0];

			if (flipped) {
				texelLineDX = Mul((fU1 - fU2), oneOverLimit);
				texelLineDY = Mul((fV1 - fV2), oneOverLimit);
				texelLineX = fU2;
				texelLineY = fV2;
			} else {
				texelLineDX = Mul((fU2 - fU1), oneOverLimit);
				texelLineDY = Mul((fV2 - fV1), oneOverLimit);
				texelLineX = fU1;
				texelLineY = fV1;
			}

			if (y >= 0 && y < YRES_FRAMEBUFFER) {
				int xPos = iFX0;
				while (limit--) {
					u = (fixToInt(texelLineX)) % texture->width;
					v = (fixToInt(texelLineY)) % texture->height;

					if (xPos >= 0 && xPos < XRES_FRAMEBUFFER) {
						*destination = *(texture->texels + (texture->width * v) + u);
					}
					++xPos;
					++destination;
					texelLineX += texelLineDX;
					texelLineY += texelLineDY;
				}
			}
		}
		fU1 += fDU1;
		fV1 += fDV1;
		fU2 += fDU2;
		fV2 += fDV2;
		fX0 -= dXDy2;
		fX1 += dXDy1;
	}
}


static void drawTexturedTopFlatTriangle(int *coords,
										uint8_t *uvCoords,
										struct Texture *texture,
										int z) {
	int y = coords[1];
	int u, v;
	GLfixed fU1, fU2, fV1, fV2;
	GLfixed fDU1;
	GLfixed fDV1;
	GLfixed fDU2;
	GLfixed fDV2;
	int yFinal = coords[3]; /* not the upper, not the lowest */

	GLfixed x0 = intToFix(coords[0]);
	GLfixed y0 = intToFix(coords[1]);
	GLfixed x1 = intToFix(coords[2]);
	GLfixed y1 = intToFix(coords[3]);
	GLfixed x2 = intToFix(coords[4]);
	GLfixed y2 = intToFix(coords[5]);

	GLfixed u0 = intToFix(uvCoords[0]);
	GLfixed v0 = intToFix(uvCoords[1]);
	GLfixed u1 = intToFix(uvCoords[2]);
	GLfixed v1 = intToFix(uvCoords[3]);
	GLfixed u2 = intToFix(uvCoords[4]);
	GLfixed v2 = intToFix(uvCoords[5]);


	GLfixed dX1X0 = (x1 - x0);
	GLfixed dX2X0 = (x2 - x0);
	GLfixed dY0Y1 = (y0 - y1);
	GLfixed dY0Y2 = (y0 - y2);

	GLfixed dXDy1;
	GLfixed dXDy2;
	GLfixed fX0;
	GLfixed fX1;
	GLfixed effectiveDelta;

	if (dY0Y1 == 0 || dY0Y2 == 0) {
		return;
	}

	dXDy1 = Div(dX1X0, dY0Y1);
	dXDy2 = Div(dX2X0, dY0Y2);
	fX0 = x0; /* p1 */
	fX1 = x0; /* p2 */

	fV1 = fV2 = v0;
	fU1 = fU2 = u0;

	effectiveDelta = intToFix(y - coords[3]);
	fDU1 = Div((u1 - u0), effectiveDelta);
	fDV1 = Div((v1 - v0), effectiveDelta);

	effectiveDelta = intToFix(y - coords[5]);
	fDU2 = Div((u2 - u0), effectiveDelta);
	fDV2 = Div((v2 - v0), effectiveDelta);

	for (; y >= yFinal; --y) {
		int iFX1;
		int iFX0;
		int flipped;
		GLfixed texelLineX;
		GLfixed texelLineY;
		GLfixed texelLineDX;
		GLfixed texelLineDY;
		GLfixed oneOverLimit;
		int limit;

		if (y <= 0) {
			return;
		}

		flipped = (fX0 > fX1);

		if (flipped) {
			iFX1 = fixToInt(fX0);
			iFX0 = fixToInt(fX1);
		} else {
			iFX1 = fixToInt(fX1);
			iFX0 = fixToInt(fX0);
		}

		limit = iFX1 - iFX0;

		if (limit) {
			FramebufferPixelFormat *destination;
			///TODO: bring in the damn Div LUT
			{
				oneOverLimit = Div(intToFix(1), intToFix(limit));
			}


			destination = &framebuffer[(XRES_FRAMEBUFFER * y) + iFX0];

			if (flipped) {
				texelLineDX = Mul((fU1 - fU2), oneOverLimit);
				texelLineDY = Mul((fV1 - fV2), oneOverLimit);
				texelLineX = fU2;
				texelLineY = fV2;
			} else {
				texelLineDX = Mul((fU2 - fU1), oneOverLimit);
				texelLineDY = Mul((fV2 - fV1), oneOverLimit);
				texelLineX = fU1;
				texelLineY = fV1;
			}

			if (y >= 0 && y < YRES_FRAMEBUFFER) {

				int xPos = iFX0;

				while (limit--) {
					u = (fixToInt(texelLineX)) % texture->width;
					v = (fixToInt(texelLineY)) % texture->height;

					if (xPos >= 0 && xPos < XRES_FRAMEBUFFER) {
						*destination = *(texture->texels + (texture->width * v) + u);
					}

					++xPos;
					++destination;
					texelLineX += texelLineDX;
					texelLineY += texelLineDY;
				}
			}
		}
		fU1 += fDU1;
		fV1 += fDV1;
		fU2 += fDU2;
		fV2 += fDV2;
		fX0 += dXDy1;
		fX1 += dXDy2;
	}
}


void
drawTexturedTriangle(int *coords,
					uint8_t *uvCoords,
					struct Texture *texture,
                     int z) {
    int newCoors[6];
    uint8_t newUV[6];
    int c;
    int upper = -1;
    int lower = -1;
    int other = 0;

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

    newUV[0] = uvCoords[2 * upper];
    newUV[1] = texture->height - uvCoords[(2 * upper) + 1];

    newUV[2] = uvCoords[2 * lower];
    newUV[3] = texture->height - uvCoords[(2 * lower) + 1];

    newUV[4] = uvCoords[2 * other];
    newUV[5] = texture->height - uvCoords[(2 * other) + 1];


    drawTexturedBottomFlatTriangle(&newCoors[0], &newUV[0], texture, z);

    newCoors[0] = coords[2 * lower];
    newCoors[1] = coords[(2 * lower) + 1];
    newCoors[2] = coords[2 * other];
    newCoors[3] = coords[(2 * other) + 1];
    newCoors[4] = coords[2 * upper];
    newCoors[5] = coords[(2 * upper) + 1];

    newUV[0] = uvCoords[2 * lower];
    newUV[1] = texture->height - uvCoords[(2 * lower) + 1];

    newUV[2] = uvCoords[2 * other];
    newUV[3] = texture->height - uvCoords[(2 * other) + 1];

    newUV[4] = uvCoords[2 * upper];
    newUV[5] = texture->height - uvCoords[(2 * upper) + 1];


    drawTexturedTopFlatTriangle(&newCoors[0], &newUV[0], texture, z);
}
