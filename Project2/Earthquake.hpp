#ifndef EARTHQUAKE_H
#define EARTHQUAKE_H

#include <cstdlib>
#include <ctime>
#include <cstring>
#include <string>
#include <vector>
// NOTE: assumes iGraphics.h, GameState.hpp, Audio.hpp, Player.hpp, NPC.hpp
// and Assets.hpp have already been included by iMain.cpp (same convention
// as every other header in this project).

// startNewGame() is defined further down in iMain.cpp. Every header in this
// project is textually pasted into that single .cpp file, so a forward
// declaration here is enough for the call inside answerEarthquakeGameOver()
// below to link correctly - the real body just has to exist somewhere later
// in the same translation unit, which it does.
void startNewGame();

// ============================================================
//  EARTHQUAKE EMERGENCY DRILL
//  Periodically (roughly every 50 seconds of active play - see
//  scheduleNextEarthquake()) an "URGENT ALERT" pops up telling the player
//  to rush to the Quad. Press A within 10 seconds:
//    - success  -> player is pulled to the Quad, a 6-second tremor plays
//                  out safely, then everyone returns to what they were doing.
//    - too slow -> the roof collapses and it's game over; the player can
//                  restart from the main menu.
// ============================================================

enum EarthquakeState {
	EQ_IDLE,
	EQ_WARNING,       // alert overlay + 10s countdown, waiting for the player to press A
	EQ_MOVE_TO_QUAD,  // short "moving to safety" transition after a successful press
	EQ_SHAKING,       // 6s of safe tremor at the Quad (everyone made it)
	EQ_RETURNING,     // short "all clear" transition back to where the player was
	EQ_COLLAPSING,    // roof/debris collapse animation - the player didn't make it in time
	EQ_GAMEOVER       // static game-over screen, waiting for the player to restart
};

EarthquakeState earthquakeState = EQ_IDLE;

// fixedUpdate() in iMain.cpp is called at a steady ~60 ticks/second - the
// same pacing CLASSROOM_UNLOCK_BANNER_DURATION (300 ticks / 5s) relies on.
const int EQ_TICKS_PER_SECOND = 60;

const int EQ_WARNING_DURATION = 10 * EQ_TICKS_PER_SECOND; // 10s to react
const int EQ_MOVE_DURATION = 1 * EQ_TICKS_PER_SECOND; // 1s "moving..." transition
const int EQ_SHAKE_DURATION = 6 * EQ_TICKS_PER_SECOND; // 6s safe tremor at the Quad
const int EQ_RETURN_DURATION = 1 * EQ_TICKS_PER_SECOND; // 1s "all clear" transition
const int EQ_COLLAPSE_DURATION = 4 * EQ_TICKS_PER_SECOND; // 4s collapse animation

int earthquakeStateTimer = 0; // ticks remaining in the current phase

// "Systematic but random" schedule: the next drill lands roughly 50 game-
// seconds after the previous one (so milestones fall near 50, 100, 150...),
// with a small jitter so it never feels like it's on a fixed timer.
bool earthquakeScheduleSeeded = false;
int nextEarthquakeAt = 0;
const int EQ_MILESTONE_SECONDS = 50;
const int EQ_MILESTONE_JITTER = 8; // +/- seconds

// Where the player was, so EQ_RETURNING can put them right back once the
// drill is over.
GameScreen earthquakeReturnScreen = SCREEN_QUAD;
double earthquakeReturnX = 300, earthquakeReturnY = 80;

// Optional generated art (see Images/eq_*.png). If these files haven't been
// added to the project yet, iLoadImage() returns -1 the same way every
// other optional asset in Assets.hpp does, and the drill simply falls back
// to plain vector shapes - nothing here requires the files to be present.
int imgEqCrackOverlay = -1;
int imgEqGameOverBg = -1;
int imgEqWarningIcon = -1;
bool earthquakeAssetsLoaded = false;

inline void loadEarthquakeAssets() {
	if (earthquakeAssetsLoaded) return;
	earthquakeAssetsLoaded = true;
	imgEqCrackOverlay = iLoadImage("Images//eq_crack_overlay.png");
	imgEqGameOverBg = iLoadImage("Images//eq_gameover_bg.png");
	imgEqWarningIcon = iLoadImage("Images//eq_warning_icon.png");
}

inline bool isEarthquakeEligibleScreen(GameScreen s) {
	// Same set of "free roam" screens fixedUpdate() already treats as normal
	// gameplay - deliberately excludes menus, dialogue, quizzes and the
	// splash screen so the drill never interrupts something modal.
	return s == SCREEN_QUAD || s == SCREEN_LIBRARY || s == SCREEN_CAFETERIA ||
		s == SCREEN_CLASSROOM || s == SCREEN_LAB || s == SCREEN_FACULTY ||
		s == SCREEN_TEACHER_ZAHID || s == SCREEN_TEACHER_TOWFIQUE ||
		s == SCREEN_TEACHER_SAHA || s == SCREEN_TEACHER_REASAD || s == SCREEN_TEACHER_MAMUN;
}

inline int eqJitterSeconds() {
	return (rand() % (2 * EQ_MILESTONE_JITTER + 1)) - EQ_MILESTONE_JITTER;
}

inline void scheduleNextEarthquake() {
	// Next milestone lands roughly EQ_MILESTONE_SECONDS further out than
	// "now" (so successive drills fall near 50s, 100s, 150s... of elapsed
	// play), with a few seconds of jitter layered on top.
	nextEarthquakeAt = animTimer + (EQ_MILESTONE_SECONDS + eqJitterSeconds()) * EQ_TICKS_PER_SECOND;
}

inline void seedEarthquakeRandomness() {
	if (earthquakeScheduleSeeded) return;
	earthquakeScheduleSeeded = true;
	srand((unsigned int)time(0));
}

// Call this once actual gameplay begins (new game or a loaded save) rather
// than relying on animTimer, which starts counting the moment the program
// launches - including any time spent sitting on the splash/menu screens.
// Without this, that idle time silently ate into the "50 seconds of active
// play" budget and a drill could fire almost immediately after the player
// actually started playing.
inline void startEarthquakeTimer() {
	seedEarthquakeRandomness();
	earthquakeState = EQ_IDLE;
	earthquakeStateTimer = 0;
	scheduleNextEarthquake();
}

// Called once per fixedUpdate() tick, before any normal movement/interact
// handling. Returns true while the drill owns input for this frame, so
// iMain.cpp's fixedUpdate() should return immediately after calling it.
inline bool updateEarthquake() {
	loadEarthquakeAssets();

	if (earthquakeState == EQ_IDLE) {
		if (isEarthquakeEligibleScreen(currentScreen) && animTimer >= nextEarthquakeAt) {
			earthquakeState = EQ_WARNING;
			earthquakeStateTimer = EQ_WARNING_DURATION;
			earthquakeReturnScreen = currentScreen;
			earthquakeReturnX = player.x;
			earthquakeReturnY = player.y;
		}
		return false; // no drill running - normal gameplay continues untouched
	}

	earthquakeStateTimer--;
	playerIsMoving = false; // nobody strolls casually through an earthquake

	switch (earthquakeState) {
	case EQ_WARNING:
		if (isKeyPressed('a') || isKeyPressed('A')) {
			earthquakeState = EQ_MOVE_TO_QUAD;
			earthquakeStateTimer = EQ_MOVE_DURATION;
		}
		else if (earthquakeStateTimer <= 0) {
			earthquakeState = EQ_COLLAPSING;
			earthquakeStateTimer = EQ_COLLAPSE_DURATION;
		}
		break;

	case EQ_MOVE_TO_QUAD:
		if (earthquakeStateTimer <= 0) {
			currentScreen = SCREEN_QUAD;
			player.x = 288; player.y = 210; // assembly point, clear of quad obstacles
			earthquakeState = EQ_SHAKING;
			earthquakeStateTimer = EQ_SHAKE_DURATION;
		}
		break;

	case EQ_SHAKING:
		if (earthquakeStateTimer <= 0) {
			earthquakeState = EQ_RETURNING;
			earthquakeStateTimer = EQ_RETURN_DURATION;
		}
		break;

	case EQ_RETURNING:
		if (earthquakeStateTimer <= 0) {
			currentScreen = earthquakeReturnScreen;
			player.x = earthquakeReturnX;
			player.y = earthquakeReturnY;
			earthquakeState = EQ_IDLE;
			scheduleNextEarthquake();
		}
		break;

	case EQ_COLLAPSING:
		if (earthquakeStateTimer <= 0) {
			earthquakeState = EQ_GAMEOVER;
			playGameOverSound();
		}
		break;

	case EQ_GAMEOVER:
		if (isKeyPressed(' ') || isKeyPressed(13)) {
			stopGameOverSound();
			startNewGame(); // full restart, as requested - resets the drill timer too
		}
		break;

	default:
		break;
	}

	return true;
}

// Per-frame camera shake offset. iDraw() in iMain.cpp applies this with
// glTranslatef() around the normal scene render so the whole world visibly
// trembles, not just the alert box.
inline void getEarthquakeShakeOffset(double& dx, double& dy) {
	dx = 0; dy = 0;
	if (earthquakeState == EQ_WARNING || earthquakeState == EQ_MOVE_TO_QUAD) {
		dx = (rand() % 7) - 3; dy = (rand() % 7) - 3;
	}
	else if (earthquakeState == EQ_SHAKING) {
		dx = (rand() % 9) - 4; dy = (rand() % 9) - 4;
	}
	else if (earthquakeState == EQ_COLLAPSING) {
		dx = (rand() % 13) - 6; dy = (rand() % 13) - 6;
	}
}

// Every NPC in the school, from every room (quadNPCs excluded - drawQuad()
// already keeps drawing those at their usual spot, so they don't need to
// be duplicated here), is treated as having evacuated to the Quad for the
// drill. Each is drawn at its own real sprite and w/h - the same NPC data
// its home room normally draws - laid out in a grid at the assembly point,
// not a separate small stand-in crowd.
inline std::vector<std::vector<NPC>*>& earthquakeEvacueeGroups() {
	static std::vector<std::vector<NPC>*> groups = {
		&libraryNPCs, &cafeteriaNPCs, &classroomNPCs, &labNPCs, &facultyHallNPCs
	};
	return groups;
}

inline void drawEarthquakeEvacuees() {
	// iGraphics' coordinate system is y-up (0 at the bottom of the screen),
	// so a LOWER y value is what puts something down on the road, and a
	// higher one pushes it up toward the back wall - the previous baseY=258
	// went the wrong way. Keeping this low and close to the same y=80 band
	// the player normally walks the Quad's road at (see startNewGame()/the
	// door-crossing spawns) is what actually reads as "on the road".
	//
	// evacueeScale draws everyone bigger here than their normal in-room
	// size - this only affects how big they look gathered at the Quad, not
	// their size anywhere else in the game.
	const int perRow = 10;
	const int spacingX = 54;   // widened to match the bigger sprites below
	const int rowSpacingY = 30;
	const double evacueeScale = 1.3;
	int startX = 288 - ((perRow - 1) * spacingX) / 2; // centered on the assembly point
	int baseY = 150;

	int index = 0;
	std::vector<std::vector<NPC>*>& groups = earthquakeEvacueeGroups();
	for (unsigned int g = 0; g < groups.size(); g++) {
		std::vector<NPC>& scene = *groups[g];
		for (unsigned int i = 0; i < scene.size(); i++) {
			NPC& npc = scene[i];
			int col = index % perRow;
			int row = index / perRow;
			int jitter = (index % 3) * 4 - 4; // -4/0/+4px so the line isn't a perfectly rigid grid
			int scaledW = (int)(npc.w * evacueeScale);
			int scaledH = (int)(npc.h * evacueeScale);
			int x = startX + col * spacingX - (scaledW - npc.w) / 2; // stay centered on the column
			int y = baseY + row * rowSpacingY + jitter;
			iShowImage(x, y, scaledW, scaledH, *npc.sprite);
			index++;
		}
	}
}

// Matches the reference mockup: dark red header reading "!! URGENT ALERT !!",
// a cream body with the evacuation instructions, a live countdown, and the
// "[A] CONFIRM URGENT MOVE" prompt. The whole box pulses (blink) so it reads
// as "coming and going" the way a real emergency banner would.
inline void drawEarthquakeWarningBox() {
	bool blink = ((animTimer / 15) % 2) == 0; // ~4 pulses/second

	int bx = 68, by = 78, bw = 464, bh = 224;

	iSetColor(20, 20, 20);
	iFilledRectangle(bx - 6, by - 6, bw + 12, bh + 12);

	iSetColor(238, 232, 214);
	iFilledRectangle(bx, by, bw, bh);
	iSetColor(140, 25, 20);
	iRectangle(bx, by, bw, bh);
	iRectangle(bx + 3, by + 3, bw - 6, bh - 6);

	// Header banner - brighter red on the "on" half of the blink
	if (blink) iSetColor(214, 62, 56); else iSetColor(178, 44, 40);
	iFilledRectangle(bx, by + bh - 44, bw, 44);
	iSetColor(255, 238, 228);
	iText(bx + 96, by + bh - 30, "!! URGENT ALERT !!", GLUT_BITMAP_TIMES_ROMAN_24);

	if (imgEqWarningIcon != -1) {
		iShowImage(bx + 10, by + bh - 42, 36, 36, imgEqWarningIcon);
		iShowImage(bx + bw - 46, by + bh - 42, 36, 36, imgEqWarningIcon);
	}

	iSetColor(35, 22, 18);
	iText(bx + 46, by + bh - 76, "A MAJOR EARTHQUAKE IS INITIATING.", GLUT_BITMAP_HELVETICA_18);

	iSetColor(150, 20, 15);
	iText(bx + 60, by + bh - 104, "ALL STUDENTS AND FACULTY:", GLUT_BITMAP_HELVETICA_18);
	if (blink) iSetColor(180, 20, 15); else iSetColor(150, 15, 10);
	iText(bx + 40, by + bh - 130, "FAST MOVE TO THE QUAD", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(35, 22, 18);
	iText(bx + 110, by + bh - 152, "FOR IMMEDIATE SAFETY.", GLUT_BITMAP_HELVETICA_18);

	int secondsLeft = earthquakeStateTimer / EQ_TICKS_PER_SECOND + 1;
	if (secondsLeft > 10) secondsLeft = 10;
	std::string countText = "STATUS URGENT - THIS IS NOT A DRILL   (" + std::to_string(secondsLeft) + "s)";
	iSetColor(90, 60, 45);
	iText(bx + 60, by + 36, const_cast<char*>(countText.c_str()), GLUT_BITMAP_HELVETICA_12);

	iSetColor(20, 20, 20);
	iFilledRectangle(bx + 130, by + 8, 204, 20);
	iSetColor(255, 225, 90);
	iText(bx + 140, by + 12, "[A] CONFIRM URGENT MOVE", GLUT_BITMAP_HELVETICA_12);
}

inline void drawEarthquakeTransitionBanner(const char* text, int r, int g, int b) {
	// Rough width estimate for HELVETICA_12 (~7px/char) plus side padding,
	// with a sensible minimum so short strings still get a decent-sized box.
	int estimatedWidth = (int)strlen(text) * 7 + 28;
	int tw = estimatedWidth > 260 ? estimatedWidth : 260;
	int bx = (SCREEN_WIDTH - tw) / 2;
	iSetColor(20, 20, 20);
	iFilledRectangle(bx, 356, tw, 24);
	iSetColor(r, g, b);
	iText(bx + 14, 362, const_cast<char*>(text), GLUT_BITMAP_HELVETICA_12);
}

inline void drawEarthquakeCollapse() {
	if (imgEqGameOverBg != -1) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgEqGameOverBg);
	}
	else {
		iSetColor(60, 10, 10);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	if (imgEqCrackOverlay != -1) iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgEqCrackOverlay);
	iSetColor(255, 230, 90);
	iText(148, 360, "THE ROOF IS COLLAPSING!", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(230, 200, 190);
	iText(140, 335, "Everyone didn't make it to the Quad in time...", GLUT_BITMAP_HELVETICA_12);
}

inline void drawEarthquakeGameOver() {
	if (imgEqGameOverBg != -1) {
		iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgEqGameOverBg);
	}
	else {
		iSetColor(15, 10, 10);
		iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	}
	iSetColor(205, 35, 30);
	iText(198, 250, "GAME OVER", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(230, 220, 210);
	iText(70, 205, "The building collapsed - nobody reached the Quad in time.", GLUT_BITMAP_HELVETICA_12);
	iSetColor(190, 190, 190);
	iText(150, 160, "Press SPACE to restart from the beginning", GLUT_BITMAP_HELVETICA_12);
}

// Called from iDraw() after the normal screen switch, so it always renders
// on top of whatever room the player is in.
inline void drawEarthquakeOverlay() {
	switch (earthquakeState) {
	case EQ_IDLE:
		return;
	case EQ_WARNING:
		drawEarthquakeWarningBox();
		break;
	case EQ_MOVE_TO_QUAD:
		drawEarthquakeTransitionBanner("Moving to the Quad...", 150, 255, 165);
		break;
	case EQ_SHAKING:
		drawEarthquakeEvacuees();
		drawEarthquakeTransitionBanner("EARTHQUAKE IN PROGRESS - STAY AT THE QUAD", 255, 210, 90);
		break;
	case EQ_RETURNING:
		drawEarthquakeTransitionBanner("ALL CLEAR - RETURNING TO CAMPUS", 150, 255, 165);
		break;
	case EQ_COLLAPSING:
		drawEarthquakeCollapse();
		break;
	case EQ_GAMEOVER:
		drawEarthquakeGameOver();
		break;
	}
}

#endif