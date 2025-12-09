/**
 * @file    utils.c
 * @brief   게임 로직에 필요한 수학적 계산 및 충돌 감지 유틸리티 함수 정의
 * @author  조정배 팀
 * 
 * 이 파일은 게임에서 자주 사용되는 유틸리티 함수들을 제공한다:
 * - 선분-사각형 충돌 검사 (마우스 궤적과 재료의 충돌)
 * - 난수 생성 (정수, 실수)
 */
#include "utils.h"

/**
 * @brief   선분(마우스 궤적)과 사각형(재료)의 교차를 검사한다.
 * @param   x1 선분 시작점 x 좌표
 * @param   y1 선분 시작점 y 좌표
 * @param   x2 선분 끝점 x 좌표
 * @param   y2 선분 끝점 y 좌표
 * @param   rect 검사할 사각형 포인터
 * @return  충돌 시 1, 미충돌 시 0
 * 
 * SDL_IntersectRectAndLine 함수를 사용하여 정밀하게 판정한다.
 * 이 함수는 마우스로 재료를 베었는지 판단하는 데 사용된다.
 */
int CheckLineRectHit(int x1, int y1, int x2, int y2, const SDL_Rect *rect) {
    // SDL_IntersectRectAndLine 함수는 원본 좌표 변수를 수정하여 
    // '잘린 선분'을 반환하므로, 원본 좌표를 보호하기 위해 임시 변수를 사용한다.
    int tx1 = x1, ty1 = y1;
    int tx2 = x2, ty2 = y2;

    // SDL 내장 함수: 선분이 사각형과 조금이라도 겹치면 SDL_TRUE(1) 반환
    if (SDL_IntersectRectAndLine(rect, &tx1, &ty1, &tx2, &ty2)) {
        return 1;
    }
    return 0;
}

/**
 * @brief   정수 범위 내의 난수를 생성한다.
 * @param   min 최소값 (이상)
 * @param   max 최대값 (미만)
 * @return  min 이상 max 미만의 난수
 * 
 * 재료 타입 결정(0~5), 랜덤 오프셋 생성 등에 사용된다.
 */
int RandInt(int min, int max) {
    if (min >= max) return min; // 잘못된 범위 처리
    return (rand() % (max - min)) + min;
}

/**
 * @brief   실수 범위 내의 난수를 생성한다.
 * @param   min 최소값 (이상)
 * @param   max 최대값 (미만)
 * @return  min 이상 max 미만의 난수
 * 
 * 재료 발사 각도, 속도 등 연속적인 값이 필요한 경우에 사용된다.
 */
double RandDouble(double min, double max) {
    if (min >= max) return min; // 잘못된 범위 처리
    // 0.0 ~ 1.0 사이의 실수 난수 생성 후 범위에 맞게 선형 변환
    double scale = (double)rand() / (double)RAND_MAX;
    return min + scale * (max - min);
}
