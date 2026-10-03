#ifndef COLLISION_H
#define COLLISION_H

#include <vector>

// A simple axis-aligned box used for solid props the player can't walk
// through (tables, counters, etc).
struct Rect { double x, y, w, h; };

inline bool rectsOverlap(const Rect& a, const Rect& b) {
	return a.x < b.x + b.w && a.x + a.w > b.x &&
	       a.y < b.y + b.h && a.y + a.h > b.y;
}

// One obstacle list per screen. Populated in Draw.hpp so the collision
// box always matches what's actually drawn - single source of truth.
std::vector<Rect> quadObstacles;
std::vector<Rect> libraryObstacles;
std::vector<Rect> cafeteriaObstacles;
std::vector<Rect> classroomObstacles;
std::vector<Rect> labObstacles;
std::vector<Rect> facultyObstacles;
std::vector<Rect> zahidOfficeObstacles;
std::vector<Rect> towfiqueObstacles;
std::vector<Rect> sahaObstacles;
std::vector<Rect> reasadObstacles;
std::vector<Rect> mamunObstacles;

#endif
