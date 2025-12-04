/**
 * @file      utils.c
 * @brief     게임 로직에 필요한 수학적 계산 및 충돌 감지 유틸리티 구현
 */

#include "utils.h"

// 선분(궤적)과 사각형(재료)의 교차 판정
// SDL_IntersectRectAndLine 함수를 사용하여 정밀하게 판정합니다.
int CheckLineRectHit(int x1, int y1, int x2, int y2, const SDL_Rect *rect) {
    // SDL_IntersectRectAndLine 함수는 원본 좌표 변수를 수정하여 
    // '잘린 선분'을 반환하므로, 원본 좌표를 보호하기 위해 임시 변수를 사용합니다.
    int tx1 = x1, ty1 = y1;
    int tx2 = x2, ty2 = y2;

    // SDL 내장 함수: 선분이 사각형과 조금이라도 겹치면 SDL_TRUE(1) 반환
    if (SDL_IntersectRectAndLine(rect, &tx1, &ty1, &tx2, &ty2)) {
        return 1;
    }
    return 0;
}

// 재료가 화면 밖(특히 바닥)으로 나갔는지 확인
// 재료가 바닥으로 떨어졌을 때(목숨 차감 등)를 감지합니다.
int CheckOutBound(Ingredient *ing) {
    // 화면 아래쪽(SCREEN_HEIGHT)보다 더 아래로 내려갔는지 확인
    // (완전히 사라지게 하기 위해 여유분 50픽셀 정도를 둡니다)
    if (ing->y > SCREEN_HEIGHT + 50) {
        return 1; // 화면 밖으로 나감
    }
    
    // (선택 사항) 좌우나 위쪽으로 너무 멀리 날아간 경우도 제거하고 싶다면 아래 주석 해제
    /*
    if (ing->x < -100 || ing->x > SCREEN_WIDTH + 100 || ing->y < -500) {
        return 1;
    }
    */

    return 0; // 아직 화면 안에 있거나 유효한 범위임
}

// 정수 난수 생성 (min 이상 max 미만)
// 재료 타입 결정(0~5) 등에 사용합니다.
int RandInt(int min, int max) {
    if (min >= max) return min;
    return (rand() % (max - min)) + min;
}

// 실수 난수 생성 (min 이상 max 미만)
double RandDouble(double min, double max) {
    if (min >= max) return min;
    // 0.0 ~ 1.0 사이의 실수 난수 생성 후 범위에 맞게 변환
    double scale = (double)rand() / (double)RAND_MAX;
    return min + scale * (max - min);
}

// Degree -> Radian 변환
double DegToRad(double degrees) {
    return degrees * (PI / 180.0);
}