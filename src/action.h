/**
 * @file    action.h
 * @brief   재료의 움직임을 구현한 함수 선언
 * @author  조정배 팀
 *
 * 이 파일은 게임의 핵심 로직을 담당한다:
 * - 재료 생성 및 스폰 (난이도에 따른 스폰 간격 조절)
 * - 재료의 물리 연산 (중력, 충돌, 냄비에 쌓이기)
 * - 베기 충돌 감지 및 조각 생성
 * - 게임 상태 업데이트
 */
  
#ifndef ACTION_H
#define ACTION_H

#include "defs.h"
#include "utils.h"
#include "sound.h" 


// 현재 스폰 간격 계산하는 헬퍼 함수
static int GetCurrentSpawnInterval(void); 

// 스택(냄비) 관련 헬퍼 함수
static void AddToStack(Ingredient *ing);
static void AddToStackRandom(Ingredient *ing);

// 재료 생성 함수, 조각 생성 함수 
void SpawnIngredient(void);
void CreateFragment(Ingredient *parent, int part);

// 훠궈 재료들의 움직임(물리, 중력)을 업데이트하는 함수
// [수정] 배열([]) 대신 포인터(*) 사용
void ActIngredients(Ingredient *ingredients, int count);

// 베기 로직(마우스 이동)에 따른 충돌 감지 및 점수/목숨 처리 함수
// [수정] 배열([]) 대신 포인터(*) 사용
void CheckSlice(Ingredient *ingredients, int count, int x1, int y1, int x2, int y2);

// 모든 객체의 상태를 업데이트하고 충돌을 확인하는 메인 액션 함수
void update_game(void);

// 게임 재시작 함수
void ResetGame(void);

#endif // ACTION_H
