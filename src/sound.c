#include "sound.h"
#include <stdio.h>

// 효과음 핸들
static Mix_Chunk *slash_sound = NULL;
static Mix_Chunk *bomb_sound = NULL;

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
}

void PlaySlashSound(void) {
    if (slash_sound) Mix_PlayChannel(-1, slash_sound, 0);
}

void PlayBombSound(void) {
    if (bomb_sound) Mix_PlayChannel(-1, bomb_sound, 0);
}

void CleanupSound(void) {
    if (slash_sound) Mix_FreeChunk(slash_sound);
    if (bomb_sound) Mix_FreeChunk(bomb_sound);
    Mix_CloseAudio();
}
