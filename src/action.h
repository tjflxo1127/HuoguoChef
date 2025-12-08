#ifndef ACTION_H
#define ACTION_H

#include "defs.h"
#include "utils.h"
#include "sound.h" 

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