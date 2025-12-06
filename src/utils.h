/**
 * @file      utils.h
 * @brief     게임 로직에 필요한 수학적 계산 및 충돌 감지 유틸리티 선언
 */

#ifndef UTILS_H
#define UTILS_H

#include "defs.h"

// [충돌 감지 함수]

/**
 * @brief 선분(마우스 궤적)이 사각형(재료)과 충돌했는지 확인합니다.
 * 마우스를 빠르게 움직였을 때 재료를 통과하는 현상(터널링)을 방지합니다.
 * @param x1, y1 선분의 시작점 (이전 마우스 위치)
 * @param x2, y2 선분의 끝점 (현재 마우스 위치)
 * @param rect 확인할 사각형 영역
 * @return 1(충돌), 0(충돌 안 함)
 */
int CheckLineRectHit(int x1, int y1, int x2, int y2, const SDL_Rect *rect);

/**
 * @brief 재료가 화면 밖(바닥)으로 나갔는지 확인합니다.
 * 재료가 바닥으로 떨어졌을 때 목숨을 차감하거나 비활성화하기 위해 사용합니다.
 * @return 1(화면 밖), 0(화면 안)
 */
int CheckOutBound(Ingredient *ing);


// [난수 생성 함수]

/**
 * @brief [min, max) 범위의 정수 난수를 반환합니다.
 * 예: RandInt(0, 3) -> 0, 1, 2 중 하나 반환
 */
int RandInt(int min, int max);

/**
 * @brief [min, max) 범위의 실수 난수를 반환합니다.
 * 재료의 발사 각도나 속도를 미세하게 조절할 때 사용합니다.
 */
double RandDouble(double min, double max);


// [수학 보조 함수]

/**
 * @brief 각도(Degree)를 라디안(Radian)으로 변환합니다.
 * C언어의 sin, cos 함수는 라디안 값을 받기 때문에 필수적입니다.
 */
double DegToRad(double degrees);

#endif