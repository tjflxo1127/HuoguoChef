#ifndef ACTION_H
#define ACTION_H

#include "defs.h"

// 모든 객체의 상태를 업데이트하고 충돌을 확인하는 메인 액션 함수
void ActGame(void);

// 훠궈 재료들의 움직임(물리, 중력)을 업데이트하는 함수
// [수정] 배열([]) 대신 포인터(*) 사용
void ActIngredients(Ingredient *ingredients, int count);

// 베기 로직(마우스 이동)에 따른 충돌 감지 및 점수/목숨 처리 함수
// [수정] 배열([]) 대신 포인터(*) 사용
void CheckSlice(Ingredient *ingredients, int count, int x1, int y1, int x2, int y2);

// 재료 생성 함수
void SpawnIngredient(void);

// 게임 오버 상태일 때의 로직
void ActGameOver(void);
// [추가] 게임 재시작 함수
void ResetGame(void);

#endif // ACTION_H