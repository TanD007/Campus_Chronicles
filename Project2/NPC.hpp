#ifndef NPC_H
#define NPC_H

#include <string>
#include <vector>
#include <math.h>
// NOTE: assumes iGraphics.h has already been included by iMain.cpp.
// Do not #include "iGraphics.h" again here - see Player.hpp for why.
// Also assumes Assets.hpp (imgNpc...) has already been included.
#include "Utils.hpp"
#include "Player.hpp"
#include "Quest.hpp"
#include "GameState.hpp"

// ---------------- NPC ----------------
struct NPC {
	double x, y;
	int w, h;
	std::string name;
	std::string dialogue;
	int questIndex; // -1 if this NPC has no quest tied to it
	int* sprite;    // pointer to this NPC's image handle in Assets.hpp
	DialogueKind dialogueKind;
	int missionRole; // 0 = normal, 1 = Ashraful, 2 = Dipta
};

const int MISSION_ROLE_NONE = 0;
const int MISSION_ROLE_ASHRAFUL = 1;
const int MISSION_ROLE_DIPTA = 2;
const int MISSION_ROLE_LAB = 3;          // Zahid Sir, the lab assistant
const int MISSION_ROLE_ZAHID_OFFICE = 4; // Zahid Hossain, same person, in his office
const int MISSION_ROLE_TOWFIQUE = 5;
const int MISSION_ROLE_SAHA = 6;
const int MISSION_ROLE_REASAD = 7;
const int MISSION_ROLE_MAMUN = 8;        // department head - final approval
const int MISSION_ROLE_CLASSMATE = 9;
const int MISSION_ROLE_NISSAN = 10;
const int MISSION_ROLE_MIM = 11;
const int MISSION_ROLE_AYON = 12;
const int MISSION_ROLE_SENIOR_STUDENT = 13;

inline bool areMissingDataPrerequisitesComplete() {
	return quests[1].isComplete && (cashierQuestionAnswered || quests[2].isComplete);
}

inline void updateMissionTracker() {
	if (quests[MISSING_DATA_QUEST_INDEX].isComplete) {
		if (currentMissionIndex < 3) currentMissionIndex = 3;
	}
	else if (missingDataState >= MISSING_DATA_ACTIVE) {
		if (currentMissionIndex < 2) currentMissionIndex = 2;
	}
	else if (areMissingDataPrerequisitesComplete()) {
		if (currentMissionIndex < 2) currentMissionIndex = 2;
		if (missingDataState == MISSING_DATA_LOCKED) missingDataState = MISSING_DATA_AVAILABLE;
	}
	else if (quests[1].isComplete) {
		if (currentMissionIndex < 1) currentMissionIndex = 1;
	}
}

std::vector<NPC> quadNPCs = {
	{ 450, 150, 30, 40, "Senior Student", "I'm so busy preparing for my final project right now.", MISSING_DATA_QUEST_INDEX, &imgNpcSenior, DIALOGUE_NORMAL, MISSION_ROLE_SENIOR_STUDENT }
};

std::vector<NPC> libraryNPCs = {
	{ 435, 295, 30, 40, "Librarian", "Welcome to the library. Please keep your voice low.", -1, &imgNpcLibrarian, DIALOGUE_NORMAL, MISSION_ROLE_NONE },
	{ 210, 245, 30, 40, "Ashraful", "Could you help me with a small favor?", -1, &imgNpcWaiter, DIALOGUE_NORMAL, MISSION_ROLE_ASHRAFUL },
	{ 285, 245, 30, 40, "Ayon", "I am looking for a quiet place to study.", -1, &imgNpcSenior, DIALOGUE_NORMAL, MISSION_ROLE_AYON }
};

std::vector<NPC> cafeteriaNPCs = {
	{ 220, 145, 30, 40, "Nissan", "I want to get to know Mim from the CSE classroom.", -1, &imgNpcClassmate, DIALOGUE_NORMAL, MISSION_ROLE_NISSAN },
	{ 120, 155, 30, 40, "Waiter", "Assalamualaikum Bhai, Ki Khaben?", -1, &imgNpcWaiter, DIALOGUE_WAITER_QUESTION, MISSION_ROLE_NONE },
	{ 485, 195, 30, 40, "Dipta", "Amar 50 taka kobe dibi?", -1, &imgNpcStudent2, DIALOGUE_NORMAL, MISSION_ROLE_DIPTA },
	{ 285, 270, 30, 40, "Cashier", "Apnar bill asche 100 taka", -1, &imgNpcCashier, DIALOGUE_CASHIER_QUESTION, MISSION_ROLE_NONE }
};

// CSE Classroom: one teacher at the front and four students at study tables.
std::vector<NPC> classroomNPCs = {
	{ 285, 285, 30, 40, "Hasib Sir", "Please take your seat and focus on the lesson.", -1, &imgNpcLibrarian, DIALOGUE_NORMAL, MISSION_ROLE_NONE },
	{ 275, 205, 30, 40, "Rafi", "I am reviewing today's CSE lesson.", -1, &imgNpcSenior, DIALOGUE_NORMAL, MISSION_ROLE_CLASSMATE },
	{ 350, 118, 30, 40, "Mim", "This algorithm problem is interesting.", -1, &imgNpcClassmate, DIALOGUE_NORMAL, MISSION_ROLE_MIM },
	{ 220, 28, 30, 40, "Mazid", "I am preparing for the next lab.", -1, &imgNpcStudent2, DIALOGUE_NORMAL, MISSION_ROLE_NONE },
	{ 350, 28, 30, 40, "Nusaiba", "Can we study together after class?", -1, &imgNpcWaiter, DIALOGUE_NORMAL, MISSION_ROLE_NONE }
};

// Computer Lab: Zahid Sir has a clearly visible help station at the front.
// The other two students are drawn before their desks, so their lower halves
// sit behind the furniture instead of appearing to stand on top of it.
std::vector<NPC> labNPCs = {
	{ 330, 316, 30, 40, "Zahid Sir", "One of the PCs on the far left keeps freezing. Sit down and press F to run a diagnostic.", -1, &imgNpcCashier, DIALOGUE_NORMAL, MISSION_ROLE_LAB },
	{ 302, 190, 26, 32, "Mazid", "This assignment is due tomorrow, I'm barely keeping up.", -1, &imgNpcStudent2, DIALOGUE_NORMAL, MISSION_ROLE_NONE },
	{ 482, 80,  26, 32, "Mim", "Careful, the lab computers can be slow to boot.", -1, &imgNpcClassmate, DIALOGUE_NORMAL, MISSION_ROLE_NONE }
};

// Faculty Hall is intentionally a single open room. Each teacher is directly
// available at a compact workstation; there is no separate office to enter.
std::vector<NPC> facultyHallNPCs = {
	{ 212, 272, 26, 34, "Zahid Hossain",       "Ready to talk about your CSE progress.", 4, &imgNpcLibrarian, DIALOGUE_NORMAL, MISSION_ROLE_ZAHID_OFFICE },
	{ 332, 272, 26, 34, "Kazi Towfique Elahi", "Let's see your algorithms review.", 5, &imgNpcSenior, DIALOGUE_NORMAL, MISSION_ROLE_TOWFIQUE },
	{ 452, 272, 26, 34, "Mr. Saha Reno",       "Bring your database project here when you're ready.", 6, &imgNpcCashier, DIALOGUE_NORMAL, MISSION_ROLE_SAHA },
	{ 272, 142, 26, 34, "Reasad Chowdhury",    "Let's discuss your software engineering task.", 7, &imgNpcWaiter, DIALOGUE_NORMAL, MISSION_ROLE_REASAD },
	{ 392, 142, 26, 34, "Prof. Al Mamun",      "Come in - show me your full semester progress.", 8, &imgNpcClassmate, DIALOGUE_NORMAL, MISSION_ROLE_MAMUN }
};

// Legacy office NPC data is retained for save compatibility, but gameplay
// now uses facultyHallNPCs above and never requires these separate rooms.
std::vector<NPC> zahidOfficeNPCs = {
	{ 272, 345, 30, 40, "Zahid Hossain", "Ready to talk about your CSE progress.", 4, &imgNpcLibrarian, DIALOGUE_NORMAL, MISSION_ROLE_ZAHID_OFFICE }
};
std::vector<NPC> towfiqueNPCs = {
	{ 272, 345, 30, 40, "Kazi Towfique Elahi", "Let's see your algorithms review.", 5, &imgNpcSenior, DIALOGUE_NORMAL, MISSION_ROLE_TOWFIQUE }
};
std::vector<NPC> sahaNPCs = {
	{ 272, 345, 30, 40, "Mr. Saha Reno", "Bring your database project here when you're ready.", 6, &imgNpcCashier, DIALOGUE_NORMAL, MISSION_ROLE_SAHA }
};
std::vector<NPC> reasadNPCs = {
	{ 272, 345, 30, 40, "Md Reasad Zaman Chowdhury", "Let's discuss your software engineering task.", 7, &imgNpcWaiter, DIALOGUE_NORMAL, MISSION_ROLE_REASAD }
};
std::vector<NPC> mamunNPCs = {
	{ 272, 345, 30, 40, "Prof. Al Mamun", "Come in - show me your full semester progress.", 8, &imgNpcClassmate, DIALOGUE_NORMAL, MISSION_ROLE_MAMUN }
};

inline void answerDialogueQuestion(int choice) {
	if (activeDialogueKind == DIALOGUE_RAFI_BULLY_CHOICE) {
		if (choice == 1) {
			rafiBullyingResolved = true;
			ayonEscortState = AYON_ESCORT_AT_CLASSROOM;
			completeQuest(AYON_ESCORT_QUEST_INDEX);
			activeDialogueText = "Rafi apologizes to Ayon. Ruel's kindness brings the classmates together.";
		}
		else {
			reputationPoints -= 5;
			if (reputationPoints < 0) reputationPoints = 0;
			activeDialogueText = "Ignoring bullying costs 5 reputation. Speak to Rafi again and choose to help Ayon.";
		}
		activeDialogueKind = DIALOGUE_NORMAL;
		return;
	}
	if (activeDialogueKind == DIALOGUE_NISSAN_MIM_CHOICE) {
		if (choice == 1) {
			completeQuest(NISSAN_MIM_QUEST_INDEX);
			activeDialogueText = "Nissan: Mim, your dedication inspires me. Mim: Thank you, Nissan. Let's study together!";
		}
		else {
			mimLeftClassroom = true;
			quests[NISSAN_MIM_QUEST_INDEX].isComplete = true; // story ends without its reward
			reputationPoints -= 10;
			if (reputationPoints < 0) reputationPoints = 0;
			activeDialogueText = "Nissan's rude words upset Mim. She leaves the classroom. Ruel loses 10 reputation.";
		}
		activeDialogueKind = DIALOGUE_NORMAL;
		return;
	}
	if (activeDialogueKind == DIALOGUE_CLASSMATE_CHOICE) {
		if (choice == 1) {
			completeQuest(CLASSROOM_COURTESY_QUEST_INDEX);
			activeDialogueText = "Thanks for being respectful. Your study table is ready for the Java Error Hunt.";
		}
		else {
			reputationPoints -= 5;
			if (reputationPoints < 0) reputationPoints = 0;
			activeDialogueText = "That was rude. You lost 5 reputation points - try speaking respectfully.";
		}
		activeDialogueKind = DIALOGUE_NORMAL;
		return;
	}
	// Dipta's mission branch: only a helpful answer lets the player return
	// Ashraful's money and finish the delivery quest.
	if (activeDialogueKind == DIALOGUE_DIPTA_CHOICE) {
		if (choice == 2) {
			takaCarried = 0;
			takaMissionState = TAKA_MISSION_COMPLETE;
			completeQuest(1);
			updateMissionTracker();
			activeDialogueText = "Ayon ekta valo pola!Dua kori jeno CGPA 4.00 pai";
		}
		else {
			activeDialogueText = "Faltu ekta pola";
		}
		activeDialogueKind = DIALOGUE_NORMAL;
		return;
	}

	bool correct = false;
	bool* alreadyAnswered = nullptr;

	if (activeDialogueKind == DIALOGUE_WAITER_QUESTION) {
		// The waiter accepts options 1-3; only the fourth answer is wrong.
		correct = (choice != 4);
		alreadyAnswered = &waiterQuestionAnswered;
	}
	else if (activeDialogueKind == DIALOGUE_CASHIER_QUESTION) {
		correct = (choice == 1); // option 1 gives the cashier's positive response
		alreadyAnswered = &cashierQuestionAnswered;
	}

	if (alreadyAnswered == nullptr || *alreadyAnswered) return;
	if (correct) {
		*alreadyAnswered = true;
		if (alreadyAnswered == &cashierQuestionAnswered) {
			completeQuest(2);
		}
		else {
			reputationPoints += 10;
		}
		updateMissionTracker();
		activeDialogueText = "Dhonnobad!";
	}
	else {
		reputationPoints -= 5;
		if (reputationPoints < 0) reputationPoints = 0;
		activeDialogueText = "chottraker bacca!";
	}
	activeDialogueKind = DIALOGUE_NORMAL;
}

inline void drawNPC(const NPC& npc) {
	double px = player.x + player.w / 2.0;
	double py = player.y + player.h / 2.0;
	double nx = npc.x + npc.w / 2.0;
	double ny = npc.y + npc.h / 2.0;
	bool inRange = getDistance(px, py, nx, ny) <= INTERACT_RADIUS;

	// Phase-offset bob so multiple NPCs don't move in perfect sync
	int bob = (int)(1.5 * sin(animTimer * 0.1 + npc.x));

	// Pulsing "you can talk to me" glow behind the NPC when player is close
	if (inRange) {
		int pulse = 90 + (int)(60 * sin(animTimer * 0.2));
		iSetColor(255, 255 - pulse / 3, pulse);
		iFilledCircle((int)nx, (int)(npc.y + npc.h / 2), npc.w, 30);
	}

	drawShadow((int)nx, (int)npc.y, npc.w, 8);

	iShowImage((int)npc.x, (int)npc.y + bob, npc.w, npc.h + 10, *npc.sprite);

	// Clean, border-free name label centred above every NPC. Helvetica 12 is
	// roughly 6 pixels per character, so this stays centred for all names.
	int nameTextX = (int)nx - (int)npc.name.size() * 3;
	int nameTextY = (int)npc.y + npc.h + 16;
	if (nameTextX < 8) nameTextX = 8;
	iSetColor(10, 10, 10); // subtle shadow keeps names readable on any floor
	iText(nameTextX + 1, nameTextY - 1, const_cast<char*>(npc.name.c_str()), GLUT_BITMAP_HELVETICA_12);
	iSetColor(255, 250, 225);
	iText(nameTextX, nameTextY, const_cast<char*>(npc.name.c_str()), GLUT_BITMAP_HELVETICA_12);

	if (inRange) {
		iSetColor(255, 255, 0);
		iText((int)nx - 33, nameTextY + 16, "Press F to talk", GLUT_BITMAP_HELVETICA_12);
	}
}

inline void tryInteract(std::vector<NPC>& npcs) {
	for (unsigned int i = 0; i < npcs.size(); i++) {
		if (mimLeftClassroom && npcs[i].missionRole == MISSION_ROLE_MIM) continue;
		if (ayonEscortState == AYON_ESCORT_ESCORTING && npcs[i].missionRole == MISSION_ROLE_AYON) continue;
		if (ayonEscortState == AYON_ESCORT_ESCORTING && npcs[i].missionRole == MISSION_ROLE_DIPTA) continue;
		double px = player.x + player.w / 2.0;
		double py = player.y + player.h / 2.0;
		double nx = npcs[i].x + npcs[i].w / 2.0;
		double ny = npcs[i].y + npcs[i].h / 2.0;

		if (getDistance(px, py, nx, ny) <= INTERACT_RADIUS) {
			activeNPC = &npcs[i];
			activeDialogueName = npcs[i].name;
			activeDialogueText = npcs[i].dialogue;
			activeDialogueKind = npcs[i].dialogueKind;

			if (npcs[i].name == "Librarian") {
				if (!quests[0].isComplete) {
					completeQuest(0);
					updateMissionTracker();
				}
			}

			// Hasib Sir is still cross about the class walking in late right after
			// the Ayon escort, for the same five seconds his angry speech cloud is
			// up (see drawClassroom() in Draw.hpp) - after that he's back to his
			// normal line.
			if (npcs[i].name == "Hasib Sir" &&
				(ayonEscortState == AYON_ESCORT_AT_CLASSROOM || ayonEscortState == AYON_ESCORT_COMPLETE) &&
				GetTickCount() < teacherAngryUntil) {
				activeDialogueText = "You're late! " + npcs[i].dialogue;
			}

			if (npcs[i].missionRole == MISSION_ROLE_SENIOR_STUDENT) {
				activeDialogueKind = DIALOGUE_NORMAL;
				updateMissionTracker();

				if (missingDataState == MISSING_DATA_COMPLETE) {
					activeDialogueText = "You're an absolute lifesaver! Thanks again for recovering my project.";
				}
				else if (missingDataState == MISSING_DATA_COLLECTED) {
					// State 4 (Quest Turn-In)
					missingDataState = MISSING_DATA_COMPLETE;
					completeQuest(MISSING_DATA_QUEST_INDEX);
					updateMissionTracker();
					activeDialogueText = "You found it! You have no idea how much trouble you just saved me from. You're an absolute lifesaver!";
				}
				else if (missingDataState == MISSING_DATA_ACTIVE) {
					// State 2 (Quest Active, USB not found)
					activeDialogueText = "Please hurry! If I don't get that USB drive back, I'm going to fail the semester.";
				}
				else if (missingDataState == MISSING_DATA_AVAILABLE || areMissingDataPrerequisitesComplete()) {
					// State 1 (Quest Start)
					activeDialogueText = "Hey, wait! Have you seen a small black USB drive? My entire final project is on it! I think I dropped it near the lockers.";
				}
				else {
					// State 0 (Before Mission is Unlocked)
					activeDialogueText = "I'm so busy preparing for my final project right now.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_ASHRAFUL) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (takaMissionState == TAKA_MISSION_NOT_STARTED) {
					takaMissionState = TAKA_MISSION_CARRYING_MONEY;
					takaCarried = 5;
					activeDialogueText = "Dipta amar rickshaw vara dise.Fokinni er 5 taka ferot diye ai";
				}
				else if (takaMissionState == TAKA_MISSION_CARRYING_MONEY) {
					activeDialogueText = "Please take the 5 taka to Dipta. He is waiting in the cafeteria.";
				}
				else {
					activeDialogueText = "Thanks for helping settle the 5 taka favor.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_AYON) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (ayonEscortState == AYON_ESCORT_FIND_AYON) {
					ayonEscortState = AYON_ESCORT_ESCORTING;
					activeDialogueText = "Rafi is looking for me? I'll follow you to the CSE classroom.";
				}
				else activeDialogueText = "I am studying in the library today.";
			}
			else if (npcs[i].missionRole == MISSION_ROLE_DIPTA) {
				if (takaMissionState == TAKA_MISSION_CARRYING_MONEY && takaCarried == 5) {
					activeDialogueKind = DIALOGUE_DIPTA_CHOICE;
					activeDialogueText = "Ayon amar 5 taka ekono de nai.Bhai amar arthik poristithi onek kharap!";
				}
				else if (takaMissionState == TAKA_MISSION_COMPLETE) {
					activeDialogueKind = DIALOGUE_NORMAL;
					activeDialogueText = "Bar bar tor mukh dekaccos ke?";
				}
				else {
					activeDialogueKind = DIALOGUE_NORMAL;
					activeDialogueText = "Ayon still owes me 5 taka for a Rickshaw Fair.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_LAB) {
				// Zahid Sir doesn't complete this quest himself - the player has
				// to actually pass the diagnostic mini-game on the reserved PC.
				activeDialogueKind = DIALOGUE_NORMAL;
				if (!quests[3].isComplete) {
					activeDialogueText = "One of the PCs on the far left keeps freezing. Sit down and press F to run a diagnostic.";
				}
				else if (!quests[4].isComplete) {
					activeDialogueText = "Nice work fixing that PC! Come find me in my office when you get a chance.";
				}
				else {
					activeDialogueText = "Everything's running smoothly now, thanks again.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_CLASSMATE) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (ayonEscortState == AYON_ESCORT_AT_CLASSROOM ||
					(quests[JAVA_ERROR_HUNT_QUEST_INDEX].isComplete && !quests[AYON_ESCORT_QUEST_INDEX].isComplete)) {
					if (ayonEscortState != AYON_ESCORT_AT_CLASSROOM) ayonEscortState = AYON_ESCORT_FIND_AYON;
					if (ayonEscortState == AYON_ESCORT_AT_CLASSROOM && !rafiBullyingResolved) {
						activeDialogueKind = DIALOGUE_RAFI_BULLY_CHOICE;
						activeDialogueText = "Rafi mocks Ayon for needing help. What should Ruel do?";
					}
					else if (!rafiBullyingResolved) activeDialogueText = "Ruel, please find Ayon in the library and bring him back to this classroom.";
					else activeDialogueText = "I'm sorry, Ayon. Thank you for helping us understand, Ruel.";
				}
				else if (!quests[1].isComplete) {
					activeDialogueText = "Finish your first mission, the Five Taka Favor, then we can study together.";
				}
				else if (!quests[CLASSROOM_COURTESY_QUEST_INDEX].isComplete) {
					activeDialogueKind = DIALOGUE_CLASSMATE_CHOICE;
					activeDialogueText = "Want to work through Java errors together? How do you reply?";
				}
				else {
					activeDialogueText = "Your study table is open. Good luck with the Java Error Hunt!";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_NISSAN) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (ayonEscortState == AYON_ESCORT_AT_CLASSROOM && rafiBullyingResolved && !quests[NISSAN_MIM_QUEST_INDEX].isComplete) {
					activeDialogueKind = DIALOGUE_NISSAN_MIM_CHOICE;
					activeDialogueText = "Mim is here. Nissan, how will you introduce yourself?";
				}
				else if (!quests[NISSAN_MIM_QUEST_INDEX].isComplete) {
					nissanConnectionStarted = true;
					activeDialogueText = "Ruel, I would like to meet Mim in the CSE classroom. Could you introduce us?";
				}
				else activeDialogueText = "Thank you for helping Mim and me connect.";
			}
			else if (npcs[i].missionRole == MISSION_ROLE_MIM) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (nissanConnectionStarted && !quests[NISSAN_MIM_QUEST_INDEX].isComplete) {
					activeDialogueText = "I'd be happy to meet Nissan. Bring him to the classroom after Rafi's group arrives.";
				}
				else if (!quests[NISSAN_MIM_QUEST_INDEX].isComplete) activeDialogueText = "I am focused on my CSE work right now.";
				else activeDialogueText = "Thanks for connecting Nissan and me, Ruel.";
			}
			else if (npcs[i].missionRole == MISSION_ROLE_ZAHID_OFFICE) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (!quests[3].isComplete) {
					activeDialogueText = "Come back after you've sorted out that PC in the lab.";
				}
				else if (!quests[4].isComplete) {
					completeQuest(4);
					activeDialogueText = "Great work on the lab task! Now go show Kazi Towfique Elahi your algorithms review.";
				}
				else {
					activeDialogueText = "Keep it up - Towfique sir is expecting you next.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_TOWFIQUE) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (!quests[4].isComplete) {
					activeDialogueText = "Come back once Zahid Hossain has sent you over.";
				}
				else if (!quests[5].isComplete) {
					completeQuest(5);
					activeDialogueText = "Good review! Next, get your database work signed off by Mr. Saha Reno.";
				}
				else {
					activeDialogueText = "See Mr. Saha Reno if you haven't already.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_SAHA) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (!quests[5].isComplete) {
					activeDialogueText = "Come back once Towfique sir has reviewed your work.";
				}
				else if (!quests[6].isComplete) {
					completeQuest(6);
					activeDialogueText = "Signed off. Now take your task to Md Reasad Zaman Chowdhury.";
				}
				else {
					activeDialogueText = "Reasad sir is waiting for you.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_REASAD) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (!quests[6].isComplete) {
					activeDialogueText = "Come back once Saha sir has signed off on your database work.";
				}
				else if (!quests[7].isComplete) {
					completeQuest(7);
					activeDialogueText = "Good submission! Last stop - present everything to Prof. Al Mamun.";
				}
				else {
					activeDialogueText = "Prof. Al Mamun is expecting your final report.";
				}
			}
			else if (npcs[i].missionRole == MISSION_ROLE_MAMUN) {
				activeDialogueKind = DIALOGUE_NORMAL;
				if (!quests[7].isComplete) {
					activeDialogueText = "Come back once the other teachers have signed off on your work.";
				}
				else if (!quests[8].isComplete) {
					completeQuest(8);
					activeDialogueText = "Congratulations! Your semester progress is fully approved.";
				}
				else {
					activeDialogueText = "Congratulations again on completing your requirements.";
				}
			}
			else if (activeDialogueKind == DIALOGUE_WAITER_QUESTION && waiterQuestionAnswered) {
				activeDialogueKind = DIALOGUE_NORMAL;
				activeDialogueText = "Thanks for answering my question earlier.";
			}
			else if (activeDialogueKind == DIALOGUE_CASHIER_QUESTION && cashierQuestionAnswered) {
				activeDialogueKind = DIALOGUE_NORMAL;
				activeDialogueText = "Thanks for paying. Enjoy your meal!";
			}
			// A conversation alone never earns reputation. Call completeQuest()
			// only when the player actually finishes a future quest objective.

			screenBeforeDialogue = currentScreen;
			currentScreen = SCREEN_DIALOGUE;
			return; // only interact with the first NPC in range
		}
	}
}

// Faculty-hall counterpart to tryInteract(): walking up to a door and
// pressing F steps straight into that teacher's office (no dialogue box -
// the room itself is the destination).
inline void tryEnterDoor(std::vector<Door>& doors) {
	double px = player.x + player.w / 2.0;
	double py = player.y + player.h / 2.0;
	for (unsigned int i = 0; i < doors.size(); i++) {
		double dx = doors[i].x + doors[i].w / 2.0;
		double dy = doors[i].y + doors[i].h / 2.0;
		if (getDistance(px, py, dx, dy) <= INTERACT_RADIUS) {
			currentScreen = doors[i].target;
			player.x = 300;
			player.y = 200;
			return; // only enter the first door in range
		}
	}
}

#endif