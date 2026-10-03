#ifndef UTILS_H
#define UTILS_H

#include <math.h>

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;

// General-purpose distance helper (used for NPC interaction range checks)
inline double getDistance(double x1, double y1, double x2, double y2) {
	double dx = x1 - x2, dy = y1 - y2;
	return sqrt(dx * dx + dy * dy);
}

// ---------------- Shared visual helpers ----------------
// NOTE: these call iGraphics drawing functions, so this header must only
// be included AFTER iGraphics.h in iMain.cpp (it already is - iGraphics.h
// is the very first include there).

// Draws a flattened "pill" shaped shadow under a character's feet.
// Cheap fake-ellipse: a rectangle capped with two circles.
inline void drawShadow(int centerX, int footY, int width, int thickness) {
	int r = thickness / 2;
	if (r < 1) r = 1;
	iSetColor(35, 35, 35);
	iFilledRectangle(centerX - width / 2 + r, footY - r, width - thickness, thickness);
	iFilledCircle(centerX - width / 2 + r, footY, r, 16);
	iFilledCircle(centerX + width / 2 - r, footY, r, 16);
}

// Thin dark frame around the play area edges - makes flat-color screens
// feel deliberately designed instead of default/placeholder.
inline void drawScreenFrame() {
	iSetColor(15, 15, 15);
	iFilledRectangle(0, 0, SCREEN_WIDTH, 6);                  // bottom
	iFilledRectangle(0, SCREEN_HEIGHT - 6, SCREEN_WIDTH, 6);  // top
	iFilledRectangle(0, 0, 6, SCREEN_HEIGHT);                 // left
	iFilledRectangle(SCREEN_WIDTH - 6, 0, 6, SCREEN_HEIGHT);  // right
}

// Scatters small dots across a band as ground texture (grass flecks, dust,
// etc). Deterministic (not rand()-based) so it never flickers frame to frame.
inline void drawGroundTexture(int x, int y, int w, int h, int r, int g, int b, int count) {
	iSetColor(r, g, b);
	for (int i = 0; i < count; i++) {
		int px = x + (i * 37 + 13) % w;
		int py = y + (i * 53 + 7) % h;
		iFilledCircle(px, py, 2, 8);
	}
}

#endif
