/**
 * @file    sound.h
 * @brief   게임 내 효과음 구현을 위한 함수 선언 
 * @author  조정배 팀
 */
#ifndef SOUND_H
#define SOUND_H

#include "defs.h"

// 효과음 핸들
static Mix_Chunk *slash_sound = NULL;
static Mix_Chunk *bomb_sound = NULL;
static Mix_Chunk *gameover_sound = NULL;
static Mix_Chunk *missingredient_sound = NULL;

// BGM 핸들
static Mix_Music *bgm_music = NULL;

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

// BGM 볼륨 설정 함수 (0-128 범위) 
void SetBGMVolume(int volume);

#endif