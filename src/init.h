/**
 * @file    init.h
 * @brief   게임 객체 및 SDL 요소 초기화 함수 선언
 * @author  조정배 팀
 */
#ifndef INIT_H
#define INIT_H

#include "defs.h"
#include "sound.h"
#include "action.h"
#include "draw.h"
#include "input.h"

/**
 * @brief SDL, 리소스, 게임 객체를 초기화한다.
 */
void init_sdl(void);

/**
 * @brief UI 텍스트 객체를 초기화하고 배치한다.
 */
void InitUI();

/**
 * @brief 재료 이미지와 관련 리소스를 로드한다.
 */
void InitIngredient(void);

/**
 * @brief 마우스 궤적 버퍼를 초기화한다.
 */
void InitTrail(void);

/**
 * @brief 모든 리소스를 해제하고 SDL을 종료한다.
 */
void cleanup_sdl(void);

#endif
