#include "sound.h"
#include <stdio.h>

// SDL_mixer 효과음 핸들
static Mix_Chunk *slash_sound = NULL;

void InitSound(void) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) < 0) {
        printf("SDL_mixer init error: %s\n", Mix_GetError());
    }
}

void LoadEffects(void) {
    slash_sound = Mix_LoadWAV("./sound/slice.wav"); 
    if (!slash_sound) printf("Slash sound load error: %s\n", Mix_GetError());
}

void PlaySlashSound(void) {
    if (slash_sound) Mix_PlayChannel(-1, slash_sound, 0);
}

void CleanupSound(void) {
    if (slash_sound) Mix_FreeChunk(slash_sound);
    Mix_CloseAudio();
}
