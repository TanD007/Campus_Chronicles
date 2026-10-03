#ifndef PLAYER_H
#define PLAYER_H

#include <math.h>
#include "Utils.hpp"
#include "GameState.hpp"

// NOTE: assumes iGraphics.h has already been included by iMain.cpp
// before this header is included. Do not #include "iGraphics.h" here
// again - it causes duplicate-definition errors (stb_image.h inside it
// gets compiled more than once in the same translation unit).
// Also assumes Assets.hpp (imgPlayer) has already been included.

// ---------------- Player ----------------
struct Player {
	double x, y;
	int w, h;
	double speed;
};

Player player = { 300, 200, 30, 40, 2.5 };

// Set in iMain.cpp's fixedUpdate() whenever a movement key is held -
// drives the tiny idle/step bob below.
bool playerIsMoving = false;
bool playerFacingLeft = false;

// Per-screen movement bounds (keeps the player inside the drawn area)
struct Bounds { double minX, maxX, minY, maxY; };

// maxY is capped at PATH_TOP (not the usual 370) so the player can walk freely on
// the grass and stone path but can't climb up into the new wall/archway
// artwork drawn above it in drawQuad() - see Draw.hpp.
Bounds quadBounds = { 20, 570, 20, 192 };
Bounds libraryBounds = { 20, 570, 20, 370 };
Bounds cafeteriaBounds = { 20, 570, 20, 370 };
Bounds classroomBounds = { 20, 570, 20, 370 };
Bounds labBounds = { 20, 570, 20, 370 };
Bounds facultyBounds = { 20, 570, 20, 370 };
Bounds zahidOfficeBounds = { 20, 570, 20, 370 };
Bounds towfiqueBounds = { 20, 570, 20, 370 };
Bounds sahaBounds = { 20, 570, 20, 370 };
Bounds reasadBounds = { 20, 570, 20, 370 };
Bounds mamunBounds = { 20, 570, 20, 370 };

// iGraphics' iShowImage has no flip argument. This is the same OpenGL draw
// call with the horizontal texture coordinates reversed.
inline void showImageFacingLeft(int x, int y, int width, int height, unsigned int texture) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glBegin(GL_QUADS);
	glTexCoord2f(1, 0);  glVertex2f(x, y);
	glTexCoord2f(0, 0);  glVertex2f(x + width, y);
	glTexCoord2f(0, -1); glVertex2f(x + width, y + height);
	glTexCoord2f(1, -1); glVertex2f(x, y + height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

inline void drawPlayer() {
	int baseX = (int)player.x;
	int baseY = (int)player.y;

	// Idle bob, a bit faster/bigger while walking
	double bobSpeed = playerIsMoving ? 0.6 : 0.15;
	double bobSize = playerIsMoving ? 2.5 : 1.5;
	int bob = (int)(bobSize * sin(animTimer * bobSpeed));

	if (ayonEscortState == AYON_ESCORT_ESCORTING) {
		int ayonX = playerFacingLeft ? baseX + 34 : baseX - 34;
		if (ayonX < 8) ayonX = baseX + 34;
		if (ayonX > SCREEN_WIDTH - 34) ayonX = baseX - 34;
		drawShadow(ayonX + 15, baseY, 30, 8);
		iShowImage(ayonX, baseY + bob, 30, 50, imgNpcSenior);
		iSetColor(255, 250, 225);
		iText(ayonX, baseY + 56, "Ayon", GLUT_BITMAP_HELVETICA_12);
		int nissanX = playerFacingLeft ? baseX + 66 : baseX - 66;
		if (nissanX < 8) nissanX = baseX + 66;
		if (nissanX > SCREEN_WIDTH - 34) nissanX = baseX - 66;
		drawShadow(nissanX + 15, baseY, 30, 8);
		iShowImage(nissanX, baseY + bob, 30, 50, imgNpcClassmate);
		iSetColor(255, 250, 225);
		iText(nissanX - 2, baseY + 56, "Nissan", GLUT_BITMAP_HELVETICA_12);
		int ashrafulX = playerFacingLeft ? baseX + 98 : baseX - 98;
		if (ashrafulX < 8) ashrafulX = baseX + 98;
		if (ashrafulX > SCREEN_WIDTH - 34) ashrafulX = baseX - 98;
		drawShadow(ashrafulX + 15, baseY, 30, 8);
		iShowImage(ashrafulX, baseY + bob, 30, 50, imgNpcWaiter);
		iSetColor(255, 250, 225);
		iText(ashrafulX - 8, baseY + 56, "Ashraful", GLUT_BITMAP_HELVETICA_12);
		int diptaX = playerFacingLeft ? baseX + 130 : baseX - 130;
		if (diptaX < 8) diptaX = baseX + 130;
		if (diptaX > SCREEN_WIDTH - 34) diptaX = baseX - 130;
		drawShadow(diptaX + 15, baseY, 30, 8);
		iShowImage(diptaX, baseY + bob, 30, 50, imgNpcStudent2);
		iSetColor(255, 250, 225);
		iText(diptaX, baseY + 56, "Dipta", GLUT_BITMAP_HELVETICA_12);
	}

	// Ground shadow anchors the sprite to the floor
	drawShadow(baseX + player.w / 2, baseY, player.w, 8);

	// Mirror the sprite while travelling left; restore its normal direction
	// when the player next walks right.
	if (playerFacingLeft)
		showImageFacingLeft(baseX, baseY + bob, player.w, player.h + 10, imgPlayer);
	else
		iShowImage(baseX, baseY + bob, player.w, player.h + 10, imgPlayer);

	iSetColor(255, 250, 225);
	iText(baseX + 2, baseY + player.h + 14, "Ruel", GLUT_BITMAP_HELVETICA_12);
}

#endif