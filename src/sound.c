/**
 * @file    sound.c
 * @brief   게임 내 효과음 구현을 위한 함수 정의 
 * @author  조정배 팀
  * 
 * 이 파일은 SDL_mixer를 사용하여 게임의 사운드를 처리한다
 * - 효과음 초기화 및 리소스 로드
 * - 게임 상황에 맞는 효과음 재생
 */
#include "sound.h"

// 효과음 핸들 (실제 정의 - sound.h에서 extern으로 선언됨)
Mix_Chunk *slash_sound = NULL;         // 재료 베기 효과음
Mix_Chunk *bomb_sound = NULL;          // 함정(신발/돌) 접촉 효과음
Mix_Chunk *gameover_sound = NULL;      // 게임 오버 효과음
Mix_Chunk *missingredient_sound = NULL; // 재료 놓침 효과음

// BGM 핸들 (배경음악)
Mix_Music *bgm_music = NULL; 

void InitSound(void) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 512) < 0) { //44.1kHz 샘플레이트, 스테레오(채널 2), 512 바이트를 버퍼 크기로 설정 (재료를 베고 나서 소리가 로드되는데 버퍼링이 생기길래 버퍼의 크기를 조절하면서 반응속도를 결정하였따.)
        printf("SDL_mixer init error: %s\n", Mix_GetError()); //오디오 초기화 실패 시 에러 메시지 출력
    }
}

void LoadEffects(void) { //모든 효과음 파일 로드
    slash_sound = Mix_LoadWAV("./sound/slash.wav"); //리소스 로딩
    if (!slash_sound) printf("Slash sound load error: %s\n", Mix_GetError()); //리소스 로딩 실패시 에러 메세지 출력

    bomb_sound = Mix_LoadWAV("./sound/bomb.wav");
    if (!bomb_sound) printf("Bomb sound load error: %s\n", Mix_GetError());

    gameover_sound = Mix_LoadWAV("./sound/gameover.wav");
    if (!gameover_sound) printf("GameOver sound load error: %s\n", Mix_GetError());

    missingredient_sound = Mix_LoadWAV("./sound/missingredient.wav");
    if (!missingredient_sound) printf("Missingredient sound load error: %s\n", Mix_GetError());

    bgm_music = Mix_LoadMUS("./sound/BGM.wav");
    if (!bgm_music) printf("BGM load error: %s\n", Mix_GetError());
}

void PlaySlashSound(void) { //재료를 벤 효과음 재생
    if (slash_sound) Mix_PlayChannel(-1, 생
}

void PlayBombSound(void) { //돌, 신발을 벤 효과음 재생
    if (bomb_sound) Mix_PlayChannel(-1, bomb_sound, 0);
}

void PlayGameOverSound(void) { //게임 오버 효과음 재생
    if (gameover_sound) Mix_PlayChannel(-1, gameover_sound, 0);
}

void PlayMissingredientSound(void) { //재료를 놓폇을때 효과음 재생
    if (missingredient_sound) Mix_PlayChannel(-1, missingredient_sound, 0);
}

void PlayBGM(void) { //배경음악 반복 재생 (게임 시작할때 - R키를 눌러 재시작하였을 때 포함)
    if (bgm_music) Mix_PlayMusic(bgm_music, -1); //무한 반복을 위해 인자로 -1 전달 
}

void StopBGM(void) { //배경음악 재생 중지 (게임이 종료되었을때)
    Mix_HaltMusic();
}

void SetBGMVolume(int volume) { //배경음악의 소리가 너무 커서 줄이는 함수를 만들었음 배경음악의 볼륨을 조절하는 함수(0~128 범위에서 조절 가능)
    if (volume < 0) volume = 0;
    if (volume > 128) volume = 128;
    Mix_VolumeMusic(volume);
}

void CleanupSound(void) { //모든 사운드 리소스 해제하고 오디오 시스템 종료 (게임 종료 시에 호출해서 메모리 누수를 방지한다)
    if (bomb_sound) Mix_FreeChunk(bomb_sound);
    if (gameover_sound) Mix_FreeChunk(gameover_sound);
    if (missingredient_sound) Mix_FreeChunk(missingredient_sound);
    if (bgm_music) Mix_FreeMusic(bgm_music);
    Mix_CloseAudio();
}
