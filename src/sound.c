#include <stdio.h>
#include "sound.h"

void InitSound(void) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) {
        printf("SDL_mixer init error: %s\n", Mix_GetError());
    }
}

void LoadEffects(void) {
    slash_sound = Mix_LoadWAV("./sound/slash.wav");
    if (!slash_sound) printf("Slash sound load error: %s\n", Mix_GetError());

    bomb_sound = Mix_LoadWAV("./sound/bomb.wav");
    if (!bomb_sound) printf("Bomb sound load error: %s\n", Mix_GetError());

    gameover_sound = Mix_LoadWAV("./sound/gameover.wav");
    if (!gameover_sound) printf("GameOver sound load error: %s\n", Mix_GetError());

    missingredient_sound = Mix_LoadWAV("./sound/missingredient.wav");
    if (!missingredient_sound) printf("Missingredient sound load error: %s\n", Mix_GetError());

    bgm_music = Mix_LoadMUS("./sound/BGM.wav");
    if (!bgm_music) printf("BGM load error: %s\n", Mix_GetError());
}

void PlaySlashSound(void) {
    if (slash_sound) Mix_PlayChannel(-1, slash_sound, 0);
}

void PlayBombSound(void) {
    if (bomb_sound) Mix_PlayChannel(-1, bomb_sound, 0);
}

void PlayGameOverSound(void) {
    if (gameover_sound) Mix_PlayChannel(-1, gameover_sound, 0);
}

void PlayMissingredientSound(void) {
    if (missingredient_sound) Mix_PlayChannel(-1, missingredient_sound, 0);
}

void PlayBGM(void) {
    if (bgm_music) Mix_PlayMusic(bgm_music, -1);
}

void StopBGM(void) {
    Mix_HaltMusic();
}

void SetBGMVolume(int volume) {
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    Mix_VolumeMusic(volume);
}

void CleanupSound(void) {
    if (slash_sound) Mix_FreeChunk(slash_sound);
    if (bomb_sound) Mix_FreeChunk(bomb_sound);
    if (gameover_sound) Mix_FreeChunk(gameover_sound);
    if (missingredient_sound) Mix_FreeChunk(missingredient_sound);
    if (bgm_music) Mix_FreeMusic(bgm_music);
    Mix_CloseAudio();
}