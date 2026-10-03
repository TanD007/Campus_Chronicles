#ifndef AUDIO_H
#define AUDIO_H

#include <windows.h>
#pragma comment(lib, "winmm.lib")

// ---------------- Background music ----------------
// Splash/menu theme, played via Windows' built-in MCI player (same
// mechanism the commented-out lines in main() were already pointing at).
// No extra audio library needed on Windows - just make sure winmm is
// linked (see note below) and that the file exists at:
//     Audios//background.mp3
// (sibling "Audios" folder to the "Images" folder used by loadAssets()
// in Assets.hpp).
//
// If you're building in Code::Blocks/MinGW and get a link error like
// "undefined reference to mciSendStringA", the #pragma comment below
// doesn't work with that toolchain - instead go to
// Project -> Build options -> Linker settings -> Add, and add: winmm

static bool bgMusicPlaying = false;

// Starts looping playback if it isn't already playing. Safe to call
// repeatedly (e.g. every frame) - it no-ops once music is running.
inline void playBackgroundMusic() {
	if (bgMusicPlaying) return;
	mciSendString("open \"Audios//background.mp3\" type mpegvideo alias bgMusic", NULL, 0, NULL);
	mciSendString("play bgMusic repeat", NULL, 0, NULL);
	bgMusicPlaying = true;
}

// Stops and releases the MCI device. Safe to call even if nothing is
// playing.
inline void stopBackgroundMusic() {
	if (!bgMusicPlaying) return;
	mciSendString("stop bgMusic", NULL, 0, NULL);
	mciSendString("close bgMusic", NULL, 0, NULL);
	bgMusicPlaying = false;
}

// ---------------- Game-over sound effect ----------------
// Short one-shot stinger played when the earthquake drill's roof-collapse
// game-over screen appears. Expected at:
//     Audios//gameover.mp3
// (same "Audios" folder as background.mp3 above).

static bool gameOverSoundPlaying = false;

// Plays Audios//gameover.mp3 once. Safe to call every frame while the
// game-over screen is up - the guard flag means it only actually starts
// playback the first time, not on every subsequent frame.
inline void playGameOverSound() {
	if (gameOverSoundPlaying) return;
	gameOverSoundPlaying = true;
	mciSendString("open \"Audios//gameover.mp3\" type mpegvideo alias gameOverSound", NULL, 0, NULL);
	mciSendString("play gameOverSound", NULL, 0, NULL);
}

// Releases the MCI device and clears the guard so the next game over plays
// the clip again. Call this when leaving the game-over screen (i.e. right
// before restarting).
inline void stopGameOverSound() {
	if (!gameOverSoundPlaying) return;
	mciSendString("stop gameOverSound", NULL, 0, NULL);
	mciSendString("close gameOverSound", NULL, 0, NULL);
	gameOverSoundPlaying = false;
}

#endif