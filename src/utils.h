/**
 * @file    utils.h
 * @brief   게임 로직에 필요한 수학적 계산 및 충돌 감지 유틸리티 함수 선언
 * @author  조정배 팀
 */
#ifndef UTILS_H
#define UTILS_H

#include "defs.h"

//충돌 감지 함수
int CheckLineRectHit(int x1, int y1, int x2, int y2, const SDL_Rect *rect);

//정수 난수 생성 함수
int RandInt(int min, int max);

//실수 난수 생성 함
double RandDouble(double min, double max);


#endif
