#ifndef QUEST_H
#define QUEST_H

#include <string>
#include <vector>

// ---------------- Reputation ----------------
int reputationPoints = 0;

// ---------------- Quest ----------------
struct Quest {
	std::string title;
	std::string description;
	bool isComplete;
	int reward;
};

// Quests 3-8 form one correlated chain: fixing the lab PC for Zahid Sir
// leads to Zahid Hossain's office, and each teacher after him sends the
// player on to the next, ending with the department head's final approval.
std::vector<Quest> quests = {
	{ "Find the Library",           "Talk to the librarian to unlock research materials.", false, 10 },
	{ "Five Taka Favor",            "Take Ashraful's 5 taka from the library to Dipta in the cafeteria.", false, 15 },
	{ "Settle a Tab",                "Help the cashier sort out a mixed-up lunch order.",    false, 10 },
	{ "Lab Diagnostic Task",         "Run the computer diagnostic mini-game on your PC in the lab for Zahid Sir.", false, 15 },
	{ "Meet Zahid Hossain",          "Report back to Zahid Hossain about the lab diagnostic.", false, 15 },
	{ "Algorithms Review",           "Show Kazi Towfique Elahi your completed algorithms review.", false, 15 },
	{ "Database Sign-off",           "Get your database project signed off by Mr. Saha Reno.", false, 15 },
	{ "Software Engineering Task",   "Submit your software engineering task to Md Reasad Zaman Chowdhury.", false, 15 },
	{ "Final Department Approval",   "Present your full semester progress to Prof. Al Mamun, the department head.", false, 25 },
	{ "Classroom Courtesy",          "After the Five Taka Favor, speak respectfully with your CSE classmates.", false, 10 },
	{ "Java Error Hunt",             "Use your study table to find and fix Java errors.", false, 15 },
	{ "Nissan and Mim",              "Introduce Nissan to Mim in the CSE classroom, then guide their respectful conversation.", false, 10 },
	{ "Bring Ayon to Rafi",          "After Java Error Hunt, find Ayon in the library and escort him to Rafi.", false, 15 },
	{ "The Missing Data",            "Find the senior's lost black USB drive containing their final project and return it.", false, 20 }
};

// Shared quest IDs. These live here (rather than in Minigame.hpp) because
// both NPC dialogue and the quiz logic use them.
const int CLASSROOM_COURTESY_QUEST_INDEX = 9;
const int JAVA_ERROR_HUNT_QUEST_INDEX = 10;
const int NISSAN_MIM_QUEST_INDEX = 11;
const int AYON_ESCORT_QUEST_INDEX = 12;
const int MISSING_DATA_QUEST_INDEX = 13;

inline void completeQuest(int index) {
	if (index < 0 || index >= (int)quests.size()) return;
	if (quests[index].isComplete) return;
	quests[index].isComplete = true;
	reputationPoints += quests[index].reward;
}

#endif
