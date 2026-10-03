#include "iGraphics.h"
#include "Utils.hpp"
#include "Audio.hpp"
#include "GameState.hpp"
#include "Assets.hpp"
#include "Player.hpp"
#include "Quest.hpp"
#include "NPC.hpp"
#include "Earthquake.hpp"
#include "Minigame.hpp"
#include "Draw.hpp"
#include <fstream>

// ============================================================
//  CAMPUS CHRONICLES - starter skeleton
//  Screens-per-location approach (no camera/scrolling needed)
// ============================================================

const char* SAVE_FILE_NAME = "campus_chronicles_save.txt";

// iGraphics keeps drawing in a 600x400 logical coordinate system even when
// the window is enlarged. Convert physical mouse coordinates back to it.
int gameMouseX(int mouseX)
{
	int windowWidth = glutGet(GLUT_WINDOW_WIDTH);
	return windowWidth > 0 ? mouseX * SCREEN_WIDTH / windowWidth : mouseX;
}

int gameMouseY(int mouseY)
{
	int windowHeight = glutGet(GLUT_WINDOW_HEIGHT);
	if (windowHeight <= 0) return mouseY;

	// iGraphics has already flipped Y using its original 400-pixel height:
	// receivedY = 400 - physicalY. Recover physicalY first, then convert it
	// to our 400-pixel logical world. This is necessary after fullscreen.
	int physicalY = SCREEN_HEIGHT - mouseY;
	return SCREEN_HEIGHT - physicalY * SCREEN_HEIGHT / windowHeight;
}

void startNewGame()
{
	stopBackgroundMusic();
	reputationPoints = 0;
	takaMissionState = TAKA_MISSION_NOT_STARTED;
	takaCarried = 0;
	waiterQuestionAnswered = false;
	cashierQuestionAnswered = false;
	labMinigameState = LAB_MINIGAME_NOT_STARTED;
	labMinigameQuestionIndex = 0;
	labMinigameCorrectCount = 0;
	labMinigameWaitForKeyRelease = false;
	classroomQuizState = CLASSROOM_QUIZ_NOT_STARTED;
	classroomQuizQuestionIndex = 0;
	classroomQuizCorrectCount = 0;
	classroomQuizWaitForKeyRelease = false;
	nissanConnectionStarted = false;
	rafiBullyingResolved = false;
	mimLeftClassroom = false;
	ayonEscortState = AYON_ESCORT_NOT_READY;
	teacherAngryUntil = 0;
	for (unsigned int i = 0; i < quests.size(); i++) quests[i].isComplete = false;
	currentMissionIndex = 0;
	missingDataState = MISSING_DATA_LOCKED;
	usbItem.isVisible = false;
	usbItem.isCollected = false;
	classroomUnlockAnnounced = false;
	classroomUnlockBannerUntil = -1;
	// y=80 keeps the player on the grass/path band of the redesigned Quad
	// (its walkable ceiling is now 160, not the old 370 - see quadBounds).
	player.x = 300; player.y = 80;
	menuStatus.clear();
	currentScreen = SCREEN_QUAD;
	// The earthquake drill's ~50s countdown should be measured from when
	// gameplay actually begins, not from when the executable launched (see
	// Earthquake.hpp for why that matters).
	startEarthquakeTimer();
}

void saveGame()
{
	std::ofstream saveFile(SAVE_FILE_NAME);
	if (!saveFile) return;
	saveFile << player.x << ' ' << player.y << ' ' << reputationPoints << ' '
		<< (int)takaMissionState << ' ' << takaCarried << ' '
		<< waiterQuestionAnswered << ' ' << cashierQuestionAnswered << '\n';
	for (unsigned int i = 0; i < quests.size(); i++) saveFile << quests[i].isComplete << ' ';
	saveFile << '\n' << currentMissionIndex << ' ' << (int)missingDataState << ' '
		<< (usbItem.isVisible ? 1 : 0) << ' ' << (usbItem.isCollected ? 1 : 0) << ' '
		<< (classroomUnlockAnnounced ? 1 : 0) << '\n';
}

bool loadGame()
{
	std::ifstream saveFile(SAVE_FILE_NAME);
	int missionValue, waiterAnswered, cashierAnswered;
	if (!saveFile || !(saveFile >> player.x >> player.y >> reputationPoints >> missionValue >> takaCarried
		>> waiterAnswered >> cashierAnswered)) return false;

	takaMissionState = (TakaMissionState)missionValue;
	waiterQuestionAnswered = waiterAnswered != 0;
	cashierQuestionAnswered = cashierAnswered != 0;
	for (unsigned int i = 0; i < quests.size(); i++) {
		int isComplete;
		// Older saves predate the classroom missions. Preserve their existing
		// progress and initialise newly added quests as incomplete.
		if (!(saveFile >> isComplete)) {
			quests[i].isComplete = false;
			continue;
		}
		quests[i].isComplete = isComplete != 0;
	}

	int mIndex = 0, mdState = 0, usbVis = 0, usbCol = 0, classroomAnnounced = 0;
	if (saveFile >> mIndex >> mdState >> usbVis >> usbCol) {
		currentMissionIndex = mIndex;
		missingDataState = (MissingDataMissionState)mdState;
		usbItem.isVisible = (usbVis != 0);
		usbItem.isCollected = (usbCol != 0);
		if (saveFile >> classroomAnnounced) {
			classroomUnlockAnnounced = classroomAnnounced != 0;
		}
		else {
			// Older save, predates this field. If progress already meets the
			// unlock requirement, treat it as already announced so the
			// banner doesn't unexpectedly replay on an old save.
			classroomUnlockAnnounced = (reputationPoints >= 25 && waiterQuestionAnswered && cashierQuestionAnswered);
		}
	}
	else {
		updateMissionTracker();
		if (quests[MISSING_DATA_QUEST_INDEX].isComplete) {
			missingDataState = MISSING_DATA_COMPLETE;
			usbItem.isVisible = false;
			usbItem.isCollected = true;
		}
		else {
			missingDataState = areMissingDataPrerequisitesComplete() ? MISSING_DATA_AVAILABLE : MISSING_DATA_LOCKED;
			usbItem.isVisible = false;
			usbItem.isCollected = false;
		}
		classroomUnlockAnnounced = (reputationPoints >= 25 && waiterQuestionAnswered && cashierQuestionAnswered);
	}
	classroomUnlockBannerUntil = -1; // never replay the banner immediately after loading

	menuStatus.clear();
	// These are transient conversation states. If a saved quest is unfinished,
	// the relevant NPC simply gives the request again after loading.
	nissanConnectionStarted = false;
	rafiBullyingResolved = false;
	mimLeftClassroom = false;
	ayonEscortState = AYON_ESCORT_NOT_READY;
	teacherAngryUntil = 0;
	stopBackgroundMusic();
	currentScreen = SCREEN_QUAD;
	// Same reasoning as in startNewGame(): start the drill countdown from
	// this moment, not from whenever the app happened to launch.
	startEarthquakeTimer();
	return true;
}

// One place that decides what each menu entry does, shared by the mouse
// click handler and the Enter key.
inline void activateMenuButton(int index) {
	switch (index) {
	case 0: startNewGame(); break;
	case 1:
		if (!loadGame()) menuStatus = "No saved game found. Start a new game, then press F5 to save.";
		break;
	case 2: currentScreen = SCREEN_INSTRUCTIONS; break;
	case 3: currentScreen = SCREEN_SETTINGS; break;
	case 4: currentScreen = SCREEN_ABOUT; break;
	}
}

void iDraw()
{
	// Earthquake camera shake: nudges the whole scene by a few pixels each
	// frame while a drill is in progress (see Earthquake.hpp). glPushMatrix/
	// glPopMatrix keep this from leaking into anything drawn afterwards.
	double eqShakeX = 0, eqShakeY = 0;
	getEarthquakeShakeOffset(eqShakeX, eqShakeY);
	glPushMatrix();
	glTranslatef((float)eqShakeX, (float)eqShakeY, 0.0f);

	switch (currentScreen) {
	case SCREEN_SPLASH:     drawSplash();    break;
	case SCREEN_MENU:      drawMenu();      break;
	case SCREEN_QUAD:      drawQuad();      break;
	case SCREEN_LIBRARY:   drawLibrary();   break;
	case SCREEN_CAFETERIA: drawCafeteria(); break;
	case SCREEN_CLASSROOM: drawClassroom(); break;
	case SCREEN_CLASSROOM_QUIZ: drawClassroomQuiz(); break;
	case SCREEN_LAB:       drawLab();       break;
	case SCREEN_LAB_MINIGAME: drawLabMinigame(); break;
	case SCREEN_FACULTY:   drawFacultyHub(); break;
	case SCREEN_TEACHER_ZAHID:    drawZahidOffice();    break;
	case SCREEN_TEACHER_TOWFIQUE: drawTowfiqueOffice(); break;
	case SCREEN_TEACHER_SAHA:     drawSahaOffice();     break;
	case SCREEN_TEACHER_REASAD:   drawReasadOffice();   break;
	case SCREEN_TEACHER_MAMUN:    drawMamunOffice();    break;
	case SCREEN_INSTRUCTIONS: drawInstructions(); break;
	case SCREEN_SETTINGS:     drawSettings();     break;
	case SCREEN_ABOUT:        drawAbout();        break;
	case SCREEN_DIALOGUE:  drawDialogue();  break;
	}

	glPopMatrix();

	// Alert box / debris / game-over screen are drawn last, on top of
	// everything and unaffected by the shake offset above.
	drawEarthquakeOverlay();
}

// Both mouse-move callbacks feed the same highlight, so the menu button
// under the cursor lights up whether or not a button is being held down.
inline void updateMenuHover(int mx, int my) {
	if (currentScreen != SCREEN_MENU) return;
	int hit = menuButtonAt(gameMouseX(mx), gameMouseY(my));
	// Moving off the buttons keeps the last one highlighted rather than
	// clearing it, so the arrow keys and the mouse never fight each other.
	if (hit != -1) menuHoverIndex = hit;
}

void iMouseMove(int mx, int my) { updateMenuHover(mx, my); }
void iPassiveMouseMove(int mx, int my) { updateMenuHover(mx, my); }

void iMouse(int button, int state, int mx, int my)
{
	if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;
	if (currentScreen == SCREEN_SPLASH) {
		currentScreen = SCREEN_MENU;
		waitForSplashKeyRelease = false;
		return;
	}
	// Fullscreen/enlarged windows report physical pixels; menu drawings use
	// SCREEN_WIDTH/SCREEN_HEIGHT, so use matching scaled coordinates here.
	mx = gameMouseX(mx);
	my = gameMouseY(my);

	if (currentScreen == SCREEN_MENU) {
		// menuButtonAt() (Draw.hpp) owns the hitboxes, so a click always
		// lands on exactly the button the highlight is showing.
		int hit = menuButtonAt(mx, my);
		if (hit != -1) { menuHoverIndex = hit; activateMenuButton(hit); }
		return;
	}

	if (currentScreen == SCREEN_SETTINGS && mx >= 160 && mx <= 440 && my >= 160 && my <= 240) {
		menuMusicEnabled = !menuMusicEnabled;
		if (menuMusicEnabled) playBackgroundMusic(); else stopBackgroundMusic();
		return;
	}

	if ((currentScreen == SCREEN_INSTRUCTIONS || currentScreen == SCREEN_SETTINGS || currentScreen == SCREEN_ABOUT) &&
		mx >= 450 && mx <= 590 && my >= 5 && my <= 55) {
		currentScreen = SCREEN_MENU;
	}
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {}
}

// NOTE: all key handling now lives in fixedUpdate() via isKeyPressed() polling,
// since that path is confirmed working in this project. This stub is kept only
// in case your iGraphics.h expects an iKeyboard function to exist.
void iKeyboard(unsigned char key)
{
}

void fixedUpdate()
{
	animTimer++; // drives idle bob / walk-cycle / glow-pulse animation everywhere

	// Earthquake drill takes over input/movement completely while active
	// (see Earthquake.hpp) - it only ever starts during normal free-roam
	// screens, so this is a no-op the rest of the time.
	if (updateEarthquake()) return;

	if (currentScreen == SCREEN_SPLASH) {
		if (isKeyPressed(13) || isKeyPressed(' ')) {
			currentScreen = SCREEN_MENU;
			waitForSplashKeyRelease = true;
		}
		return;
	}

	// Menu -> Quad
	if (currentScreen == SCREEN_MENU) {
		if (waitForSplashKeyRelease) {
			if (!isKeyPressed(13) && !isKeyPressed(' ')) waitForSplashKeyRelease = false;
			return;
		}

		// Up/Down move the highlight; the button grows and turns white in
		// drawMenu(). The release flag stops one long key press from running
		// through the whole list in a few frames.
		bool up = isSpecialKeyPressed(GLUT_KEY_UP) || isKeyPressed('w') || isKeyPressed('W');
		bool down = isSpecialKeyPressed(GLUT_KEY_DOWN) || isKeyPressed('s') || isKeyPressed('S');
		if (!up && !down) menuWaitForArrowRelease = false;
		else if (!menuWaitForArrowRelease) {
			if (up) menuHoverIndex--;
			if (down) menuHoverIndex++;
			if (menuHoverIndex < 0) menuHoverIndex = MENU_BUTTON_COUNT - 1;
			if (menuHoverIndex >= MENU_BUTTON_COUNT) menuHoverIndex = 0;
			menuWaitForArrowRelease = true;
		}

		if (isKeyPressed(13) || isKeyPressed(' ')) { // Enter or Space
			activateMenuButton(menuHoverIndex);
		}
		return;
	}

	if (currentScreen == SCREEN_INSTRUCTIONS || currentScreen == SCREEN_SETTINGS || currentScreen == SCREEN_ABOUT) {
		if (isKeyPressed(27) || isKeyPressed('b') || isKeyPressed('B')) currentScreen = SCREEN_MENU;
		return;
	}

	// Close dialogue
	if (currentScreen == SCREEN_DIALOGUE) {
		if (isKeyPressed(27)) { // Esc returns safely to the main menu
			activeNPC = nullptr;
			currentScreen = SCREEN_MENU;
			return;
		}
		if (activeDialogueKind != DIALOGUE_NORMAL) {
			if (isKeyPressed('1')) answerDialogueQuestion(1);
			else if (isKeyPressed('2')) answerDialogueQuestion(2);
			else if (activeDialogueKind == DIALOGUE_WAITER_QUESTION && isKeyPressed('3')) answerDialogueQuestion(3);
			else if (activeDialogueKind == DIALOGUE_WAITER_QUESTION && isKeyPressed('4')) answerDialogueQuestion(4);
			return;
		}
		if (isKeyPressed(' ')) {
			currentScreen = screenBeforeDialogue;
			activeNPC = nullptr;
		}
		return;
	}

	// Lab PC diagnostic mini-game
	if (currentScreen == SCREEN_LAB_MINIGAME) {
		if (labMinigameState != LAB_MINIGAME_IN_PROGRESS) { currentScreen = SCREEN_LAB; return; }

		if (labMinigameQuestionIndex < (int)labMinigameQuestions.size()) {
			// Debounce: require the answer key to be released before the next
			// press counts, so holding it down can't blow through every question.
			if (labMinigameWaitForKeyRelease) {
				if (!isKeyPressed('1') && !isKeyPressed('2') && !isKeyPressed('3'))
					labMinigameWaitForKeyRelease = false;
				return;
			}
			int choice = 0;
			if (isKeyPressed('1')) choice = 1;
			else if (isKeyPressed('2')) choice = 2;
			else if (isKeyPressed('3')) choice = 3;
			if (choice != 0) {
				answerLabMinigame(choice);
				labMinigameWaitForKeyRelease = true;
			}
		}
		else if (isKeyPressed(' ')) {
			finishLabMinigame();
			currentScreen = SCREEN_LAB;
		}
		return;
	}

	if (currentScreen == SCREEN_CLASSROOM_QUIZ) {
		if (classroomQuizState != CLASSROOM_QUIZ_IN_PROGRESS) { currentScreen = SCREEN_CLASSROOM; return; }
		if (classroomQuizQuestionIndex < (int)classroomQuizQuestions.size()) {
			if (classroomQuizWaitForKeyRelease) {
				if (!isKeyPressed('1') && !isKeyPressed('2') && !isKeyPressed('3')) classroomQuizWaitForKeyRelease = false;
				return;
			}
			int choice = 0;
			if (isKeyPressed('1')) choice = 1;
			else if (isKeyPressed('2')) choice = 2;
			else if (isKeyPressed('3')) choice = 3;
			if (choice != 0) { answerClassroomQuiz(choice); classroomQuizWaitForKeyRelease = true; }
		}
		else if (isKeyPressed(' ')) { finishClassroomQuiz(); currentScreen = SCREEN_CLASSROOM; }
		return;
	}

	if (currentScreen != SCREEN_QUAD && currentScreen != SCREEN_LIBRARY && currentScreen != SCREEN_CAFETERIA &&
		currentScreen != SCREEN_CLASSROOM && currentScreen != SCREEN_LAB && currentScreen != SCREEN_FACULTY &&
		currentScreen != SCREEN_TEACHER_ZAHID && currentScreen != SCREEN_TEACHER_TOWFIQUE &&
		currentScreen != SCREEN_TEACHER_SAHA && currentScreen != SCREEN_TEACHER_REASAD && currentScreen != SCREEN_TEACHER_MAMUN)
		return; // don't move player on menu/dialogue screens

	// Return to the menu without resetting progress. Load Game can restore
	// the current session after the player saves with F5.
	if (isKeyPressed(27)) {
		playerIsMoving = false;
		currentScreen = SCREEN_MENU;
		return;
	}

	// Toggle the full-screen "current objectives" popup with I. Debounced
	// so holding the key doesn't flicker it open/closed every frame.
	if (waitForInstructionsKeyRelease) {
		if (!isKeyPressed('i') && !isKeyPressed('I')) waitForInstructionsKeyRelease = false;
	}
	else if (isKeyPressed('i') || isKeyPressed('I')) {
		showInstructionsPopup = !showInstructionsPopup;
		waitForInstructionsKeyRelease = true;
	}
	if (showInstructionsPopup) {
		playerIsMoving = false;
		return; // freeze movement/interaction while the popup is open
	}

	if (isSpecialKeyPressed(GLUT_KEY_F5)) saveGame();

	Bounds b;
	std::vector<NPC>* npcs;
	std::vector<Rect>* obstacles;
	switch (currentScreen) {
	case SCREEN_QUAD:      b = quadBounds;      npcs = &quadNPCs;      obstacles = &quadObstacles;      break;
	case SCREEN_LIBRARY:   b = libraryBounds;    npcs = &libraryNPCs;   obstacles = &libraryObstacles;   break;
	case SCREEN_CAFETERIA: b = cafeteriaBounds;  npcs = &cafeteriaNPCs; obstacles = &cafeteriaObstacles; break;
	case SCREEN_CLASSROOM: b = classroomBounds;  npcs = &classroomNPCs; obstacles = &classroomObstacles; break;
	case SCREEN_LAB:       b = labBounds;        npcs = &labNPCs;       obstacles = &labObstacles;       break;
	case SCREEN_FACULTY:   b = facultyBounds;    npcs = &facultyHallNPCs; obstacles = &facultyObstacles;   break;
	case SCREEN_TEACHER_ZAHID:    b = zahidOfficeBounds; npcs = &zahidOfficeNPCs; obstacles = &zahidOfficeObstacles; break;
	case SCREEN_TEACHER_TOWFIQUE: b = towfiqueBounds;    npcs = &towfiqueNPCs;    obstacles = &towfiqueObstacles;    break;
	case SCREEN_TEACHER_SAHA:     b = sahaBounds;        npcs = &sahaNPCs;        obstacles = &sahaObstacles;        break;
	case SCREEN_TEACHER_REASAD:   b = reasadBounds;      npcs = &reasadNPCs;      obstacles = &reasadObstacles;      break;
	case SCREEN_TEACHER_MAMUN:    b = mamunBounds;       npcs = &mamunNPCs;       obstacles = &mamunObstacles;       break;
	default: return;
	}

	// Cafeteria -> CSE Classroom unlock achievement. Checked every frame while
	// the player is standing in the cafeteria, so the "CLASSROOM UNLOCKED!"
	// banner pops up right there as soon as the requirement is met, instead of
	// only appearing after the player has already walked over to the door.
	if (currentScreen == SCREEN_CAFETERIA && !classroomUnlockAnnounced &&
		reputationPoints >= 25 && waiterQuestionAnswered && cashierQuestionAnswered) {
		classroomUnlockAnnounced = true;
		classroomUnlockBannerUntil = animTimer + CLASSROOM_UNLOCK_BANNER_DURATION;
	}

	// Reserved lab PC: press F near it to start the diagnostic mini-game
	// (only relevant before the "Lab Diagnostic Task" quest is complete).
	if (currentScreen == SCREEN_LAB && (isKeyPressed('f') || isKeyPressed('F')) &&
		!quests[LAB_MINIGAME_QUEST_INDEX].isComplete) {
		double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
		double pcx = LAB_PLAYER_DESK_X + 20, pcy = LAB_PLAYER_DESK_Y + 10;
		if (getDistance(px, py, pcx, pcy) <= INTERACT_RADIUS) {
			startLabMinigame();
			currentScreen = SCREEN_LAB_MINIGAME;
			return;
		}
	}

	// The Java table unlocks only after the first mission and a respectful
	// conversation with Rafi in the CSE classroom.
	if (currentScreen == SCREEN_CLASSROOM && (isKeyPressed('f') || isKeyPressed('F')) &&
		quests[1].isComplete && quests[CLASSROOM_COURTESY_QUEST_INDEX].isComplete &&
		!quests[JAVA_ERROR_HUNT_QUEST_INDEX].isComplete) {
		double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
		if (getDistance(px, py, CLASS_PLAYER_DESK_X + 22, CLASS_PLAYER_DESK_Y + 20) <= INTERACT_RADIUS) {
			startClassroomQuiz();
			currentScreen = SCREEN_CLASSROOM_QUIZ;
			return;
		}
	}

	// Classroom blackboard interaction (instruction board for The Missing Data)
	if (currentScreen == SCREEN_CLASSROOM && (isKeyPressed('f') || isKeyPressed('F'))) {
		double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
		const int blackboardX = 220, blackboardY = 340, blackboardW = 160, blackboardH = 42;
		double bx = blackboardX + blackboardW / 2.0, by = blackboardY + blackboardH / 2.0;
		if (getDistance(px, py, bx, by) <= INTERACT_RADIUS) {
			updateMissionTracker();
			activeNPC = nullptr;
			activeDialogueName = "Classroom Blackboard";
			activeDialogueKind = DIALOGUE_NORMAL;
			if (missingDataState == MISSING_DATA_COLLECTED || missingDataState == MISSING_DATA_COMPLETE) {
				activeDialogueText = "QUEST COMPLETE: The USB drive has been safely returned.";
			}
			else if (currentMissionIndex >= 2 || areMissingDataPrerequisitesComplete()) {
				// Transition quest from LOCKED/AVAILABLE to ACTIVE and spawn USB
				missingDataState = MISSING_DATA_ACTIVE;
				currentMissionIndex = 2;
				usbItem.isVisible = true;
				activeDialogueText = "URGENT QUEST: A senior lost a black USB drive containing their final project. Search the campus and return it for a reward.";
			}
			else {
				activeDialogueText = "CLASS NOTICE: Daily lectures underway. Complete your early campus tasks first.";
			}
			screenBeforeDialogue = SCREEN_CLASSROOM;
			currentScreen = SCREEN_DIALOGUE;
			return;
		}
	}

	// USB drive pickup in the classroom
	if (currentScreen == SCREEN_CLASSROOM && usbItem.isVisible && !usbItem.isCollected) {
		double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
		double ux = usbItem.x + usbItem.w / 2.0, uy = usbItem.y + usbItem.h / 2.0;
		Rect usbRect = { usbItem.x, usbItem.y, (double)usbItem.w, (double)usbItem.h };
		Rect playerRect = { player.x, player.y, (double)player.w, (double)player.h };
		bool nearUSB = getDistance(px, py, ux, uy) <= INTERACT_RADIUS;
		bool collidesUSB = rectsOverlap(playerRect, usbRect);

		if (collidesUSB || (nearUSB && (isKeyPressed('f') || isKeyPressed('F')))) {
			usbItem.isCollected = true;
			usbItem.isVisible = false;
			missingDataState = MISSING_DATA_COLLECTED;
			activeNPC = nullptr;
			activeDialogueName = "Ruel (You)";
			activeDialogueKind = DIALOGUE_NORMAL;
			// State 3 (Item Picked Up): Player internal monologue
			activeDialogueText = "Here it is. USB drive secured. I'd better get this back to the senior before they panic.";
			screenBeforeDialogue = SCREEN_CLASSROOM;
			currentScreen = SCREEN_DIALOGUE;
			return;
		}
	}

	bool up = isKeyPressed('w') || isSpecialKeyPressed(GLUT_KEY_UP);
	bool down = isKeyPressed('s') || isSpecialKeyPressed(GLUT_KEY_DOWN);
	bool left = isKeyPressed('a') || isSpecialKeyPressed(GLUT_KEY_LEFT);
	bool right = isKeyPressed('d') || isSpecialKeyPressed(GLUT_KEY_RIGHT);
	playerIsMoving = up || down || left || right;
	if (left && !right) playerFacingLeft = true;
	else if (right && !left) playerFacingLeft = false;

	// Try moving on each axis separately (lets the player slide along an
	// obstacle's edge instead of getting fully stuck when hitting it at
	// an angle). Only apply the move if it doesn't land inside a solid prop.
	double tryX = player.x + (right ? player.speed : 0) - (left ? player.speed : 0);
	Rect testX = { tryX, player.y, (double)player.w, (double)player.h };
	bool blockedX = false;
	for (unsigned int i = 0; i < obstacles->size(); i++)
	if (rectsOverlap(testX, (*obstacles)[i])) { blockedX = true; break; }
	if (!blockedX) player.x = tryX;

	double tryY = player.y + (up ? player.speed : 0) - (down ? player.speed : 0);
	Rect testY = { player.x, tryY, (double)player.w, (double)player.h };
	bool blockedY = false;
	for (unsigned int i = 0; i < obstacles->size(); i++)
	if (rectsOverlap(testY, (*obstacles)[i])) { blockedY = true; break; }
	if (!blockedY) player.y = tryY;

	// clamp to bounds
	if (player.x < b.minX) player.x = b.minX;
	if (player.x + player.w > b.maxX) player.x = b.maxX - player.w;
	if (player.y < b.minY) player.y = b.minY;
	if (player.y + player.h > b.maxY) player.y = b.maxY - player.h;

	// Location travel happens at the room edges, not through shortcut keys.
	// Each arrival starts a few pixels inside the new screen to prevent the
	// player from immediately crossing back while holding a movement key.
	if (currentScreen == SCREEN_QUAD && player.x <= quadBounds.minX) {
		currentScreen = SCREEN_LIBRARY;
		player.x = libraryBounds.maxX - player.w - 8;
		player.y = 200;
		return;
	}
	if (currentScreen == SCREEN_QUAD && player.x + player.w >= quadBounds.maxX) {
		currentScreen = SCREEN_CAFETERIA;
		player.x = cafeteriaBounds.minX + 8;
		player.y = 200;
		return;
	}
	if (currentScreen == SCREEN_LIBRARY && player.x + player.w >= libraryBounds.maxX) {
		currentScreen = SCREEN_QUAD;
		player.x = quadBounds.minX + 8;
		player.y = 80; // Quad's walkable ceiling is 160, not 370 - see quadBounds
		return;
	}
	if (currentScreen == SCREEN_CAFETERIA && player.x <= cafeteriaBounds.minX) {
		currentScreen = SCREEN_QUAD;
		player.x = quadBounds.maxX - player.w - 8;
		player.y = 80; // Quad's walkable ceiling is 160, not 370 - see quadBounds
		return;
	}
	if (currentScreen == SCREEN_CAFETERIA && player.x + player.w >= cafeteriaBounds.maxX) {
		// The CSE building is the second level. Both optional cafeteria
		// conversations must be completed correctly, and their reputation plus
		// the first mission must bring Ruel to at least 25 points.
		if (reputationPoints >= 25 && waiterQuestionAnswered && cashierQuestionAnswered) {
			currentScreen = SCREEN_CLASSROOM;
			player.x = classroomBounds.minX + 8;
			player.y = 70;
			// The unlock banner has already been triggered back in the
			// cafeteria (see the check above), so nothing to do for it here.
		}
		else {
			player.x = cafeteriaBounds.maxX - player.w - 8;
		}
		return;
	}
	if (currentScreen == SCREEN_CLASSROOM && player.x <= classroomBounds.minX) {
		currentScreen = SCREEN_CAFETERIA;
		player.x = cafeteriaBounds.maxX - player.w - 8;
		player.y = 200;
		return;
	}
	if (currentScreen == SCREEN_CLASSROOM && player.x + player.w >= classroomBounds.maxX) {
		currentScreen = SCREEN_LAB;
		player.x = labBounds.minX + 8;
		player.y = 200;
		return;
	}
	if (currentScreen == SCREEN_LAB && player.x <= labBounds.minX) {
		currentScreen = SCREEN_CLASSROOM;
		player.x = classroomBounds.maxX - player.w - 8;
		player.y = 70;
		return;
	}
	if (currentScreen == SCREEN_LAB && player.x + player.w >= labBounds.maxX) {
		currentScreen = SCREEN_FACULTY;
		player.x = facultyBounds.minX + 8;
		player.y = 200;
		return;
	}
	if (currentScreen == SCREEN_FACULTY && player.x <= facultyBounds.minX) {
		currentScreen = SCREEN_LAB;
		player.x = labBounds.maxX - player.w - 8;
		player.y = 200;
		return;
	}

	// Arrival stages the classroom scene. Ruel must still speak up for Ayon.
	if (currentScreen == SCREEN_CLASSROOM && ayonEscortState == AYON_ESCORT_ESCORTING) {
		ayonEscortState = AYON_ESCORT_AT_CLASSROOM;
		teacherAngryUntil = GetTickCount() + 5000; // exactly five seconds
	}

	// Each teacher's office has a single exit back to the faculty hall,
	// re-spawning the player right by the door they came through.
	if (currentScreen == SCREEN_TEACHER_ZAHID && player.x <= zahidOfficeBounds.minX) {
		currentScreen = SCREEN_FACULTY;
		player.x = facultyHubDoors[0].x + 8;
		player.y = facultyBounds.minY + 40;
		return;
	}
	if (currentScreen == SCREEN_TEACHER_TOWFIQUE && player.x <= towfiqueBounds.minX) {
		currentScreen = SCREEN_FACULTY;
		player.x = facultyHubDoors[1].x + 8;
		player.y = facultyBounds.minY + 40;
		return;
	}
	if (currentScreen == SCREEN_TEACHER_SAHA && player.x <= sahaBounds.minX) {
		currentScreen = SCREEN_FACULTY;
		player.x = facultyHubDoors[2].x + 8;
		player.y = facultyBounds.minY + 40;
		return;
	}
	if (currentScreen == SCREEN_TEACHER_REASAD && player.x <= reasadBounds.minX) {
		currentScreen = SCREEN_FACULTY;
		player.x = facultyHubDoors[3].x + 8;
		player.y = facultyBounds.minY + 40;
		return;
	}
	if (currentScreen == SCREEN_TEACHER_MAMUN && player.x <= mamunBounds.minX) {
		currentScreen = SCREEN_FACULTY;
		player.x = facultyHubDoors[4].x + 8;
		player.y = facultyBounds.minY + 40;
		return;
	}

	// Nissan joins Ayon at the staged classroom scene. His conversation with
	// Mim is triggered at his in-room position rather than back in the cafe.
	if (currentScreen == SCREEN_CLASSROOM && ayonEscortState == AYON_ESCORT_AT_CLASSROOM &&
		rafiBullyingResolved && !quests[NISSAN_MIM_QUEST_INDEX].isComplete &&
		(isKeyPressed('f') || isKeyPressed('F'))) {
		double px = player.x + player.w / 2.0, py = player.y + player.h / 2.0;
		if (getDistance(px, py, 415, 143) <= INTERACT_RADIUS) {
			activeNPC = nullptr;
			activeDialogueName = "Nissan and Mim";
			activeDialogueText = "Mim is here. Nissan, how will you introduce yourself?";
			activeDialogueKind = DIALOGUE_NISSAN_MIM_CHOICE;
			screenBeforeDialogue = SCREEN_CLASSROOM;
			currentScreen = SCREEN_DIALOGUE;
			return;
		}
	}

	// interact key
	if (npcs != nullptr && (isKeyPressed('f') || isKeyPressed('F'))) {
		tryInteract(*npcs);
	}
}

int main()
{
	iInitialize(SCREEN_WIDTH, SCREEN_HEIGHT, "Campus Chronicles");

	loadAssets(); // must happen after iInitialize() creates the window/GL context

	if (menuMusicEnabled) playBackgroundMusic(); // splash/menu theme - stops once New/Load Game actually begins

	iStart();
	return 0;
}