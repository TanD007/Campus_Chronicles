#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <string>
#include <vector>

struct NPC; // forward declaration - full definition lives in NPC.hpp

// ---------------- Game Screens ----------------
enum GameScreen {
	SCREEN_SPLASH,
	SCREEN_MENU,
	SCREEN_QUAD,       // outdoor central area - hub, connects to other screens
	SCREEN_LIBRARY,
	SCREEN_CAFETERIA,
	SCREEN_CLASSROOM,
	SCREEN_CLASSROOM_QUIZ, // Java error-hunt quiz opened from the study table
	SCREEN_LAB,
	SCREEN_LAB_MINIGAME,  // overlay: the PC diagnostic mini-game played from inside the lab
	SCREEN_FACULTY,        // open faculty hall; teachers are at PC workstations
	SCREEN_TEACHER_ZAHID,
	SCREEN_TEACHER_TOWFIQUE,
	SCREEN_TEACHER_SAHA,
	SCREEN_TEACHER_REASAD,
	SCREEN_TEACHER_MAMUN,
	SCREEN_INSTRUCTIONS,
	SCREEN_SETTINGS,
	SCREEN_ABOUT,
	SCREEN_DIALOGUE    // overlay state, drawn on top of whichever screen opened it
};

GameScreen currentScreen = SCREEN_SPLASH;
GameScreen screenBeforeDialogue = SCREEN_QUAD; // remember where we were when dialogue opened

// ---------------- Active dialogue ----------------
std::string activeDialogueName;
std::string activeDialogueText;
NPC* activeNPC = nullptr; // pointer to the NPC currently being talked to (for quest triggering)

enum DialogueKind {
	DIALOGUE_NORMAL,
	DIALOGUE_WAITER_QUESTION,
	DIALOGUE_CASHIER_QUESTION,
	DIALOGUE_DIPTA_CHOICE,
	DIALOGUE_CLASSMATE_CHOICE,
	DIALOGUE_RAFI_BULLY_CHOICE,
	DIALOGUE_NISSAN_MIM_CHOICE
};
DialogueKind activeDialogueKind = DIALOGUE_NORMAL;

// ---------------- Library -> Cafeteria mission ----------------
enum TakaMissionState {
	TAKA_MISSION_NOT_STARTED,
	TAKA_MISSION_CARRYING_MONEY,
	TAKA_MISSION_COMPLETE
};
TakaMissionState takaMissionState = TAKA_MISSION_NOT_STARTED;
int takaCarried = 0;

bool nissanConnectionStarted = false;
bool rafiBullyingResolved = false;
bool mimLeftClassroom = false;
enum AyonEscortState { AYON_ESCORT_NOT_READY, AYON_ESCORT_FIND_AYON, AYON_ESCORT_ESCORTING, AYON_ESCORT_AT_CLASSROOM, AYON_ESCORT_COMPLETE };
AyonEscortState ayonEscortState = AYON_ESCORT_NOT_READY;
// Absolute timestamp keeps Hasib Sir's arrival reaction at exactly five
// seconds, independent of keyboard-repeat or frame-update frequency.
DWORD teacherAngryUntil = 0;

bool waiterQuestionAnswered = false;
bool cashierQuestionAnswered = false;

// ---------------- Mission Sequence Tracker & "The Missing Data" Quest ----------------
int currentMissionIndex = 0;

enum MissingDataMissionState {
	MISSING_DATA_LOCKED = 0,    // Prerequisites not yet completed
	MISSING_DATA_AVAILABLE = 1, // Board notice ready to be accepted
	MISSING_DATA_ACTIVE = 2,    // Quest accepted, USB spawned
	MISSING_DATA_COLLECTED = 3, // USB picked up by player
	MISSING_DATA_COMPLETE = 4   // Returned to Senior Student
};
MissingDataMissionState missingDataState = MISSING_DATA_LOCKED;

struct USBItem {
	double x, y;
	int w, h;
	bool isVisible;
	bool isCollected;
};
USBItem usbItem = { 460, 326, 20, 20, false, false };

// Menu preferences. Audio is optional in this project, but the setting is
// kept here so the menu behaves correctly when audio is enabled later.
bool menuMusicEnabled = true;
std::string menuStatus;
// Avoid using the same Enter/Space press to dismiss the splash and start a game.
bool waitForSplashKeyRelease = false;
// Same idea for the menu's Up/Down highlight: one press moves one button.
bool menuWaitForArrowRelease = false;

// Cafeteria staff stand behind their counter, so this leaves enough reach
// to talk to them from the customer side without walking through furniture.
const double INTERACT_RADIUS = 65.0;

// The lab desk reserved for the player (top-left of the 2x3 desk grid in
// drawLab()). Shared between Draw.hpp (drawing/highlighting the desk) and
// iMain.cpp (the F-to-run-diagnostic trigger), so both always agree on
// exactly where that desk is.
const int LAB_PLAYER_DESK_X = 70;
const int LAB_PLAYER_DESK_Y = 190;

// Personal study table in the CSE classroom. It becomes usable after the
// player has completed the first (Five Taka Favor) mission and then shown
// courteous behaviour to a classmate.
const int CLASS_PLAYER_DESK_X = 75;
const int CLASS_PLAYER_DESK_Y = 105;

// ---------------- Faculty hall doors ----------------
// The faculty hall (SCREEN_FACULTY) is a small hub with one door per
// teacher's office. Walking up to a door and pressing F enters that room -
// see tryEnterDoor() in NPC.hpp.
struct Door {
	double x, y; int w, h;
	std::string label;
	GameScreen target;
};

std::vector<Door> facultyHubDoors = {
	{ 40, 300, 80, 60, "Zahid Hossain", SCREEN_TEACHER_ZAHID },
	{ 150, 300, 80, 60, "Kazi Towfique Elahi", SCREEN_TEACHER_TOWFIQUE },
	{ 260, 300, 80, 60, "Mr. Saha Reno", SCREEN_TEACHER_SAHA },
	{ 370, 300, 80, 60, "Reasad Chowdhury", SCREEN_TEACHER_REASAD },
	{ 480, 300, 80, 60, "Prof. Al Mamun", SCREEN_TEACHER_MAMUN }
};

// ---------------- Animation clock ----------------
// Incremented once per fixedUpdate() call in iMain.cpp. Drives idle bob,
// walk cycles, and pulsing glow effects across the whole game.
int animTimer = 0;

#endif