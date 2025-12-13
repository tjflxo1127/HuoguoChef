/**
 * @file    utils.c
 * @brief   게임 로직에 필요한 수학적 계산 및 충돌 감지 유틸리티 함수 정의
 * @author  조정배 팀
 * 
 * 이 파일은 게임에서 자주 사용되는 유틸리티 함수들을 제공한다:
 * - 선분-사각형 충돌 검사 (마우스 궤적과 재료의 충돌)
 * - 난수 생성 (정수, 실수)
 */
#ifndef UTILS_H
#define UTILS_H

#include "defs.h"


/** 충돌 감지 함수
 * @brief   선분(마우스 궤적)과 사각형(재료)의 교차를 검사한다.
 * @param   x1 선분 시작점 x 좌표
 * @param   y1 선분 시작점 y 좌표
 * @param   x2 선분 끝점 x 좌표
 * @param   y2 선분 끝점 y 좌표
 * @param   rect 검사할 사각형 포인터
 * @return  충돌 시 1, 미충돌 시 0
 * 
 * SDL_IntersectRectAndLine 함수를 사용하여 판정한다.
 * 이 함수는 마우스로 재료를 베었는지 판단하는 데 사용된다.
 */
int CheckLineRectHit(int x1, int y1, int x2, int y2, const SDL_Rect *rect);


/**
 * @brief   정수 범위 내의 난수를 생성한다.
 * @param   min 최소값 (이상)
 * @param   max 최대값 (미만)
 * @return  min 이상 max 미만의 난수
 * 
 * 재료 타입 결정(0~5), 랜덤 오프셋 생성 등에 사용된다.
 */
int RandInt(int min, int max);


/**
 * @brief   실수 범위 내의 난수를 생성한다.
 * @param   min 최소값 (이상)
 * @param   max 최대값 (미만)
 * @return  min 이상 max 미만의 난수
 * 
 * 재료 발사 각도, 속도 등 연속적인 값이 필요한 경우에 사용된다.
 */
double RandDouble(double min, double max);


#endif
