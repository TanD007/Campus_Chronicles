#ifndef DRAW_H
#define DRAW_H

#include <string>
#include <math.h>
// NOTE: assumes iGraphics.h has already been included by iMain.cpp.
// Do not #include "iGraphics.h" again here - see Player.hpp for why.
#include "Utils.hpp"
#include "GameState.hpp"
#include "Player.hpp"
#include "NPC.hpp"
#include "Quest.hpp"
#include "Assets.hpp"
#include "Collision.hpp"
#include "Minigame.hpp"

// Cafeteria table positions - shared source of truth for both drawing
// and collision, so they can never drift out of sync.
struct DiningSet { double x, y; };
std::vector<DiningSet> cafeteriaTables = {
	{ 115, 205 }, { 390, 205 },
	{ 115, 70 }, { 390, 70 }
};

// ---------------- HUD ----------------
inline void drawHUD() {
	// Panel shadow + background - moved below the screen title so they
	// don't overlap
	iSetColor(20, 20, 20);
	iFilledRectangle(10, 320, 168, 30);
	iSetColor(245, 245, 235);
	iFilledRectangle(12, 322, 164, 26);
	iSetColor(90, 70, 20);
	iRectangle(12, 322, 164, 26);

	iSetColor(30, 30, 30);
	std::string repText = "Reputation: " + std::to_string(reputationPoints);
	iText(20, 330, const_cast<char*>(repText.c_str()), GLUT_BITMAP_HELVETICA_18);

	// Compact objective strip: shows only the next action, preserving the
	// room view and keeping all gameplay objects unobstructed.
	iSetColor(12, 24, 44);
	iFilledRectangle(12, 300, 164, 16);
	iSetColor(255, 205, 65);
	iFilledRectangle(12, 300, 5, 16);
	iSetColor(235, 245, 250);
	if (takaMissionState == TAKA_MISSION_NOT_STARTED)
		iText(22, 305, "GO: Ashraful / Library", GLUT_BITMAP_HELVETICA_12);
	else if (takaMissionState == TAKA_MISSION_CARRYING_MONEY)
		iText(22, 305, "GO: Dipta / Cafe (5tk)", GLUT_BITMAP_HELVETICA_12);
	else if (missingDataState == MISSING_DATA_ACTIVE) {
		iSetColor(255, 215, 80);
		iText(22, 305, "USB: Search Lockers", GLUT_BITMAP_HELVETICA_12);
	}
	else if (missingDataState == MISSING_DATA_COLLECTED) {
		iSetColor(255, 215, 80);
		iText(22, 305, "USB: Return to Senior", GLUT_BITMAP_HELVETICA_12);
	}
	else if (missingDataState == MISSING_DATA_COMPLETE) {
		iSetColor(130, 255, 165);
		iText(22, 305, "DONE: Missing Data", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iSetColor(130, 255, 165);
		iText(22, 305, "DONE: Five Taka Favor", GLUT_BITMAP_HELVETICA_12);
	}

	// Second, smaller status strip tracking the correlated CSE approval
	// chain (lab diagnostic -> Zahid Hossain -> Towfique -> Saha -> Reasad
	// -> Prof. Al Mamun), drawn just below the taka-mission strip so both
	// objectives stay visible at once.
	iSetColor(20, 35, 20);
	iFilledRectangle(12, 282, 164, 16);
	iSetColor(140, 255, 170);
	iFilledRectangle(12, 282, 5, 16);
	iSetColor(225, 245, 230);
	std::string cseText;
	// State-specific directions: each step names the exact person or object
	// Ruel should visit next, rather than showing a broad mission title.
	if (!waiterQuestionAnswered)
		cseText = "CSE: Talk Waiter (Cafe)";
	else if (!cashierQuestionAnswered)
		cseText = "CSE: Talk Cashier (Cafe)";
	else if (reputationPoints < 25)
		cseText = "CSE: Need " + std::to_string(25 - reputationPoints) + " more REP";
	else if (!nissanConnectionStarted)
		cseText = "CSE: Talk Nissan (Cafe)";
	else if (!quests[CLASSROOM_COURTESY_QUEST_INDEX].isComplete)
		cseText = "CSE: Talk Rafi (Class)";
	else if (!quests[JAVA_ERROR_HUNT_QUEST_INDEX].isComplete)
		cseText = "CSE: Use Study Table";
	else if (ayonEscortState == AYON_ESCORT_NOT_READY)
		cseText = "CSE: Talk Rafi (Class)";
	else if (ayonEscortState == AYON_ESCORT_FIND_AYON)
		cseText = "CSE: Find Ayon (Library)";
	else if (ayonEscortState == AYON_ESCORT_ESCORTING)
		cseText = "CSE: Take Ayon to Class";
	else if (!rafiBullyingResolved)
		cseText = "CSE: Talk Rafi (Class)";
	else if (!quests[NISSAN_MIM_QUEST_INDEX].isComplete)
		cseText = "CSE: Talk Nissan (Class)";
	else if (!quests[3].isComplete) cseText = "CSE: Fix PC (Zahid Sir)";
	else if (!quests[4].isComplete) cseText = "CSE: See Zahid Hossain";
	else if (!quests[5].isComplete) cseText = "CSE: See Towfique Sir";
	else if (!quests[6].isComplete) cseText = "CSE: See Saha Sir";
	else if (!quests[7].isComplete) cseText = "CSE: See Reasad Sir";
	else if (!quests[8].isComplete) cseText = "CSE: See Prof. Al Mamun";
	else                             cseText = "CSE: All Approved!";
	iText(22, 287, const_cast<char*>(cseText.c_str()), GLUT_BITMAP_HELVETICA_12);

	iSetColor(255, 255, 255);
	iText(15, 12, "WASD/Arrows: move   F: interact   ESC: menu   I: objectives", GLUT_BITMAP_HELVETICA_12);
}

// ---------------- Full-screen objectives / instructions popup ----------------
// On-demand overlay (toggle with 'I', handled in iMain.cpp's fixedUpdate())
// that spells out the current objective in full sentences, instead of the
// short codes in the HUD strips. Self-contained here so it doesn't need
// anything added to GameState.hpp.
bool showInstructionsPopup = false;
bool waitForInstructionsKeyRelease = false;
// Set once the classroom-unlock banner below has been triggered, so it
// only ever plays the first time (not on every walk back in).
bool classroomUnlockAnnounced = false;
// animTimer tick at which the unlock banner stops showing. -1 = not active.
// Gameplay is NOT paused while this is up - the player keeps moving and
// can already look around the newly-unlocked room while it fades in view.
int classroomUnlockBannerUntil = -1;
const int CLASSROOM_UNLOCK_BANNER_DURATION = 300; // ~5s at the game's tick rate; tune to taste

// Non-blocking "achievement" banner shown across the top of the Cafeteria
// screen the moment the CSE Classroom unlocks, so the player finds out
// immediately instead of having to walk over to the classroom first.
// Called from drawCafeteria() - see below.
inline void drawClassroomUnlockBanner() {
	if (animTimer > classroomUnlockBannerUntil) return;

	iSetColor(10, 10, 10);
	iFilledRectangle(44, 300, 520, 66);
	iSetColor(255, 210, 90);
	iFilledRectangle(48, 304, 512, 58);
	iSetColor(20, 20, 20);
	iRectangle(48, 304, 512, 58);

	iText(65, 345, "CLASSROOM UNLOCKED!", GLUT_BITMAP_TIMES_ROMAN_24);
	iText(65, 325, "A senior lost their USB drive.", GLUT_BITMAP_HELVETICA_12);
	iText(65, 310, "Talk to the Senior Student in the Quad.", GLUT_BITMAP_HELVETICA_12);
}

// Long-form version of the top HUD strip (taka favor / missing data chain).
inline std::string getObjectiveDetail() {
	if (takaMissionState == TAKA_MISSION_NOT_STARTED)
		return "Find Ashraful in the Library and ask about his 5 taka favor. He'll hand you the money to deliver.";
	else if (takaMissionState == TAKA_MISSION_CARRYING_MONEY)
		return "Carry Ashraful's 5 taka to Dipta in the Cafeteria to finish the favor.";
	else if (missingDataState == MISSING_DATA_ACTIVE)
		return "A senior lost their USB drive with their final project on it. Search near the lockers in the Classroom and press F to pick it up.";
	else if (missingDataState == MISSING_DATA_COLLECTED)
		return "You have the USB drive. Head back to the Quad and return it to the Senior Student.";
	else if (missingDataState == MISSING_DATA_COMPLETE)
		return "You returned the senior's USB drive. That story is finished - focus on the CSE approvals below.";
	else
		return "You helped Dipta with the Five Taka Favor. Keep exploring the campus for your next task.";
}

// Long-form version of the bottom HUD strip (the CSE approval chain).
inline std::string getCseObjectiveDetail() {
	if (!waiterQuestionAnswered)
		return "Speak with the Waiter in the Cafeteria and answer his question correctly.";
	else if (!cashierQuestionAnswered)
		return "Speak with the Cashier in the Cafeteria and help sort out the mixed-up order.";
	else if (reputationPoints < 25)
		return "Build up more reputation by finishing quests before the CSE building will let you in.";
	else if (!nissanConnectionStarted)
		return "Talk to Nissan in the Cafeteria to start the CSE storyline.";
	else if (!quests[CLASSROOM_COURTESY_QUEST_INDEX].isComplete)
		return "Head to the Classroom and speak respectfully with Rafi.";
	else if (!quests[JAVA_ERROR_HUNT_QUEST_INDEX].isComplete)
		return "Use the study table in the Classroom to find and fix the Java errors.";
	else if (ayonEscortState == AYON_ESCORT_NOT_READY)
		return "Talk to Rafi in the Classroom again to continue the story.";
	else if (ayonEscortState == AYON_ESCORT_FIND_AYON)
		return "Find Ayon in the Library and bring him along with you.";
	else if (ayonEscortState == AYON_ESCORT_ESCORTING)
		return "Escort Ayon safely back to the Classroom.";
	else if (!rafiBullyingResolved)
		return "Speak with Rafi in the Classroom to resolve the situation.";
	else if (!quests[NISSAN_MIM_QUEST_INDEX].isComplete)
		return "In the Classroom, introduce Nissan to Mim and guide their conversation.";
	else if (!quests[3].isComplete)
		return "Go to the Lab and use your reserved PC to run the diagnostic mini-game for Zahid Sir.";
	else if (!quests[4].isComplete)
		return "Report back to Zahid Hossain in his office about the lab diagnostic.";
	else if (!quests[5].isComplete)
		return "Show Kazi Towfique Elahi your completed algorithms review.";
	else if (!quests[6].isComplete)
		return "Get your database project signed off by Mr. Saha Reno.";
	else if (!quests[7].isComplete)
		return "Submit your software engineering task to Md Reasad Zaman Chowdhury.";
	else if (!quests[8].isComplete)
		return "Present your full semester progress to Prof. Al Mamun, the department head.";
	else
		return "All CSE approvals are complete! Explore freely or check any remaining side quests.";
}

// Breaks a long sentence into lines no wider than maxChars, so it fits
// cleanly inside the popup box instead of running off the edge.
inline std::vector<std::string> wrapText(const std::string& text, size_t maxChars) {
	std::vector<std::string> lines;
	std::string current, word;
	for (size_t i = 0; i <= text.size(); i++) {
		if (i == text.size() || text[i] == ' ') {
			if (!current.empty() && current.size() + word.size() + 1 > maxChars) {
				lines.push_back(current);
				current = word;
			}
			else {
				if (!current.empty()) current += " ";
				current += word;
			}
			word.clear();
		}
		else word += text[i];
	}
	if (!current.empty()) lines.push_back(current);
	return lines;
}

inline void drawInstructionsPopup() {
	if (!showInstructionsPopup) return;

	iSetColor(8, 16, 34);
	iFilledRectangle(56, 66, 488, 268);
	iSetColor(255, 220, 80);
	iRectangle(56, 66, 488, 268);

	iSetColor(255, 220, 80);
	iText(76, 312, "CURRENT OBJECTIVES", GLUT_BITMAP_TIMES_ROMAN_24);

	iSetColor(140, 255, 170);
	iText(76, 280, "MAIN STORY", GLUT_BITMAP_HELVETICA_12);
	iSetColor(235, 245, 250);
	int y = 262;
	for (const std::string& line : wrapText(getObjectiveDetail(), 60)) {
		iText(76, y, const_cast<char*>(line.c_str()), GLUT_BITMAP_HELVETICA_12);
		y -= 18;
	}

	y -= 14;
	iSetColor(140, 255, 170);
	iText(76, y, "CSE APPROVAL CHAIN", GLUT_BITMAP_HELVETICA_12);
	y -= 18;
	iSetColor(225, 245, 230);
	for (const std::string& line : wrapText(getCseObjectiveDetail(), 60)) {
		iText(76, y, const_cast<char*>(line.c_str()), GLUT_BITMAP_HELVETICA_12);
		y -= 18;
	}

	iSetColor(180, 205, 225);
	iText(76, 84, "Press I to close", GLUT_BITMAP_HELVETICA_12);
}

// ---------------- Drifting sky clouds ----------------
// Each cloud's x is recomputed from animTimer every frame instead of being
// stored and nudged, so the drift is perfectly smooth, never accumulates
// rounding error, and survives a save/load without any extra state.
//
// Clouds move right -> left. Slower clouds read as further away (parallax),
// so the small/high ones are given the lowest speeds. Everything wraps
// around a track wider than the screen, which means a cloud is fully off the
// left edge before it re-enters from the right - no visible pop.
struct SkyCloud {
	int imgIndex;   // index into imgClouds[] in Assets.hpp
	int y, w, h;    // vertical position and on-screen size
	double speed;   // pixels per animTimer tick
	double offset;  // starting point along the wrap track, 0..CLOUD_TRACK
};

// Track is the screen plus a margin at each side, so clouds enter and leave
// off-screen. Keep it wider than (SCREEN_WIDTH + widest cloud + margin).
const double CLOUD_TRACK = SCREEN_WIDTH + 340.0;
const double CLOUD_MARGIN = 170.0;

// Spread along the track so they never bunch up. Sizes keep each PNG's
// aspect ratio; y stays under ~345 to clear the HUD text at the top.
std::vector<SkyCloud> skyClouds = {
	{ 0, 312, 128, 54, 0.22, 0.0 },  // cloud_01 - long low bank
	{ 6, 340, 120, 26, 0.10, 150.0 },  // cloud_07 - thin streaks, high and slow
	{ 1, 308, 112, 55, 0.30, 320.0 },  // cloud_02 - big near cumulus
	{ 8, 344, 46, 23, 0.12, 470.0 },  // cloud_09 - small puff
	{ 3, 328, 92, 36, 0.18, 620.0 },  // cloud_04 - wisp
	{ 4, 296, 84, 45, 0.26, 780.0 }   // cloud_05 - cumulus, lowest and fastest
};

inline void drawSkyClouds() {
	for (unsigned int i = 0; i < skyClouds.size(); i++) {
		const SkyCloud& c = skyClouds[i];
		if (c.imgIndex < 0 || c.imgIndex >= CLOUD_IMAGE_COUNT) continue;
		int handle = imgClouds[c.imgIndex];
		if (handle == -1) continue; // PNG missing - skip instead of crashing

		// Subtracting the clock makes the motion right -> left; fmod keeps
		// the value on the track no matter how long the game has been open.
		double x = c.offset - fmod(c.speed * animTimer, CLOUD_TRACK);
		if (x < 0) x += CLOUD_TRACK;
		x -= CLOUD_MARGIN;

		iShowImage((int)x, c.y, c.w, c.h, handle);
	}
}

// ---------------- Shared campus backdrop ----------------
// The Quad's scenery (sky + drifting clouds, brick wall with two archway
// gates, stone path, grass) is also used behind the main menu and the info
// screens, so it lives here as one function instead of being duplicated.
// Band layout is bottom-to-top because iGraphics' y=0 is the bottom edge.
const int QUAD_GRASS_TOP = 128; // grass covers y 0..128
const int QUAD_PATH_TOP = 192; // path covers y 128..192 (two 32px tile rows)
const int QUAD_WALL_TOP = 288; // wall covers y 192..288 (three tile rows)
const int QUAD_TILE = 32;

inline void drawCampusBackdrop() {
	// Sky
	iSetColor(99, 179, 230);
	iFilledRectangle(0, QUAD_WALL_TOP, SCREEN_WIDTH, SCREEN_HEIGHT - QUAD_WALL_TOP);
	drawSkyClouds();

	// Brick wall, tiled
	for (int ty = QUAD_PATH_TOP; ty < QUAD_WALL_TOP; ty += QUAD_TILE)
	for (int tx = 0; tx < SCREEN_WIDTH; tx += QUAD_TILE)
		iShowImage(tx, ty, QUAD_TILE, QUAD_TILE, imgQuadWall);

	// Stone walking path, tiled (two rows directly below the wall)
	for (int tx = 0; tx < SCREEN_WIDTH; tx += QUAD_TILE)
		iShowImage(tx, QUAD_GRASS_TOP, QUAD_TILE, QUAD_PATH_TOP - QUAD_GRASS_TOP, imgQuadPath);

	// Grass, tiled
	for (int ty = 0; ty < QUAD_GRASS_TOP; ty += QUAD_TILE)
	for (int tx = 0; tx < SCREEN_WIDTH; tx += QUAD_TILE)
		iShowImage(tx, ty, QUAD_TILE, QUAD_TILE, imgQuadGrass);

	// Scattered dirt patches for texture. Deterministic (not rand()-based)
	// so nothing flickers between frames.
	const int dirtSpots[][2] = { { 40, 100 }, { 230, 110 }, { 410, 80 }, { 560, 70 }, { 150, 30 }, { 380, 45 }, { 20, 50 }, { 520, 20 } };
	for (int i = 0; i < 8; i++)
		iShowImage(dirtSpots[i][0], dirtSpots[i][1], 24, 14, imgQuadDirt);

	// Two archway "gates" standing in the wall. Height is trimmed to 124 so
	// the gates peek about 28px above the wall line.
	iShowImage(40, QUAD_PATH_TOP, 96, 124, imgQuadArch);
	iShowImage(464, QUAD_PATH_TOP, 96, 124, imgQuadArch);
}

// ---------------- Menu and information screens ----------------
inline void drawMenuBackground() {
	// Same campus scene as The Quad, so the menu previews the game world
	// instead of showing a separate flat backdrop.
	drawCampusBackdrop();

	// Decorative topiary bushes, kept clear of the button column in the
	// middle of the screen. No collision here - nobody walks on the menu.
	const int bushX[] = { 24, 118, 470, 552 };
	for (int i = 0; i < 4; i++)
		iShowImage(bushX[i], 26, 48, 48, imgQuadBush);
}

// Dark plaque with a thin gold edge - used behind menu text so it stays
// readable over the grass and brickwork.
inline void drawMenuPlaque(int x, int y, int w, int h) {
	iSetColor(10, 18, 34);
	iFilledRectangle(x, y, w, h);
	iSetColor(255, 220, 80);
	iRectangle(x, y, w, h);
}

// ---------------- Main menu buttons ----------------
// One table drives drawing, hover detection and clicking, so a hitbox can
// never end up somewhere other than the button the player can see.
const int MENU_BUTTON_COUNT = 5;
const int MENU_BUTTON_CX = 303;  // horizontal centre of the button column
const int MENU_BUTTON_W = 166;
const int MENU_BUTTON_H = 32;
const int menuButtonY[MENU_BUTTON_COUNT] = { 250, 200, 150, 100, 50 };
const char* menuButtonLabel[MENU_BUTTON_COUNT] = {
	"START GAME", "LOAD GAME", "INSTRUCTIONS", "SETTINGS", "ABOUT US"
};

// Which button the mouse (or the Up/Down arrow keys) is currently on, and
// how far each button has grown toward its highlighted size: 0 = resting,
// 1 = fully highlighted. Eased a little every frame so the button swells
// and fades instead of snapping.
int menuHoverIndex = 0;
double menuButtonGrow[MENU_BUTTON_COUNT] = { 0, 0, 0, 0, 0 };

// Returns the button under a point, or -1. Hitboxes are a little more
// generous than the drawn button so selection near an edge still works.
inline int menuButtonAt(int mx, int my) {
	if (mx < MENU_BUTTON_CX - 103 || mx > MENU_BUTTON_CX + 102) return -1;
	for (int i = 0; i < MENU_BUTTON_COUNT; i++)
	if (my >= menuButtonY[i] - 10 && my <= menuButtonY[i] + MENU_BUTTON_H + 8) return i;
	return -1;
}

inline void drawMenuButton(int index) {
	double g = menuButtonGrow[index];           // 0..1 highlight amount
	int w = MENU_BUTTON_W + (int)(22 * g);      // grows ~13% wider
	int h = MENU_BUTTON_H + (int)(8 * g);
	int x = MENU_BUTTON_CX - w / 2;             // grow outward from the centre
	int y = menuButtonY[index] - (int)(4 * g);

	// Fill fades blue -> white, so the text has to flip to dark to stay
	// readable. Both are straight linear blends on g.
	int fr = (int)(65 + (250 - 65) * g);
	int fg = (int)(115 + (252 - 115) * g);
	int fb = (int)(150 + (255 - 150) * g);
	int tr = (int)(255 + (18 - 255) * g);
	int tg = (int)(220 + (34 - 220) * g);
	int tb = (int)(80 + (60 - 80) * g);

	iSetColor(8, 16, 34);
	iFilledRectangle(x - 2, y - 3, w + 4, h + 6);
	iSetColor(fr, fg, fb);
	iFilledRectangle(x, y, w, h);
	iSetColor(255, 220, 80);
	iRectangle(x, y, w, h);

	// HELVETICA_18 runs about 11px per character - close enough to centre
	// the label inside a button whose width is changing every frame.
	std::string text = menuButtonLabel[index];
	int textW = (int)text.size() * 11;
	iSetColor(tr, tg, tb);
	iText(MENU_BUTTON_CX - textW / 2, y + h / 2 - 6, const_cast<char*>(text.c_str()), GLUT_BITMAP_HELVETICA_18);
}

inline void drawBackButton() {
	iSetColor(8, 16, 34);
	iFilledRectangle(467, 14, 112, 32);
	iSetColor(65, 115, 150);
	iFilledRectangle(469, 16, 108, 28);
	iSetColor(255, 220, 80);
	iRectangle(469, 16, 108, 28);
	iText(493, 25, "BACK", GLUT_BITMAP_HELVETICA_12);
}

inline void drawSplash() {
	iClear();
	// The supplied artwork is 3:2, exactly matching the 600x400 game window.
	iShowImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, imgSplash);

	// Clear instruction for both keyboard and mouse players.
	iSetColor(5, 10, 20);
	iFilledRectangle(155, 12, 290, 30);
	iSetColor(255, 235, 180);
	iText(205, 22, "CLICK OR PRESS ENTER TO CONTINUE", GLUT_BITMAP_HELVETICA_12);
	drawScreenFrame();
}

inline void drawMenu() {
	iClear();
	drawMenuBackground();

	// Title plaque sits in the sky, with the clouds drifting past behind it.
	drawMenuPlaque(100, 306, 400, 80);
	iSetColor(0, 0, 0);
	iText(147, 350, "CAMPUS CHRONICLES", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(255, 220, 80);
	iText(145, 352, "CAMPUS CHRONICLES", GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(195, 220, 240);
	iText(198, 326, "A campus life adventure", GLUT_BITMAP_HELVETICA_12);

	// Ease every button toward its target size. Doing it here (rather than
	// in fixedUpdate) keeps all the menu's animation in one place; the 0.2
	// factor gives a quick but visibly smooth swell.
	for (int i = 0; i < MENU_BUTTON_COUNT; i++) {
		double target = (i == menuHoverIndex) ? 1.0 : 0.0;
		menuButtonGrow[i] += (target - menuButtonGrow[i]) * 0.2;
		if (menuButtonGrow[i] < 0.001) menuButtonGrow[i] = 0.0;
		drawMenuButton(i);
	}

	drawMenuPlaque(88, 10, 424, 26);
	iSetColor(180, 205, 225);
	iText(104, 18, "Mouse or Arrow Keys: select    |    ENTER: confirm", GLUT_BITMAP_HELVETICA_12);
	if (!menuStatus.empty()) {
		drawMenuPlaque(60, 288, 480, 24);
		iSetColor(255, 235, 150);
		iText(72, 295, const_cast<char*>(menuStatus.c_str()), GLUT_BITMAP_HELVETICA_12);
	}
	drawScreenFrame();
}

inline void drawInfoPageFrame(const char* title, const char* subtitle) {
	iClear();
	drawMenuBackground();
	iSetColor(10, 18, 34);
	iFilledRectangle(55, 65, 490, 265);
	iSetColor(87, 147, 180);
	iRectangle(55, 65, 490, 265);
	iSetColor(255, 220, 80);
	iText(75, 290, const_cast<char*>(title), GLUT_BITMAP_TIMES_ROMAN_24);
	iSetColor(205, 225, 235);
	iText(75, 265, const_cast<char*>(subtitle), GLUT_BITMAP_HELVETICA_12);
	drawBackButton();
	drawScreenFrame();
}

inline void drawInstructions() {
	drawInfoPageFrame("INSTRUCTIONS", "Explore campus, help students, and build your reputation.");
	iSetColor(235, 235, 225);
	iText(85, 225, "Move: WASD or Arrow Keys", GLUT_BITMAP_HELVETICA_18);
	iText(85, 195, "Interact: F near NPCs, blackboard & quest items", GLUT_BITMAP_HELVETICA_18);
	iText(85, 165, "Missions: Check classroom blackboard for urgent tasks", GLUT_BITMAP_HELVETICA_18);
	iText(85, 135, "Dialogue: use shown number keys, SPACE to continue", GLUT_BITMAP_HELVETICA_18);
	iText(85, 105, "Save & Load: press F5 while exploring to save", GLUT_BITMAP_HELVETICA_18);
	iText(85, 75, "Objectives: press I any time for full instructions", GLUT_BITMAP_HELVETICA_18);
}

inline void drawSettings() {
	drawInfoPageFrame("GAME SETTINGS", "Choose your preferred options.");
	iSetColor(24, 40, 65);
	iFilledRectangle(180, 175, 240, 50);
	iSetColor(255, 220, 80);
	iRectangle(180, 175, 240, 50);
	std::string musicText = std::string("MUSIC: ") + (menuMusicEnabled ? "ON" : "OFF");
	iText(245, 193, const_cast<char*>(musicText.c_str()), GLUT_BITMAP_HELVETICA_18);
	iSetColor(205, 225, 235);
	iText(180, 145, "Click the button to toggle music.", GLUT_BITMAP_HELVETICA_12);
}

inline void drawAbout() {
	iClear();
	drawMenuBackground();

	// Same composition as the original About Us page: a Credits heading
	// and four member cards in a two-by-two grid, scaled for 600x400.
	iSetColor(8, 16, 34);
	iFilledRectangle(55, 292, 105, 38);
	iSetColor(255, 220, 80);
	iText(70, 306, "CREDITS", GLUT_BITMAP_TIMES_ROMAN_24);

	const int cardX[2] = { 105, 335 };
	const int cardY[2] = { 195, 85 };
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 2; col++) {
			iSetColor(8, 16, 34);
			iFilledRectangle(cardX[col], cardY[row], 180, 78);
			iSetColor(65, 115, 150);
			iRectangle(cardX[col], cardY[row], 180, 78);
		}
	}

	iSetColor(255, 220, 80);
	iText(120, 242, "KISUR AZMAIN NISAN", GLUT_BITMAP_HELVETICA_12);
	iText(120, 217, "ID: 00725105101112", GLUT_BITMAP_HELVETICA_12);
	iText(120, 132, "TOWFIQ AYON", GLUT_BITMAP_HELVETICA_12);
	iText(120, 107, "ID: 00725105101097", GLUT_BITMAP_HELVETICA_12);
	iText(350, 242, "DIPTO DAS TANMOY", GLUT_BITMAP_HELVETICA_12);
	iText(350, 217, "ID: 00725105101117", GLUT_BITMAP_HELVETICA_12);
	iText(350, 132, "MD. ASHRAFUL ISLAM", GLUT_BITMAP_HELVETICA_12);
	iText(350, 107, "ID: 00725105101104", GLUT_BITMAP_HELVETICA_12);

	drawBackButton();
	drawScreenFrame();
}

inline void drawQuad() {
	iClear();

	// Sky + clouds, wall, path, grass and the two archway gates all come
	// from the shared backdrop (see drawCampusBackdrop above); the band
	// heights there are what quadBounds.maxY in Player.hpp is matched to.
	drawCampusBackdrop();

	// Rebuilt every frame from the same numbers used to draw the props below,
	// so a collision box can never drift out of sync with the artwork.
	quadObstacles.clear();

	// Topiary bushes, moved down onto the open grass. Each one is solid -
	// the rect covers the leafy mound only (the sprite has transparent
	// padding around it), so the player walks around instead of through.
	const int bushX[] = { 100, 270, 440 };
	const int bushY = 28;
	for (int i = 0; i < 3; i++) {
		iShowImage(bushX[i], bushY, 48, 48, imgQuadBush);
		quadObstacles.push_back(Rect{ (double)bushX[i] + 6, (double)bushY + 4, 36, 30 });
	}

	iSetColor(0, 0, 0);
	iText(15, 385, "The Quad", GLUT_BITMAP_HELVETICA_18);
	iText(55, 367, "LEFT EXIT: LIBRARY", GLUT_BITMAP_HELVETICA_12);
	iText(420, 367, "RIGHT EXIT: CAFETERIA", GLUT_BITMAP_HELVETICA_12);

	for (unsigned int i = 0; i < quadNPCs.size(); i++) drawNPC(quadNPCs[i]);
	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawScreenFrame();
}

inline void drawLibrary() {
	iClear();

	// Warm timber floor with plank seams gives the library its own identity.
	iSetColor(106, 76, 52);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	for (int y = 0; y < SCREEN_HEIGHT; y += 28) {
		iSetColor(126, 92, 61);
		iFilledRectangle(6, y, SCREEN_WIDTH - 12, 2);
	}
	drawGroundTexture(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 92, 62, 42, 35);

	// Quiet back wall and sign.
	iSetColor(70, 48, 36);
	iFilledRectangle(6, 350, SCREEN_WIDTH - 12, 44);
	iSetColor(185, 151, 95);
	iFilledRectangle(25, 360, 550, 22);
	iSetColor(60, 42, 30);
	iText(245, 367, "CAMPUS LIBRARY", GLUT_BITMAP_HELVETICA_18);

	// Two shelf rows with a generous central aisle. Shelf collision is made
	// from the same positions as the sprites, so shelves cannot be walked through.
	libraryObstacles.clear();
	const int shelfRows[2] = { 100, 205 };
	for (int row = 0; row < 2; row++) {
		for (int bx = 45; bx <= 515; bx += 40) {
			if (bx >= 245 && bx <= 325) continue; // main aisle
			iShowImage(bx, shelfRows[row], 40, 40, imgShelf);
			libraryObstacles.push_back(Rect{ (double)bx, (double)shelfRows[row], 40, 40 });
		}
	}

	// Reading tables at the bottom keep the middle of the room clear.
	const int studyTableX[2] = { 80, 410 };
	for (int i = 0; i < 2; i++) {
		int tx = studyTableX[i];
		drawShadow(tx + 55, 54, 110, 8);
		iSetColor(81, 52, 35);
		iFilledRectangle(tx, 54, 110, 34);
		iSetColor(185, 125, 78);
		iFilledRectangle(tx + 4, 84, 102, 7);
		iSetColor(225, 205, 160);
		iFilledRectangle(tx + 18, 92, 22, 4); // open book
		iFilledRectangle(tx + 70, 92, 22, 4);
		libraryObstacles.push_back(Rect{ (double)tx, 54, 110, 38 });
	}

	// Librarian is rendered before the desk, making them visibly stand behind it.
	drawNPC(libraryNPCs[0]);
	iSetColor(77, 50, 35);
	iFilledRectangle(375, 275, 160, 27);
	iSetColor(191, 131, 79);
	iFilledRectangle(368, 300, 174, 8);
	iSetColor(250, 235, 195);
	// Horizontally centred on the 174-pixel-wide issue desk.
	iText(423, 283, "ISSUE DESK", GLUT_BITMAP_HELVETICA_12);
	libraryObstacles.push_back(Rect{ 368, 275, 174, 33 });

	iSetColor(255, 255, 255);
	iText(15, 385, "Library", GLUT_BITMAP_HELVETICA_18);
	iText(410, 385, "RIGHT EXIT: QUAD", GLUT_BITMAP_HELVETICA_12);

	// The librarian was intentionally drawn behind the issue desk above.
	for (unsigned int i = 1; i < libraryNPCs.size(); i++)
		// Ayon and Ashraful are drawn as Ruel's followers during the escort,
		// so hide their original library placements to prevent duplicates.
	if (!(ayonEscortState == AYON_ESCORT_ESCORTING &&
		(libraryNPCs[i].missionRole == MISSION_ROLE_AYON || libraryNPCs[i].missionRole == MISSION_ROLE_ASHRAFUL))) drawNPC(libraryNPCs[i]);
	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawScreenFrame();
}

inline void drawCafeteria() {
	iClear();

	// Warm tiled floor. Scaling each 16x16 tile evenly to 32x32 preserves
	// the pattern while making it read clearly at this window size.
	int floorTile = 32;
	for (int ty = 0; ty < SCREEN_HEIGHT; ty += floorTile) {
		for (int tx = 0; tx < SCREEN_WIDTH; tx += floorTile) {
			iShowImage(tx, ty, floorTile, floorTile, imgCafFloor);
		}
	}

	// Back wall and a centred serving counter. It sits lower than the wall,
	// leaving room for the cashier and a clear path around the tables.
	iSetColor(76, 49, 36);
	iFilledRectangle(6, 352, SCREEN_WIDTH - 12, 42);
	iSetColor(130, 87, 57);
	iFilledRectangle(18, 358, 564, 26);

	// Draw the cashier before the counter so the counter front hides the
	// lower part of the sprite: visually, the cashier is behind the desk.
	drawNPC(cafeteriaNPCs[3]);

	iSetColor(95, 58, 38);
	iFilledRectangle(195, 255, 210, 23);
	iSetColor(185, 125, 78);
	iFilledRectangle(189, 278, 222, 8);
	iSetColor(66, 42, 30);
	iFilledRectangle(189, 252, 222, 6);
	iSetColor(255, 240, 205);
	iText(249, 263, "CAFETERIA COUNTER", GLUT_BITMAP_HELVETICA_12);

	// The table PNG is a clean standalone object. The supplied stool and
	// bench images are sprite sheets, so they are intentionally not drawn
	// whole here (that was the source of the cluttered food icons).
	cafeteriaObstacles.clear();
	for (unsigned int i = 0; i < cafeteriaTables.size(); i++) {
		double tx = cafeteriaTables[i].x;
		double ty = cafeteriaTables[i].y;

		drawShadow((int)tx + 20, (int)ty, 40, 8);
		iShowImage((int)tx, (int)ty, 40, 56, imgCafTable);

		cafeteriaObstacles.push_back(Rect{ tx, ty, 40, 56 });
	}
	// Only actual furniture blocks the player. The decorative back wall does
	// not need collision because the screen bounds already stop movement.
	cafeteriaObstacles.push_back(Rect{ 189, 252, 222, 34 });

	iSetColor(0, 0, 0);
	iText(15, 385, "Cafeteria", GLUT_BITMAP_HELVETICA_18);
	iText(25, 367, "LEFT EXIT: QUAD", GLUT_BITMAP_HELVETICA_12);
	iText(420, 367, "RIGHT EXIT: CSE CLASS", GLUT_BITMAP_HELVETICA_12);
	if (reputationPoints < 25 || !waiterQuestionAnswered || !cashierQuestionAnswered) {
		iSetColor(255, 235, 125);
		iText(370, 338, "CSE LEVEL LOCKED: 25 REP + waiter + cashier", GLUT_BITMAP_HELVETICA_12);
	}

	// Cashier was drawn before the counter; draw the other characters here.
	for (unsigned int i = 0; i + 1 < cafeteriaNPCs.size(); i++)
	if (!(ayonEscortState == AYON_ESCORT_ESCORTING &&
		(cafeteriaNPCs[i].missionRole == MISSION_ROLE_NISSAN || cafeteriaNPCs[i].missionRole == MISSION_ROLE_DIPTA))) drawNPC(cafeteriaNPCs[i]);
	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawClassroomUnlockBanner();
	drawScreenFrame();
}

inline void drawClassroom() {
	iClear();

	// The fixed HUD occupies the upper-left (x=0..180). The wide upper-right
	// area is intentionally used as the classroom's visible front wall.

	// Blue classroom floor tile across the full play area.
	for (int ty = 0; ty < SCREEN_HEIGHT; ty += 32) {
		for (int tx = 0; tx < SCREEN_WIDTH; tx += 32) {
			iShowImage(tx, ty, 32, 32, imgClassFloor);
		}
	}

	// All collision rectangles below are created from the same coordinates as
	// their corresponding drawing calls, keeping visuals and collision aligned.
	classroomObstacles.clear();

	// FRONT OF CLASS: one centred blackboard and the teacher standing below it.
	// x=220 centres a 160-pixel wide board (classroom-style) in the 600-pixel room.
	const int blackboardX = 220, blackboardY = 340, blackboardW = 160, blackboardH = 42;
	iShowImage(blackboardX, blackboardY, blackboardW, blackboardH, imgClassBlackboard);
	classroomObstacles.push_back(Rect{ (double)blackboardX, (double)blackboardY, blackboardW, blackboardH });

	double px = player.x + player.w / 2.0;
	double py = player.y + player.h / 2.0;
	double bx = blackboardX + blackboardW / 2.0;
	double by = blackboardY + blackboardH / 2.0;
	if (getDistance(px, py, bx, by) <= INTERACT_RADIUS) {
		iSetColor(255, 255, 0);
		iText(blackboardX - 16, blackboardY + blackboardH + 6, "Press F to read Board", GLUT_BITMAP_HELVETICA_12);
	}

	// Classroom lockers against the back wall
	const int lockerX = 505, lockerY = 325, lockerW = 28, lockerH = 44;
	drawShadow(lockerX + 14, lockerY, lockerW, 7);
	iShowImage(lockerX, lockerY, lockerW, lockerH, imgClassLocker);
	classroomObstacles.push_back(Rect{ (double)lockerX, (double)lockerY, (double)lockerW, (double)lockerH });

	// USB drive spawned near locker when "The Missing Data" quest is active
	if (usbItem.isVisible && !usbItem.isCollected) {
		int bob = (int)(2.0 * sin(animTimer * 0.15));
		drawShadow((int)usbItem.x + usbItem.w / 2, (int)usbItem.y, usbItem.w, 5);
		iShowImage((int)usbItem.x, (int)usbItem.y + bob, usbItem.w, usbItem.h, imgUSB);
		double ux = usbItem.x + usbItem.w / 2.0;
		double uy = usbItem.y + usbItem.h / 2.0;
		if (getDistance(px, py, ux, uy) <= INTERACT_RADIUS) {
			iSetColor(255, 255, 0);
			iText((int)usbItem.x - 32, (int)usbItem.y + usbItem.h + 14, "Press F: Collect USB", GLUT_BITMAP_HELVETICA_12);
		}
	}

	// FOUR STUDY TABLES: a compact 2x2 group at the bottom-centre of the room.
	// 205/335 keep the group balanced around x=300; the 70px middle gap and
	// wide outer margins remain walkable. Every table faces the blackboard.
	const int deskRows[2] = { 155, 65 };
	const int deskColumns[2] = { 205, 335 };
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 2; col++) {
			int dx = deskColumns[col];
			int dy = deskRows[row];
			// Every desk uses the same unflipped sprite orientation, aimed toward
			// the front wall/blackboard for a consistent classroom layout.
			drawShadow(dx + 22, dy, 44, 7);
			iShowImage(dx, dy, 44, 44, imgClassDeskChair);
			classroomObstacles.push_back(Rect{ (double)dx, (double)dy, 44, 44 });
		}
	}

	// Personal study table, kept separate from the class groups so the player
	// can immediately recognise the Java Error Hunt interaction point.
	drawShadow(CLASS_PLAYER_DESK_X + 22, CLASS_PLAYER_DESK_Y, 44, 7);
	iShowImage(CLASS_PLAYER_DESK_X, CLASS_PLAYER_DESK_Y, 44, 44, imgClassDeskChair);
	classroomObstacles.push_back(Rect{ (double)CLASS_PLAYER_DESK_X, (double)CLASS_PLAYER_DESK_Y, 44, 44 });
	iSetColor(255, 235, 130);
	iText(CLASS_PLAYER_DESK_X - 10, CLASS_PLAYER_DESK_Y + 50, "YOUR STUDY TABLE", GLUT_BITMAP_HELVETICA_12);
	if (quests[1].isComplete && quests[CLASSROOM_COURTESY_QUEST_INDEX].isComplete &&
		!quests[JAVA_ERROR_HUNT_QUEST_INDEX].isComplete) {
		double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
		if (getDistance(px, py, CLASS_PLAYER_DESK_X + 22, CLASS_PLAYER_DESK_Y + 20) <= INTERACT_RADIUS) {
			iSetColor(255, 255, 0);
			iText(CLASS_PLAYER_DESK_X - 14, CLASS_PLAYER_DESK_Y + 64, "Press F: Java Error Hunt", GLUT_BITMAP_HELVETICA_12);
		}
	}

	// Teacher is placed at the front; students are drawn after their tables so
	// their sprites visibly sit at the bottom-centre study group.
	drawNPC(classroomNPCs[0]);
	for (unsigned int i = 1; i < classroomNPCs.size(); i++)
	if (!(mimLeftClassroom && classroomNPCs[i].missionRole == MISSION_ROLE_MIM)) drawNPC(classroomNPCs[i]);

	// The escort group stays in the room while Ruel resolves the conflict.
	if (ayonEscortState == AYON_ESCORT_AT_CLASSROOM || ayonEscortState == AYON_ESCORT_COMPLETE) {
		// Five-second angry speech cloud: warm cream base, dark-red outline and
		// red text make Hasib Sir's reaction prominent without a dialogue box.
		if (GetTickCount() < teacherAngryUntil) {
			iSetColor(95, 35, 30);
			iFilledCircle(360, 324, 22, 20);
			iFilledCircle(470, 324, 22, 20);
			iFilledRectangle(360, 302, 110, 44);
			iSetColor(255, 244, 220);
			iFilledCircle(360, 326, 19, 20);
			iFilledCircle(470, 326, 19, 20);
			iFilledRectangle(360, 305, 110, 42);
			// Small cloud-tail puffs point back toward the angry teacher.
			iSetColor(255, 244, 220);
			iFilledCircle(348, 307, 7, 16);
			iFilledCircle(340, 299, 4, 16);
			iSetColor(150, 35, 30);
			iText(382, 330, "HASIB SIR!", GLUT_BITMAP_HELVETICA_12);
			iText(366, 313, "You're late! Take your seats!", GLUT_BITMAP_HELVETICA_12);
		}
		drawShadow(245, 205, 30, 8);
		iShowImage(230, 205, 30, 50, imgNpcSenior);
		iSetColor(255, 250, 225); iText(230, 261, "Ayon", GLUT_BITMAP_HELVETICA_12);
		drawShadow(415, 118, 30, 8);
		iShowImage(400, 118, 30, 50, imgNpcClassmate);
		iSetColor(255, 250, 225); iText(397, 174, "Nissan", GLUT_BITMAP_HELVETICA_12);
		drawShadow(205, 142, 30, 8);
		iShowImage(190, 142, 30, 50, imgNpcWaiter);
		iSetColor(255, 250, 225); iText(184, 198, "Ashraful", GLUT_BITMAP_HELVETICA_12);
		// Dipta sits at a matching desk immediately beside Mim's. The desk is
		// kept in the centre group so the right exit path stays completely open.
		// Desk drawn (and collision-registered) first, same as every other
		// classroom desk, so Dipta's sprite sits on top of it instead of the
		// two overlapping, and the player can no longer walk through it.
		drawShadow(467, 155, 44, 7);
		iShowImage(445, 155, 44, 44, imgClassDeskChair);
		classroomObstacles.push_back(Rect{ 445, 155, 44, 44 });
		// Dipta stands below the desk (same offset as Mim/Nissan/Mazid) rather than on it.
		drawShadow(475, 118, 30, 8);
		iShowImage(460, 118, 30, 50, imgNpcStudent2);
		iSetColor(255, 250, 225); iText(462, 174, "Dipta", GLUT_BITMAP_HELVETICA_12);
	}

	// Fixed HUD labels stay clear on the upper-left.
	iSetColor(10, 20, 35);
	iText(15, 385, "CSE Classroom", GLUT_BITMAP_HELVETICA_18);
	iText(25, 367, "LEFT EXIT: CAFETERIA", GLUT_BITMAP_HELVETICA_12);
	iText(420, 367, "RIGHT EXIT: LAB", GLUT_BITMAP_HELVETICA_12);

	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawScreenFrame();
}

// ---------------- Computer Lab ----------------
// No computer/monitor sprite asset is available yet, so each desk is drawn
// as a small composite of primitives (desk, monitor, glowing screen,
// keyboard, chair). Swapping in a real sprite later only means replacing
// the body of this loop - the collision and layout logic stay the same.
inline void drawLab() {
	iClear();

	// Light, low-contrast floor. The old imgClassFloor tile read as a busy
	// dark grid at this room's scale, so the lab now gets its own flat,
	// bright floor instead (kept visually distinct from the classroom).
	iSetColor(214, 224, 230);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	for (int y = 0; y < SCREEN_HEIGHT; y += 40) {
		iSetColor(200, 212, 220);
		iFilledRectangle(0, y, SCREEN_WIDTH, 2);
	}
	drawGroundTexture(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 190, 202, 212, 20);

	labObstacles.clear();

	// FRONT OF ROOM: a compact help counter, with Zahid Sir beside it so he
	// remains fully visible and is easy to approach.
	int deskX = 260, deskY = 310, deskW = 48, deskH = 44;
	drawShadow(deskX + 24, deskY, deskW + 6, 8);
	iShowImage(deskX, deskY, deskW, deskH, imgClassTeacherDesk);
	labObstacles.push_back(Rect{ (double)deskX, (double)deskY, (double)deskW, (double)deskH });

	// A small server tower is kept for atmosphere, but moved out of the
	// walkway and toned down so it no longer reads as a big dark block.
	int rackX = 550, rackY = 335, rackW = 24, rackH = 45;
	iSetColor(110, 115, 125);
	iFilledRectangle(rackX, rackY, rackW, rackH);
	iSetColor(80, 85, 95);
	iRectangle(rackX, rackY, rackW, rackH);
	for (int i = 0; i < 4; i++) {
		iSetColor((i % 2 == 0) ? 90 : 230, (i % 2 == 0) ? 210 : 100, 110);
		iFilledRectangle(rackX + 4, rackY + 6 + i * 9, 10, 4);
	}
	labObstacles.push_back(Rect{ (double)rackX, (double)rackY, (double)rackW, (double)rackH });

	// SIX COMPUTER DESKS: 2 rows x 3 columns, all facing the front of the
	// room, with wide aisles left walkable between and around them. The
	// top-left desk (LAB_PLAYER_DESK_X/Y) is reserved for the player.
	const int deskColumns[3] = { 70, 250, 430 };
	const int deskRows[2] = { 190, 80 };
	for (int row = 0; row < 2; row++) {
		for (int col = 0; col < 3; col++) {
			int dx = deskColumns[col];
			int dy = deskRows[row];
			bool isPlayerDesk = (dx == LAB_PLAYER_DESK_X && dy == LAB_PLAYER_DESK_Y);

			drawShadow(dx + 20, dy - 12, 44, 7);
			// Chair
			iSetColor(45, 45, 50);
			iFilledRectangle(dx + 10, dy - 14, 20, 12);
			// Desk surface
			iSetColor(120, 85, 55);
			iFilledRectangle(dx, dy, 40, 20);
			// Keyboard
			iSetColor(60, 60, 65);
			iFilledRectangle(dx + 6, dy + 15, 28, 6);
			// Monitor body
			iSetColor(30, 30, 35);
			iFilledRectangle(dx + 8, dy + 20, 24, 20);

			if (isPlayerDesk) {
				bool solved = quests[LAB_MINIGAME_QUEST_INDEX].isComplete;
				// Green once fixed, amber while the diagnostic is still pending.
				iSetColor(solved ? 90 : 255, solved ? 230 : 210, solved ? 130 : 60);
				iFilledRectangle(dx + 11, dy + 23, 18, 13);
				iSetColor(255, 235, 150);
				iText(dx - 6, dy + 48, "YOUR PC", GLUT_BITMAP_HELVETICA_12);
				if (!solved) {
					double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
					if (getDistance(px, py, dx + 20, dy + 10) <= INTERACT_RADIUS) {
						iSetColor(255, 255, 0);
						iText(dx - 12, dy + 62, "Press F to run diagnostic", GLUT_BITMAP_HELVETICA_12);
					}
				}
			}
			else {
				// Glowing screen for every other desk.
				iSetColor(80, 190, 230);
				iFilledRectangle(dx + 11, dy + 23, 18, 13);
			}

			labObstacles.push_back(Rect{ (double)dx, (double)(dy - 14), 40, 54 });
		}
	}

	// Keep students beside their monitors rather than directly underneath
	// them, so their full sprites remain visible in the lab scene.
	for (unsigned int i = 1; i < labNPCs.size(); i++) drawNPC(labNPCs[i]);

	// Zahid is rendered last at the open side of the counter, keeping his full
	// sprite visible instead of hiding it behind the desk.
	drawNPC(labNPCs[0]);

	iSetColor(20, 30, 40);
	iText(15, 385, "Computer Lab", GLUT_BITMAP_HELVETICA_18);
	iText(25, 367, "LEFT EXIT: CSE CLASS", GLUT_BITMAP_HELVETICA_12);
	iText(420, 367, "RIGHT EXIT: FACULTY", GLUT_BITMAP_HELVETICA_12);

	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawScreenFrame();
}

// Diagnostic mini-game overlay, opened by pressing F at the reserved PC.
inline void drawLabMinigame() {
	drawLab(); // frozen lab scene behind the question box

	bool showingResults = labMinigameQuestionIndex >= (int)labMinigameQuestions.size();

	iSetColor(10, 10, 10);
	iFilledRectangle(46, 44, 520, 190);
	iSetColor(20, 35, 45);
	iFilledRectangle(40, 50, 520, 190);
	iSetColor(120, 210, 230);
	iRectangle(40, 50, 520, 190);

	if (!showingResults) {
		const MinigameQuestion& q = labMinigameQuestions[labMinigameQuestionIndex];
		iSetColor(255, 230, 150);
		std::string header = "PC Diagnostic (" + std::to_string(labMinigameQuestionIndex + 1) + "/" +
			std::to_string((int)labMinigameQuestions.size()) + ")";
		iText(55, 210, const_cast<char*>(header.c_str()), GLUT_BITMAP_HELVETICA_18);
		iSetColor(230, 230, 230);
		iText(55, 180, const_cast<char*>(q.prompt.c_str()), GLUT_BITMAP_HELVETICA_12);
		for (int i = 0; i < 3; i++) {
			std::string line = std::to_string(i + 1) + ") " + q.options[i];
			iText(55, 150 - i * 25, const_cast<char*>(line.c_str()), GLUT_BITMAP_HELVETICA_12);
		}
		iSetColor(150, 150, 150);
		iText(250, 60, "Press 1, 2, or 3 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		bool passed = labMinigameCorrectCount >= 2;
		iSetColor(255, 230, 150);
		iText(55, 200, "Diagnostic Complete", GLUT_BITMAP_HELVETICA_18);
		iSetColor(230, 230, 230);
		std::string scoreLine = "Score: " + std::to_string(labMinigameCorrectCount) + " / " +
			std::to_string((int)labMinigameQuestions.size());
		iText(55, 165, const_cast<char*>(scoreLine.c_str()), GLUT_BITMAP_HELVETICA_12);
		iSetColor(passed ? 150 : 255, passed ? 230 : 130, passed ? 150 : 130);
		iText(55, 135, const_cast<char*>(passed ? "PC fixed! Zahid Sir will be pleased." : "Not quite - the PC is still acting up. Try again."), GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150);
		iText(250, 60, "Press SPACE to continue", GLUT_BITMAP_HELVETICA_12);
	}
}

inline void drawClassroomQuiz() {
	drawClassroom();
	bool showingResults = classroomQuizQuestionIndex >= (int)classroomQuizQuestions.size();
	iSetColor(10, 10, 10); iFilledRectangle(46, 44, 520, 190);
	iSetColor(35, 25, 55); iFilledRectangle(40, 50, 520, 190);
	iSetColor(225, 180, 100); iRectangle(40, 50, 520, 190);
	if (!showingResults) {
		const MinigameQuestion& q = classroomQuizQuestions[classroomQuizQuestionIndex];
		iSetColor(255, 230, 150);
		std::string header = "Java Error Hunt (" + std::to_string(classroomQuizQuestionIndex + 1) + "/" + std::to_string((int)classroomQuizQuestions.size()) + ")";
		iText(55, 210, const_cast<char*>(header.c_str()), GLUT_BITMAP_HELVETICA_18);
		iSetColor(230, 230, 230); iText(55, 180, const_cast<char*>(q.prompt.c_str()), GLUT_BITMAP_HELVETICA_12);
		for (int i = 0; i < 3; i++) {
			std::string line = std::to_string(i + 1) + ") " + q.options[i];
			iText(55, 150 - i * 25, const_cast<char*>(line.c_str()), GLUT_BITMAP_HELVETICA_12);
		}
		iSetColor(150, 150, 150); iText(250, 60, "Press 1, 2, or 3 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		bool passed = classroomQuizCorrectCount >= 2;
		iSetColor(255, 230, 150); iText(55, 200, "Java Error Hunt Complete", GLUT_BITMAP_HELVETICA_18);
		iSetColor(230, 230, 230);
		std::string score = "Score: " + std::to_string(classroomQuizCorrectCount) + " / " + std::to_string((int)classroomQuizQuestions.size());
		iText(55, 165, const_cast<char*>(score.c_str()), GLUT_BITMAP_HELVETICA_12);
		iSetColor(passed ? 150 : 255, passed ? 230 : 130, passed ? 150 : 130);
		iText(55, 135, const_cast<char*>(passed ? "Great debugging! The Java study task is complete." : "Review the errors and try the study table again."), GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150); iText(250, 60, "Press SPACE to continue", GLUT_BITMAP_HELVETICA_12);
	}
}

// ---------------- Faculty Hall ----------------
// Teachers work directly from compact PC desks in this open hall. It keeps
// the room purposeful and lets the player interact without entering offices.
inline void drawFacultyHub() {
	iClear();

	// Plain warm floor colour keeps this hall calm and visually distinct
	// from the busier rooms around it.
	iSetColor(196, 178, 148);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	drawGroundTexture(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 178, 160, 130, 25);

	facultyObstacles.clear();
	const int stationX[5] = { 188, 308, 428, 248, 368 };
	const int stationY[5] = { 250, 250, 250, 120, 120 };
	for (int i = 0; i < 5; i++) {
		int x = stationX[i], y = stationY[i];
		// Teacher first, then desk: each instructor reads as seated behind a
		// small workstation rather than floating in a large empty room.
		drawNPC(facultyHallNPCs[i]);
		drawShadow(x + 28, y - 9, 56, 7);
		iSetColor(104, 70, 42);
		iFilledRectangle(x, y, 56, 18);
		iSetColor(143, 96, 55);
		iFilledRectangle(x + 3, y + 15, 50, 5);
		iSetColor(35, 40, 45);
		iFilledRectangle(x + 17, y + 20, 24, 18);
		iSetColor(88, 205, 235);
		iFilledRectangle(x + 20, y + 23, 18, 11);
		iSetColor(60, 60, 65);
		iFilledRectangle(x + 13, y + 12, 32, 4);
		facultyObstacles.push_back(Rect{ (double)x, (double)(y - 9), 56, 47 });
	}

	iSetColor(30, 25, 20);
	iText(15, 385, "Faculty Hall", GLUT_BITMAP_HELVETICA_18);
	iText(25, 367, "LEFT EXIT: LAB   |   Talk to teachers at their PCs", GLUT_BITMAP_HELVETICA_12);

	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawScreenFrame();
}

// Shared layout for every teacher's office - a small, quiet room reusing
// existing classroom/library furniture sprites. Wall and speckle colours
// differ per teacher so the five rooms don't all look identical.
inline void drawGenericOfficeRoom(const char* title, NPC& teacher, std::vector<Rect>& obstacles,
	int wallR, int wallG, int wallB,
	int speckR, int speckG, int speckB) {
	iClear();

	iSetColor(wallR, wallG, wallB);
	iFilledRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	drawGroundTexture(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, speckR, speckG, speckB, 25);

	obstacles.clear();

	// FRONT OF ROOM: the teacher's desk.
	int deskX = 260, deskY = 335, deskW = 48, deskH = 44;
	drawShadow(deskX + 24, deskY, deskW + 6, 8);
	iShowImage(deskX, deskY, deskW, deskH, imgClassTeacherDesk);
	obstacles.push_back(Rect{ (double)deskX, (double)deskY, (double)deskW, (double)deskH });

	// A single bookshelf against the right wall.
	int shelfX = 500, shelfY = 300, shelfW = 40, shelfH = 40;
	iShowImage(shelfX, shelfY, shelfW, shelfH, imgShelf);
	obstacles.push_back(Rect{ (double)shelfX, (double)shelfY, (double)shelfW, (double)shelfH });

	// Small visitor chair in front of the desk (decorative only).
	iSetColor(70, 55, 40);
	iFilledRectangle(deskX + 12, deskY - 28, 24, 16);

	drawNPC(teacher);

	iSetColor(255, 255, 255);
	iText(15, 385, const_cast<char*>(title), GLUT_BITMAP_HELVETICA_18);
	iText(25, 367, "LEFT EXIT: FACULTY HALL", GLUT_BITMAP_HELVETICA_12);

	drawPlayer();
	drawHUD();
	drawInstructionsPopup();
	drawScreenFrame();
}

inline void drawZahidOffice()     { drawGenericOfficeRoom("Zahid Hossain's Office", zahidOfficeNPCs[0], zahidOfficeObstacles, 60, 80, 100, 45, 65, 85); }
inline void drawTowfiqueOffice()  { drawGenericOfficeRoom("Kazi Towfique Elahi's Office", towfiqueNPCs[0], towfiqueObstacles, 95, 70, 55, 78, 55, 42); }
inline void drawSahaOffice()      { drawGenericOfficeRoom("Mr. Saha Reno's Office", sahaNPCs[0], sahaObstacles, 55, 90, 78, 40, 75, 62); }
inline void drawReasadOffice()    { drawGenericOfficeRoom("Reasad Chowdhury's Office", reasadNPCs[0], reasadObstacles, 95, 85, 58, 78, 68, 44); }
inline void drawMamunOffice()     { drawGenericOfficeRoom("Prof. Al Mamun - Dept. Head", mamunNPCs[0], mamunObstacles, 45, 48, 70, 32, 34, 55); }

inline void drawDialogue() {
	// Draw the frozen background screen first
	switch (screenBeforeDialogue) {
	case SCREEN_QUAD:      drawQuad();      break;
	case SCREEN_LIBRARY:   drawLibrary();   break;
	case SCREEN_CAFETERIA: drawCafeteria(); break;
	case SCREEN_CLASSROOM: drawClassroom(); break;
	case SCREEN_CLASSROOM_QUIZ: drawClassroomQuiz(); break;
	case SCREEN_LAB:       drawLab();       break;
	case SCREEN_FACULTY:   drawFacultyHub(); break;
	case SCREEN_TEACHER_ZAHID:    drawZahidOffice();    break;
	case SCREEN_TEACHER_TOWFIQUE: drawTowfiqueOffice(); break;
	case SCREEN_TEACHER_SAHA:     drawSahaOffice();     break;
	case SCREEN_TEACHER_REASAD:   drawReasadOffice();   break;
	case SCREEN_TEACHER_MAMUN:    drawMamunOffice();    break;
	default: break;
	}

	// Dialogue box expands for questions so every answer is readable.
	bool isQuestion = activeDialogueKind != DIALOGUE_NORMAL;
	int boxY = isQuestion ? 25 : 40;
	int boxH = isQuestion ? 195 : 120;
	iSetColor(10, 10, 10);
	iFilledRectangle(46, boxY - 6, 520, boxH);
	iSetColor(25, 25, 45);
	iFilledRectangle(40, boxY, 520, boxH);
	iSetColor(255, 210, 90);
	iRectangle(40, boxY, 520, boxH);

	iSetColor(255, 220, 100);
	iText(55, isQuestion ? 190 : 135, const_cast<char*>(activeDialogueName.c_str()), GLUT_BITMAP_HELVETICA_18);
	iSetColor(230, 230, 230);
	iText(55, isQuestion ? 160 : 100, const_cast<char*>(activeDialogueText.c_str()), GLUT_BITMAP_HELVETICA_12);

	if (activeDialogueKind == DIALOGUE_WAITER_QUESTION) {
		iText(55, 130, "1) Sphegetti", GLUT_BITMAP_HELVETICA_12);
		iText(55, 105, "2) Coffee", GLUT_BITMAP_HELVETICA_12);
		iText(55, 80, "3) Lemon Mint", GLUT_BITMAP_HELVETICA_12);
		iText(55, 55, "4) Khamu na kisu, emnei arsi", GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150);
		iText(250, 35, "Press 1, 2, 3, or 4 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else if (activeDialogueKind == DIALOGUE_CASHIER_QUESTION) {
		iText(55, 120, "1) 100 taka", GLUT_BITMAP_HELVETICA_12);
		iText(55, 90, "2) 90 taka", GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150);
		iText(250, 45, "Press 1 or 2 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else if (activeDialogueKind == DIALOGUE_DIPTA_CHOICE) {
		iText(55, 120, "1)Chottraker bacca ekta Ayon", GLUT_BITMAP_HELVETICA_12);
		iText(55, 90, "2) Ei ne tor 5 taka, fokinni. Ayon bollo:Fokinni take 5 taka diye ai", GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150);
		iText(250, 45, "Press 1 or 2 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else if (activeDialogueKind == DIALOGUE_CLASSMATE_CHOICE) {
		iText(55, 120, "1) Sure, I'd be happy to study with you.", GLUT_BITMAP_HELVETICA_12);
		iText(55, 90, "2) Go away, I don't want to help.", GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150);
		iText(250, 45, "Press 1 or 2 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else if (activeDialogueKind == DIALOGUE_RAFI_BULLY_CHOICE) {
		iText(55, 120, "1) Rafi, stop. Ayon deserves respect and support.", GLUT_BITMAP_HELVETICA_12);
		iText(55, 90, "2) Ignore it and let Rafi continue.", GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150); iText(250, 45, "Press 1 or 2 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else if (activeDialogueKind == DIALOGUE_NISSAN_MIM_CHOICE) {
		iText(55, 120, "1) Mim, your hard work is inspiring. Would you like to study together?", GLUT_BITMAP_HELVETICA_12);
		iText(55, 90, "2) You should stop studying and pay attention to me.", GLUT_BITMAP_HELVETICA_12);
		iSetColor(150, 150, 150); iText(250, 45, "Press 1 or 2 to answer", GLUT_BITMAP_HELVETICA_12);
	}
	else {
		iSetColor(150, 150, 150);
		iText(55, 55, "Press SPACE to close", GLUT_BITMAP_HELVETICA_12);
	}
}

#endif