#ifndef MINIGAME_H
#define MINIGAME_H

#include <string>
#include <vector>
#include "GameState.hpp"
#include "Quest.hpp"

// ---------------- Lab PC diagnostic mini-game ----------------
// Played on the player's reserved computer in the lab (see LAB_PLAYER_DESK_X/Y
// in GameState.hpp and drawLab()/drawLabMinigame() in Draw.hpp). A short
// multiple-choice "diagnostic" the player must pass to fix the frozen PC and
// kick off the CSE teacher-approval quest chain.

// Index into the quests vector (Quest.hpp) that this mini-game completes.
const int LAB_MINIGAME_QUEST_INDEX = 3;

enum LabMinigameState {
	LAB_MINIGAME_NOT_STARTED,
	LAB_MINIGAME_IN_PROGRESS,
	LAB_MINIGAME_FINISHED
};

LabMinigameState labMinigameState = LAB_MINIGAME_NOT_STARTED;
int labMinigameQuestionIndex = 0;
int labMinigameCorrectCount = 0;
// Debounces the 1/2/3 answer keys so holding one down doesn't blow through
// every question in a single frame-polling burst.
bool labMinigameWaitForKeyRelease = false;

struct MinigameQuestion {
	std::string prompt;
	std::string options[3];
	int correctOption; // 1, 2, or 3
};

std::vector<MinigameQuestion> labMinigameQuestions = {
	{
		"The lab PC freezes right after boot. First thing to check?",
		{ "Whether a process is stuck in an infinite loop", "The color of the mouse", "How many browser tabs are open" },
		1
	},
	{
		"A program crashes with a null pointer error. What likely happened?",
		{ "Too much RAM is installed", "A pointer was used before it was set to something valid", "The monitor brightness is too high" },
		2
	},
	{
		"Best way to track down a bug in code someone else wrote?",
		{ "Rewrite the whole program from scratch", "Read the error message and trace it step by step", "Restart the computer and hope" },
		2
	}
};

inline void startLabMinigame() {
	labMinigameState = LAB_MINIGAME_IN_PROGRESS;
	labMinigameQuestionIndex = 0;
	labMinigameCorrectCount = 0;
	labMinigameWaitForKeyRelease = false;
}

inline void answerLabMinigame(int choice) {
	if (labMinigameQuestionIndex < 0 || labMinigameQuestionIndex >= (int)labMinigameQuestions.size()) return;
	if (choice == labMinigameQuestions[labMinigameQuestionIndex].correctOption) labMinigameCorrectCount++;
	labMinigameQuestionIndex++;
}

// Called when the player closes the results screen. Passing (2/3 correct)
// completes the "Lab Diagnostic Task" quest; failing lets them retry later.
inline void finishLabMinigame() {
	bool passed = labMinigameCorrectCount >= 2;
	if (passed) completeQuest(LAB_MINIGAME_QUEST_INDEX);
	labMinigameState = passed ? LAB_MINIGAME_FINISHED : LAB_MINIGAME_NOT_STARTED;
}

// ---------------- CSE classroom: Java Error Hunt ----------------
enum ClassroomQuizState { CLASSROOM_QUIZ_NOT_STARTED, CLASSROOM_QUIZ_IN_PROGRESS, CLASSROOM_QUIZ_FINISHED };
ClassroomQuizState classroomQuizState = CLASSROOM_QUIZ_NOT_STARTED;
int classroomQuizQuestionIndex = 0;
int classroomQuizCorrectCount = 0;
bool classroomQuizWaitForKeyRelease = false;

std::vector<MinigameQuestion> classroomQuizQuestions = {
	{ "Which line fixes: int total = \"10\";", { "int total = 10;", "String total = 10;", "int total == 10;" }, 1 },
	{ "What is missing here? public static void main(String[] args) {", { "A semicolon after args", "A closing }", "A comma after main" }, 2 },
	{ "Which comparison is correct for String names in Java?", { "name == \"Ayon\"", "name.equals(\"Ayon\")", "name = \"Ayon\"" }, 2 }
};

inline void startClassroomQuiz() {
	classroomQuizState = CLASSROOM_QUIZ_IN_PROGRESS;
	classroomQuizQuestionIndex = 0;
	classroomQuizCorrectCount = 0;
	classroomQuizWaitForKeyRelease = false;
}

inline void answerClassroomQuiz(int choice) {
	if (classroomQuizQuestionIndex < 0 || classroomQuizQuestionIndex >= (int)classroomQuizQuestions.size()) return;
	if (choice == classroomQuizQuestions[classroomQuizQuestionIndex].correctOption) classroomQuizCorrectCount++;
	classroomQuizQuestionIndex++;
}

inline void finishClassroomQuiz() {
	bool passed = classroomQuizCorrectCount >= 2;
	if (passed) completeQuest(JAVA_ERROR_HUNT_QUEST_INDEX);
	classroomQuizState = passed ? CLASSROOM_QUIZ_FINISHED : CLASSROOM_QUIZ_NOT_STARTED;
}

#endif
