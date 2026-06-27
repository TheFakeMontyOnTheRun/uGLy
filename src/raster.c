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

static void drawTexturedBottomFlatTriangle(const int *coords,
											const uint8_t *uvCoords,
											const uint8_t *colourChannels,
											const struct Texture *texture,
#ifndef	DISABLE_DEPTH_BUFFER
											const uint16_t *z,
#endif
											const uint8_t* lightDot,
											const uint8_t* ambientLight) {
	int y = coords[1];
	int u, v;
	GLfixed fU1, fU2, fV1, fV2;
	GLfixed fDU1;
	GLfixed fDU2;
	GLfixed fDV1;
	GLfixed fDV2;

#ifndef	DISABLE_DEPTH_BUFFER
	uint16_t currentDepth;
	GLfixed fDZ1;
	GLfixed fDZ2;
	GLfixed fZ1;
	GLfixed fZ2;
#endif

	uint8_t currentLight[8];
	GLfixed fDLight1[8];
	GLfixed fDLight2[8];
	GLfixed fLight1[8];
	GLfixed fLight2[8];

	uint8_t currentR;
	uint8_t currentG;
	uint8_t currentB;
	GLfixed fDR1;
	GLfixed fDR2;
	GLfixed fDG1;
	GLfixed fDG2;
	GLfixed fDB1;
	GLfixed fDB2;
	GLfixed fR1;
	GLfixed fR2;
	GLfixed fG1;
	GLfixed fG2;
	GLfixed fB1;
	GLfixed fB2;


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

#ifndef	DISABLE_DEPTH_BUFFER
	fDZ1 = Div( intToFix(z[2] - z[0]), effectiveDelta );
#endif

	fDR1 = Div( intToFix( colourChannels[8] - colourChannels[0]), effectiveDelta );
	fDG1 = Div( intToFix( colourChannels[9] - colourChannels[1]), effectiveDelta );
	fDB1 = Div( intToFix( colourChannels[10] - colourChannels[2]), effectiveDelta );

	for ( int c = 0; c < 8; ++c)
	{
		fDLight1[c] = Div( (intToFix(lightDot[c * 3 + 2] - lightDot[c * 3 + 0])), effectiveDelta );
	}


	effectiveDelta = intToFix(coords[3] - y);

	fDU2 = Div((u1 - u0), effectiveDelta);
	fDV2 = Div((v1 - v0), effectiveDelta);

#ifndef	DISABLE_DEPTH_BUFFER
	fDZ2 = Div( intToFix(z[1] - z[0]), effectiveDelta );
#endif

	fDR2 = Div( intToFix( colourChannels[4] - colourChannels[0]), effectiveDelta );
	fDG2 = Div( intToFix( colourChannels[5] - colourChannels[1]), effectiveDelta );
	fDB2 = Div( intToFix( colourChannels[6] - colourChannels[2]), effectiveDelta );

	for ( int c = 0; c < 8; ++c)
	{
		fDLight2[c] = Div( (intToFix(lightDot[c * 3 + 1] - lightDot[c * 3 + 0])), effectiveDelta );
	}

#ifndef	DISABLE_DEPTH_BUFFER
	fZ1 = fZ2 = intToFix(z[0]);
#endif

	fR1 = fR2 = intToFix(colourChannels[0]);
	fG1 = fG2 = intToFix(colourChannels[1]);
	fB1 = fB2 = intToFix(colourChannels[2]);

	for ( int c = 0; c < 8; ++c)
	{
		fLight1[c] = fLight2[c] = intToFix(lightDot[c * 3]);
	}

	for (; y < yFinal; ++y) {

		int iFX1;
		int iFX0;
		int flipped;
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
			GLfixed texelLineX;
			GLfixed texelLineY;
			GLfixed texelLineDX;
			GLfixed texelLineDY;
			GLfixed oneOverLimit;

			FramebufferPixelFormat *destination;

#ifndef	DISABLE_DEPTH_BUFFER
			uint16_t *depthDestination;
			GLfixed fZ;
			GLfixed fDZLine;
#endif

			GLfixed fR;
			GLfixed fG;
			GLfixed fB;
			GLfixed fDRLine;
			GLfixed fDGLine;
			GLfixed fDBLine;

			GLfixed fLight[8];
			GLfixed fDLightLine[8];

			///TODO: bring in the Div LUT
			{
				oneOverLimit = Div(intToFix(1), intToFix(limit));
			}


			destination = &framebuffer[(XRES_FRAMEBUFFER * y) + iFX0];

#ifndef	DISABLE_DEPTH_BUFFER
			depthDestination = &zBuffer[(XRES_FRAMEBUFFER * y) + iFX0];
#endif

			if (flipped) {
				texelLineDX = Mul((fU1 - fU2), oneOverLimit);
				texelLineDY = Mul((fV1 - fV2), oneOverLimit);
				texelLineX = fU2;
				texelLineY = fV2;

#ifndef	DISABLE_DEPTH_BUFFER
				fZ = fZ2;
				fDZLine = Mul( (fZ1 - fZ2), oneOverLimit );
#endif

				fR = fR2;
				fG = fG2;
				fB = fB2;
				fDRLine = Mul( (fR1 - fR2), oneOverLimit );
				fDGLine = Mul( (fG1 - fG2), oneOverLimit );
				fDBLine = Mul( (fB1 - fB2), oneOverLimit );

				for ( int c = 0; c < 8; ++c)
				{
					fLight[c] = fLight2[c];
					fDLightLine[c] = Mul( (fLight1[c] - fLight2[c]), oneOverLimit );
				}

			} else {
				texelLineDX = Mul((fU2 - fU1), oneOverLimit);
				texelLineDY = Mul((fV2 - fV1), oneOverLimit);
				texelLineX = fU1;
				texelLineY = fV1;

#ifndef	DISABLE_DEPTH_BUFFER
				fZ = fZ1;
				fDZLine = Mul( (fZ2 - fZ1), oneOverLimit );
#endif

				fR = fR1;
				fG = fG1;
				fB = fB1;
				fDRLine = Mul( (fR2 - fR1), oneOverLimit );
				fDGLine = Mul( (fG2 - fG1), oneOverLimit );
				fDBLine = Mul( (fB2 - fB1), oneOverLimit );

				for ( int c = 0; c < 8; ++c)
				{
					fLight[c] = fLight1[c];
					fDLightLine[c] = Mul( (fLight2[c] - fLight1[c]), oneOverLimit );
				}
			}

			if (y >= 0) {
				int xPos = iFX0;
				while (limit--) {
					if (xPos >= 0 && xPos < XRES_FRAMEBUFFER) {
#ifndef	DISABLE_DEPTH_BUFFER
						currentDepth = fixToInt(fZ);
						if (!depthTestEnabled || *depthDestination >= currentDepth)
#endif
						{
							u = (fixToInt(texelLineX)) % texture->width;
							v = (fixToInt(texelLineY)) % texture->height;

							currentR = fixToInt(fR);
							currentG = fixToInt(fG);
							currentB = fixToInt(fB);

							for ( int c = 0; c < 8; ++c)
							{
								currentLight[c] = fixToInt(fLight[c]);
							}

							uint32_t texel = *(texture->texels + (texture->width * v) + u);
							uint8_t texelR = (texel & 0xFF000000) >> 24;
							uint8_t texelG = (texel & 0x00FF0000) >> 16;
							uint8_t texelB = (texel & 0x0000FF00) >>  8;
							uint32_t fragR = ( texelR * (currentR * ambientLight[0]) / 256) / 256;
							uint32_t fragG = ( texelG * (currentG * ambientLight[1]) / 256) / 256;
							uint32_t fragB = ( texelB * (currentB * ambientLight[2]) / 256) / 256;

							for (int c = 0; c < 8; ++c)
							{
								fragR += ( texelR * (currentR * currentLight[c]) / 256) / 256;
								fragG += ( texelG * (currentG * currentLight[c]) / 256) / 256;
								fragB += ( texelB * (currentB * currentLight[c]) / 256) / 256;
							}

							*destination = MAKE_PIXEL(MIN(fragR, 255 ), MIN(fragG, 255 ), MIN(fragB, 255 ), 0xFF);

#ifndef	DISABLE_DEPTH_BUFFER
							if (depthWritesEnabled)
							{
								*depthDestination = currentDepth;
							}
#endif

						}
					}
					++xPos;
					++destination;
					texelLineX += texelLineDX;
					texelLineY += texelLineDY;

#ifndef	DISABLE_DEPTH_BUFFER
					++depthDestination;
					fZ += fDZLine;
#endif

					fR += fDRLine;
					fG += fDGLine;
					fB += fDBLine;

					for ( int c = 0; c < 8; ++c)
					{
						fLight[c] += fDLightLine[c];
					}
				}
			}
		}
		fU1 += fDU1;
		fV1 += fDV1;
		fU2 += fDU2;
		fV2 += fDV2;
		fX0 -= dXDy2;
		fX1 += dXDy1;

#ifndef	DISABLE_DEPTH_BUFFER
		fZ1 += fDZ1;
		fZ2 += fDZ2;
#endif

		fR1 += fDR1;
		fG1 += fDG1;
		fB1 += fDB1;
		fR2 += fDR2;
		fG2 += fDG2;
		fB2 += fDB2;

		for ( int c = 0; c < 8; ++c)
		{
			fLight1[c] += fDLight1[c];
			fLight2[c] += fDLight2[c];
		}
	}
}


static void drawTexturedTopFlatTriangle(const int *coords,
										const uint8_t *uvCoords,
										const uint8_t *colourChannels,
										const struct Texture *texture,
#ifndef	DISABLE_DEPTH_BUFFER
										const uint16_t *z,
#endif
										const uint8_t* lightDot,
										const uint8_t* ambientLight) {
	int y = coords[1];
	int u, v;
	GLfixed fU1, fU2, fV1, fV2;
	GLfixed fDU1;
	GLfixed fDV1;
	GLfixed fDU2;
	GLfixed fDV2;

#ifndef	DISABLE_DEPTH_BUFFER
	uint16_t currentDepth;
	GLfixed fDZ1;
	GLfixed fDZ2;
	GLfixed fZ1;
	GLfixed fZ2;
#endif

	uint8_t currentLight[8];
	GLfixed fDLight1[8];
	GLfixed fDLight2[8];
	GLfixed fLight1[8];
	GLfixed fLight2[8];

	uint8_t currentR;
	uint8_t currentG;
	uint8_t currentB;
	GLfixed fDR1;
	GLfixed fDR2;
	GLfixed fDG1;
	GLfixed fDG2;
	GLfixed fDB1;
	GLfixed fDB2;
	GLfixed fR1;
	GLfixed fR2;
	GLfixed fG1;
	GLfixed fG2;
	GLfixed fB1;
	GLfixed fB2;

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

#ifndef	DISABLE_DEPTH_BUFFER
	fDZ1 = Div( intToFix(z[1] - z[0]), effectiveDelta );
#endif

	fDR1 = Div( intToFix( colourChannels[4] - colourChannels[0]), effectiveDelta );
	fDG1 = Div( intToFix( colourChannels[5] - colourChannels[1]), effectiveDelta );
	fDB1 = Div( intToFix( colourChannels[6] - colourChannels[2]), effectiveDelta );

	for ( int c = 0; c < 8; ++c)
	{
		fDLight1[c] = Div( (intToFix(lightDot[c * 3 + 1] - lightDot[c * 3])), effectiveDelta );
	}
	effectiveDelta = intToFix(y - coords[5]);

	fDU2 = Div((u2 - u0), effectiveDelta);
	fDV2 = Div((v2 - v0), effectiveDelta);

#ifndef	DISABLE_DEPTH_BUFFER
	fDZ2 = Div( intToFix(z[2] - z[0]), effectiveDelta );
#endif

	fDR2 = Div( intToFix( colourChannels[8] - colourChannels[0]), effectiveDelta );
	fDG2 = Div( intToFix( colourChannels[9] - colourChannels[1]), effectiveDelta );
	fDB2 = Div( intToFix( colourChannels[10] - colourChannels[2]), effectiveDelta );

	for ( int c = 0; c < 8; ++c)
	{
		fDLight2[c] = Div( (intToFix(lightDot[c * 3 + 2] - lightDot[c * 3])), effectiveDelta );
	}

#ifndef	DISABLE_DEPTH_BUFFER
	fZ1 = fZ2 = intToFix(z[0]);
#endif

	fR1 = fR2 = intToFix(colourChannels[0]);
	fG1 = fG2 = intToFix(colourChannels[1]);
	fB1 = fB2 = intToFix(colourChannels[2]);

	for ( int c = 0; c < 8; ++c)
	{
		fLight1[c] = fLight2[c] = intToFix(lightDot[c * 3]);
	}

	for (; y >= yFinal; --y) {
		int iFX1;
		int iFX0;
		int flipped;
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
			GLfixed texelLineX;
			GLfixed texelLineY;
			GLfixed texelLineDX;
			GLfixed texelLineDY;
			GLfixed oneOverLimit;

			FramebufferPixelFormat *destination;

#ifndef	DISABLE_DEPTH_BUFFER
			uint16_t *depthDestination;
			GLfixed fZ;
			GLfixed fDZLine;
#endif

			GLfixed fR;
			GLfixed fG;
			GLfixed fB;
			GLfixed fDRLine;
			GLfixed fDGLine;
			GLfixed fDBLine;

			GLfixed fLight[8];
			GLfixed fDLightLine[8];

			///TODO: bring in the damn Div LUT
			{
				oneOverLimit = Div(intToFix(1), intToFix(limit));
			}


			destination = &framebuffer[(XRES_FRAMEBUFFER * y) + iFX0];

#ifndef	DISABLE_DEPTH_BUFFER
			depthDestination = &zBuffer[(XRES_FRAMEBUFFER * y) + iFX0];
#endif

			if (flipped) {
				texelLineDX = Mul((fU1 - fU2), oneOverLimit);
				texelLineDY = Mul((fV1 - fV2), oneOverLimit);
				texelLineX = fU2;
				texelLineY = fV2;

#ifndef	DISABLE_DEPTH_BUFFER
				fZ = fZ2;
				fDZLine = Mul( (fZ1 - fZ2), oneOverLimit );
#endif

				fR = fR2;
				fG = fG2;
				fB = fB2;
				fDRLine = Mul( (fR1 - fR2), oneOverLimit );
				fDGLine = Mul( (fG1 - fG2), oneOverLimit );
				fDBLine = Mul( (fB1 - fB2), oneOverLimit );

				for ( int c = 0; c < 8; ++c)
				{
					fLight[c] = fLight2[c];
					fDLightLine[c] = Mul( (fLight1[c] - fLight2[c]), oneOverLimit );
				}
			} else {
				texelLineDX = Mul((fU2 - fU1), oneOverLimit);
				texelLineDY = Mul((fV2 - fV1), oneOverLimit);
				texelLineX = fU1;
				texelLineY = fV1;

#ifndef	DISABLE_DEPTH_BUFFER
				fZ = fZ1;
				fDZLine = Mul( (fZ2 - fZ1), oneOverLimit );
#endif

				fR = fR1;
				fG = fG1;
				fB = fB1;
				fDRLine = Mul( (fR2 - fR1), oneOverLimit );
				fDGLine = Mul( (fG2 - fG1), oneOverLimit );
				fDBLine = Mul( (fB2 - fB1), oneOverLimit );

				for ( int c = 0; c < 8; ++c)
				{
					fLight[c] = fLight1[c];
					fDLightLine[c] = Mul( (fLight2[c] - fLight1[c]), oneOverLimit );
				}
			}

			if (y < YRES_FRAMEBUFFER) {

				int xPos = iFX0;

				while (limit--) {
					if (xPos >= 0 && xPos < XRES_FRAMEBUFFER) {

#ifndef	DISABLE_DEPTH_BUFFER
						currentDepth = fixToInt(fZ);
						if (!depthTestEnabled || *depthDestination >= currentDepth)
#endif
						{
							u = (fixToInt(texelLineX)) % texture->width;
							v = (fixToInt(texelLineY)) % texture->height;
							currentR = fixToInt(fR);
							currentG = fixToInt(fG);
							currentB = fixToInt(fB);

							for (int c = 0; c < 8; ++c)
							{
								currentLight[c] = fixToInt(fLight[c]);
							}

							uint32_t texel = *(texture->texels + (texture->width * v) + u);
							uint8_t texelR = (texel & 0xFF000000) >> 24;
							uint8_t texelG = (texel & 0x00FF0000) >> 16;
							uint8_t texelB = (texel & 0x0000FF00) >>  8;
							uint32_t fragR = ( texelR * (currentR * ambientLight[0]) / 256) / 256;
							uint32_t fragG = ( texelG * (currentG * ambientLight[1]) / 256) / 256;
							uint32_t fragB = ( texelB * (currentB * ambientLight[2]) / 256) / 256;

							for (int c = 0; c < 8; ++c)
							{
								fragR += ( texelR * (currentR * currentLight[c]) / 256) / 256;
								fragG += ( texelG * (currentG * currentLight[c]) / 256) / 256;
								fragB += ( texelB * (currentB * currentLight[c]) / 256) / 256;
							}

							*destination = MAKE_PIXEL(MIN(fragR, 255 ), MIN(fragG, 255 ), MIN(fragB, 255 ), 0xFF);

#ifndef	DISABLE_DEPTH_BUFFER
							if (depthWritesEnabled)
							{
								*depthDestination = currentDepth;
							}
#endif
						}
					}

					++xPos;
					++destination;
					texelLineX += texelLineDX;
					texelLineY += texelLineDY;

#ifndef	DISABLE_DEPTH_BUFFER
					++depthDestination;
					fZ += fDZLine;
#endif

					fR += fDRLine;
					fG += fDGLine;
					fB += fDBLine;

					for ( int c = 0; c < 8; ++c)
					{
						fLight[c] += fDLightLine[c];
					}
				}
			}
		}
		fU1 += fDU1;
		fV1 += fDV1;
		fU2 += fDU2;
		fV2 += fDV2;
		fX0 += dXDy1;
		fX1 += dXDy2;

#ifndef	DISABLE_DEPTH_BUFFER
		fZ1 += fDZ1;
		fZ2 += fDZ2;
#endif

		fR1 += fDR1;
		fG1 += fDG1;
		fB1 += fDB1;
		fR2 += fDR2;
		fG2 += fDG2;
		fB2 += fDB2;

		for ( int c = 0; c < 8; ++c)
		{
			fLight1[c] += fDLight1[c];
			fLight2[c] += fDLight2[c];
		}
	}
}


void
drawTexturedTriangle(const int *coords,
					 const uint8_t *uvCoords,
					 const uint8_t *colourChannels,
					 const struct Texture *texture,
#ifndef	DISABLE_DEPTH_BUFFER
					 const uint16_t *z,
#endif
 					 const uint8_t* lightDot,
	 				 const uint8_t* ambientLight) {

    int newCoors[6];
    uint8_t newUV[6];
	uint8_t newColours[12];
	uint16_t newZ[3];
	uint8_t newLightDot[24];

    int upper = -1;
    int lower = -1;
    int other = 0;

    for (int c = 0; c < 3; ++c) {
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

	newColours[0] = colourChannels[4 * upper];
	newColours[1] = colourChannels[(4 * upper) + 1];
	newColours[2] = colourChannels[(4 * upper) + 2];
	newColours[3] = colourChannels[(4 * upper) + 3];

	newColours[4] = colourChannels[4 * lower];
	newColours[5] = colourChannels[(4 * lower) + 1];
	newColours[6] = colourChannels[(4 * lower) + 2];
	newColours[7] = colourChannels[(4 * lower) + 3];

	newColours[8] = colourChannels[4 * other];
	newColours[9] = colourChannels[(4 * other) + 1];
	newColours[10] = colourChannels[(4 * other) + 2];
	newColours[11] = colourChannels[(4 * other) + 3];

	/* note the convertion to the OpenGL texture coordinate system */
    newUV[0] = uvCoords[2 * upper];
    newUV[1] = texture->height - uvCoords[(2 * upper) + 1];

    newUV[2] = uvCoords[2 * lower];
    newUV[3] = texture->height - uvCoords[(2 * lower) + 1];

    newUV[4] = uvCoords[2 * other];
    newUV[5] = texture->height - uvCoords[(2 * other) + 1];

#ifndef	DISABLE_DEPTH_BUFFER
	newZ[0] = z[upper];
	newZ[1] = z[lower];
	newZ[2] = z[other];
#endif

	for (int l = 0; l < 8; ++l)
	{
		newLightDot[l * 3 + 0] = lightDot[l * 3 + upper];
		newLightDot[l * 3 + 1] = lightDot[l * 3 + lower];
		newLightDot[l * 3 + 2] = lightDot[l * 3 + other];
	}



    drawTexturedBottomFlatTriangle(&newCoors[0], &newUV[0], &newColours[0], texture,
#ifndef	DISABLE_DEPTH_BUFFER
    	&newZ[0],
#endif
    	&newLightDot[0], &ambientLight[0]);

    newCoors[0] = coords[2 * lower];
    newCoors[1] = coords[(2 * lower) + 1];
    newCoors[2] = coords[2 * other];
    newCoors[3] = coords[(2 * other) + 1];
    newCoors[4] = coords[2 * upper];
    newCoors[5] = coords[(2 * upper) + 1];


	newColours[0] = colourChannels[4 * lower];
	newColours[1] = colourChannels[(4 * lower) + 1];
	newColours[2] = colourChannels[(4 * lower) + 2];
	newColours[3] = colourChannels[(4 * lower) + 3];

	newColours[4] = colourChannels[4 * other];
	newColours[5] = colourChannels[(4 * other) + 1];
	newColours[6] = colourChannels[(4 * other) + 2];
	newColours[7] = colourChannels[(4 * other) + 3];

	newColours[8] = colourChannels[4 * upper];
	newColours[9] = colourChannels[(4 * upper) + 1];
	newColours[10] = colourChannels[(4 * upper) + 2];
	newColours[11] = colourChannels[(4 * upper) + 3];

	/* note the convertion to the OpenGL texture coordinate system */
    newUV[0] = uvCoords[2 * lower];
    newUV[1] = texture->height - uvCoords[(2 * lower) + 1];

    newUV[2] = uvCoords[2 * other];
    newUV[3] = texture->height - uvCoords[(2 * other) + 1];

    newUV[4] = uvCoords[2 * upper];
    newUV[5] = texture->height - uvCoords[(2 * upper) + 1];

#ifndef	DISABLE_DEPTH_BUFFER
	newZ[0] = z[lower];
	newZ[1] = z[other];
	newZ[2] = z[upper];
#endif

	for (int l = 0; l < 8; ++l)
	{
		newLightDot[l * 3 + 0] = lightDot[l * 3 + lower];
		newLightDot[l * 3 + 1] = lightDot[l * 3 + other];
		newLightDot[l * 3 + 2] = lightDot[l * 3 + upper];
	}

    drawTexturedTopFlatTriangle(&newCoors[0], &newUV[0], &newColours[0], texture,
#ifndef	DISABLE_DEPTH_BUFFER
    	&newZ[0],
#endif
    	&newLightDot[0], &ambientLight[0]);
}


static void fillRect(int x0, int y0, uint16_t width, uint16_t height, uint8_t* colour
#ifndef	DISABLE_DEPTH_BUFFER
, uint16_t currentDepth
#endif
)
{
	int x, y;

	FramebufferPixelFormat fragment = MAKE_PIXEL(colour[0], colour[1], colour[2], colour[3]);
	for (y = 0; y < height; ++y)
	{
		FramebufferPixelFormat* fbPtr =  &framebuffer[(XRES_FRAMEBUFFER * (y0 + y)) + x0];

#ifndef	DISABLE_DEPTH_BUFFER
		uint16_t* depthDestination = &zBuffer[(XRES_FRAMEBUFFER * (y0 + y)) + x0];
#endif

		for (x = 0; x < width; ++x)
		{
#ifndef	DISABLE_DEPTH_BUFFER
			if (!depthTestEnabled || *depthDestination >= currentDepth)
#endif
			{
				*fbPtr = fragment;
				++fbPtr;
#ifndef	DISABLE_DEPTH_BUFFER
				if (depthWritesEnabled)
				{
					*depthDestination = currentDepth;
				}
#endif
			}
		}
	}

}

void drawPoint(int* coords, uint8_t* colour,
#ifndef	DISABLE_DEPTH_BUFFER
uint16_t zValue,
#endif
uint16_t pointSize)
{
	uint16_t halfPointSize = pointSize / 2;
	fillRect(coords[0] - halfPointSize,
			 coords[1] - halfPointSize,
			 pointSize,
			 pointSize,
			 colour
#ifndef	DISABLE_DEPTH_BUFFER
			 ,zValue
#endif
			 );
}
