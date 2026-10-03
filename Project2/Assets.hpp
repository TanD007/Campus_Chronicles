#ifndef ASSETS_H
#define ASSETS_H

// NOTE: assumes iGraphics.h has already been included by iMain.cpp.
// Do not #include "iGraphics.h" again here - see Player.hpp for why.

// Image handles - loaded once by loadAssets() in main(), used everywhere
// in Draw.hpp / Player.hpp / NPC.hpp. -1 means "not loaded yet".
int imgTree = -1;
// Four dedicated Quad tree variations supplied with the project.
int imgQuadTree01 = -1;
int imgQuadTree02 = -1;
int imgQuadTree03 = -1;
int imgQuadTree04 = -1;
int imgGrass = -1;
int imgShelf = -1;
int imgFloorAccent = -1;
int imgSplash = -1;

// Sky decoration for the Quad. Drawn as images (see drawSkyClouds() in
// Draw.hpp) rather than plain rectangles, so the cloud artwork lives in
// files. All twelve are transparent PNGs in the Images folder; -1 means a
// file is missing, and drawSkyClouds() simply skips those.
const int CLOUD_IMAGE_COUNT = 12;
int imgClouds[CLOUD_IMAGE_COUNT] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };

// Redesigned Quad screen tiles/sprites (stone wall + archway gates,
// cobblestone path, dirt patches, topiary bushes). See drawQuad() in
// Draw.hpp for how these are laid out.
int imgQuadGrass = -1;
int imgQuadPath = -1;
int imgQuadWall = -1;
int imgQuadDirt = -1;
int imgQuadBush = -1;
int imgQuadArch = -1;

// Character sprites (16x32 native size, drawn scaled up via iShowImage)
int imgPlayer = -1;
int imgNpcSenior = -1;
int imgNpcLibrarian = -1;
int imgNpcClassmate = -1;
int imgNpcWaiter = -1;
int imgNpcStudent2 = -1;
int imgNpcCashier = -1;

// Cafeteria props
int imgCafTable = -1;
int imgCafStool = -1;
int imgCafBench = -1;
int imgCafFloor = -1;

// CSE Classroom props
int imgClassFloor = -1;
int imgClassBlackboard = -1;
int imgClassEasel = -1;
int imgClassDeskChair = -1;
int imgClassGlobe = -1;
int imgClassLocker = -1;
int imgClassTeacherDesk = -1;
int imgUSB = -1;

// Call this once from main(), AFTER iInitialize() and BEFORE iStart() -
// same pattern as initBalloons() in the balloon project.
inline void loadAssets() {
	imgTree = iLoadImage("Images//tile_0004.png");
	imgQuadTree01 = iLoadImage("Images//tree_01.png");
	imgQuadTree02 = iLoadImage("Images//tree_02.png");
	imgQuadTree03 = iLoadImage("Images//tree_03.png");
	imgQuadTree04 = iLoadImage("Images//tree_04.png");
	imgGrass = iLoadImage("Images//tile_0000.png");
	imgShelf = iLoadImage("Images//tile_0072.png");
	imgFloorAccent = iLoadImage("Images//tile_0025.png");
	imgSplash = iLoadImage("Images//campus_chronicles_splash.png");

	imgClouds[0] = iLoadImage("Images//cloud_01.png");
	imgClouds[1] = iLoadImage("Images//cloud_02.png");
	imgClouds[2] = iLoadImage("Images//cloud_03.png");
	imgClouds[3] = iLoadImage("Images//cloud_04.png");
	imgClouds[4] = iLoadImage("Images//cloud_05.png");
	imgClouds[5] = iLoadImage("Images//cloud_06.png");
	imgClouds[6] = iLoadImage("Images//cloud_07.png");
	imgClouds[7] = iLoadImage("Images//cloud_08.png");
	imgClouds[8] = iLoadImage("Images//cloud_09.png");
	imgClouds[9] = iLoadImage("Images//cloud_10.png");
	imgClouds[10] = iLoadImage("Images//cloud_11.png");
	imgClouds[11] = iLoadImage("Images//cloud_12.png");

	imgQuadGrass = iLoadImage("Images//quad_grass.png");
	imgQuadPath = iLoadImage("Images//quad_path.png");
	imgQuadWall = iLoadImage("Images//quad_wall.png");
	imgQuadDirt = iLoadImage("Images//quad_dirt.png");
	imgQuadBush = iLoadImage("Images//quad_bush.png");
	imgQuadArch = iLoadImage("Images//quad_arch.png");

	imgPlayer = iLoadImage("Images//player_front.png");
	imgNpcSenior = iLoadImage("Images//npc_senior.png");
	imgNpcLibrarian = iLoadImage("Images//npc_librarian.png");
	imgNpcClassmate = iLoadImage("Images//npc_classmate.png");
	imgNpcWaiter = iLoadImage("Images//npc_waiter.png");
	imgNpcStudent2 = iLoadImage("Images//npc_student2.png");
	imgNpcCashier = iLoadImage("Images//npc_cashier.png");

	imgCafTable = iLoadImage("Images//cafeteria_table.png");
	imgCafStool = iLoadImage("Images//cafeteria_stool.png");
	imgCafBench = iLoadImage("Images//cafeteria_bench.png");
	imgCafFloor = iLoadImage("Images//cafeteria_floor.png");

	imgClassFloor = iLoadImage("Images//classroom_floor.png");
	imgClassBlackboard = iLoadImage("Images//classroom_blackboard.png");
	imgClassEasel = iLoadImage("Images//classroom_easel.png");
	imgClassDeskChair = iLoadImage("Images//classroom_desk_chair.png");
	imgClassGlobe = iLoadImage("Images//classroom_globe1.png");
	imgClassLocker = iLoadImage("Images//classroom_locker.png");
	imgClassTeacherDesk = iLoadImage("Images//classroom_teacher_desk.png");
	imgUSB = iLoadImage("Images//USB.PNG");
}

#endif