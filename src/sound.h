/**
 * @file    sound.h
 * @brief   게임 내 효과음 구현을 위한 함수 선언 
 * @author  조정배 팀
 */
#ifndef SOUND_H
#define SOUND_H

#include "defs.h"

// 효과음 핸들 (extern 선언 - 실제 정의는 sound.c에 있음)
extern Mix_Chunk *slash_sound;
extern Mix_Chunk *bomb_sound;
extern Mix_Chunk *gameover_sound;
extern Mix_Chunk *missingredient_sound;

// BGM 핸들
extern Mix_Music *bgm_music;

// 효과음 초기화/종료 함수
void InitSound(void);
void CleanupSound(void);

// 효과음 로딩 함수
void LoadEffects(void);

// 효과음 플레이 함수
void PlaySlashSound(void);

// 폭탄 효과음 (신발·돌)
void PlayBombSound(void);

// 게임 오버 효과음 (목숨 다 잃음)
void PlayGameOverSound(void);

// 재료 놓침 효과음 (재료가 땅에 떨어질 때)
void PlayMissingredientSound(void);

// BGM 재생 함수
void PlayBGM(void);

// BGM 정지 함수
void StopBGM(void);


#endif
