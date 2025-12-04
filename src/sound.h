#ifndef SOUND_H
#define SOUND_H

#include "defs.h"

// 효과음 초기화/종료 함수
void InitSound(void);
void CleanupSound(void);

// 효과음 로딩 함수
void LoadEffects(void);

// 효과음 플레이 함수
void PlaySlashSound(void);

// 폭탄 효과음 (신발·돌)
void PlayBombSound(void);

// BGM 재생 함수
void PlayBGM(void);

// BGM 정지 함수
void StopBGM(void);

// BGM 볼륨 설정 함수 (0-128 범위) 
void SetBGMVolume(int volume);

#endif