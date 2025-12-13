/**
 * @file    utils.c
 * @brief   게임 로직에 필요한 수학적 계산 및 충돌 감지 유틸리티 함수 정의
 * @author  조정배 팀
 */
#include "utils.h"

// 선분(궤적)과 사각형(재료)가 겹치는지 판정
int CheckLineRectHit(int x1, int y1, int x2, int y2, const SDL_Rect *rect) {
   // SDL_IntersectRectAndLine 함수 : 원본 좌표 변수를 수정
    // 임시 변수를 사용
    int tx1 = x1, ty1 = y1;
    int tx2 = x2, ty2 = y2;

    // SDL 라이브러리 함수: 선분이 사각형과 조금이라도 겹치면 1 반환
    if (SDL_IntersectRectAndLine(rect, &tx1, &ty1, &tx2, &ty2)) {
        return 1;
    }
    return 0;
}

// 정수 난수 생성 : 재료 타입 결정, 랜덤 위치 저장 등에 사용
int RandInt(int min, int max) {
    if (min >= max)
        return min;

    // 정수 생성 후 범위에 맞게 조정 후 반환
    return (rand() % (max - min)) + min;
}

// 실수 난수 생성 : 각도와 속도에 사용
double RandDouble(double min, double max) {
    if (min >= max)
        return min;

    double scale = (double)rand() / (double)RAND_MAX; // scale은 0.0 ~ 1.0 사이의 랜덤 실수 
    return min + scale * (max - min); // 범위에 맞게 조정 후 반환
}
