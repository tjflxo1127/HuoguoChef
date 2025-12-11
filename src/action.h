/**
 * @file    action.h
 * @brief   재료의 움직임을 구현한 함수 선언
 * @author  조정배 팀
 */
#ifndef ACTION_H
#define ACTION_H

#include "defs.h"
#include "utils.h"
#include "sound.h" 


/**
 * @brief   현재 점수에 따라 스폰 간격을 계산하는 헬퍼 함수
 * @return  현재 난이도에 맞는 스폰 간격 (프레임 단위)
 * 
 * 점수가 높아질수록 스폰 간격이 짧아져 게임이 어려워진다.
 * DIFFICULTY_SCORE_RATE 점수마다 간격이 1프레임씩 감소한다.
 * 최소 간격(SPAWN_INTERVAL_MIN)은 보장된다.
 */
static int GetCurrentSpawnInterval(void); 


/**
 * @brief   잘린 재료를 냄비 스택에 추가하는 헬퍼 함수
 * @param   ing 스택에 추가할 재료 포인터
 * 
 * 재료의 현재 위치, 크기, 텍스처 정보를 스택에 저장한다.
 * 원형 버퍼를 사용하여 MAX_STACK_INGREDIENTS 개수만큼만 유지하며,
 * 초과 시 가장 오래된 재료부터 덮어쓴다.
 */
static void AddToStack(Ingredient *ing);


/**
 * @brief   화면 밖으로 나간 잘린 재료를 냄비 안 랜덤 위치에 강제 배치하는 헬퍼 함수
 * @param   ing 배치할 재료 포인터
 * 
 * 재료가 냄비를 빗나가 화면 밖으로 떨어졌을 때,
 * 냄비 범위 내의 랜덤 위치에 배치하여 시각적 연속성을 유지한다.
 */
static void AddToStackRandom(Ingredient *ing);


/**
 * @brief   새로운 재료를 화면 아래에서 생성하고 발사한다.
 * 
 * 활성화되지 않은 슬롯을 찾아 새로운 재료를 생성한다.
 * 재료는 화면 하단 중앙에서 약간의 랜덤 오프셋을 가지고
 * 60~120도 각도로 랜덤 속도로 발사된다.
 * 재료 타입은 랜덤하게 결정되며, 식재료 또는 함정(신발/돌)이 될 수 있다.
 */
void SpawnIngredient(void);


/**
 * @brief   베어진 재료로부터 좌우 조각을 생성한다.
 * @param   parent 베어진 원본 재료 포인터
 * @param   part 조각 번호 (1: 왼쪽 조각, 2: 오른쪽 조각)
 * 
 * 재료가 베어지면 원본은 제거되고 좌우 두 개의 조각이 생성된다.
 * 각 조각은 잘린 텍스처를 사용하며, 좌우로 퍼지며 떨어지는 물리 효과를 받는다.
 * 조각은 냄비에 떨어지면 스택에 쌓인다.
 */
void CreateFragment(Ingredient *parent, int part);


/**
 * @brief   모든 활성 재료의 물리 연산 및 상태 업데이트를 수행한다.
 * @param   ingredients 재료 배열 포인터
 * @param   count 재료 배열 크기
 * 
 * 이 함수는 매 프레임 호출되어 다음을 처리한다:
 * - 중력 적용 및 위치 업데이트
 * - 좌우 벽 충돌 처리 (튕기기)
 * - 냄비 범위 내 잘린 재료를 스택에 추가
 * - 화면 밖으로 나간 재료 처리 (목숨 차감 또는 강제 배치)
 */
void ActIngredients(Ingredient *ingredients, int count);


/**
 * @brief   마우스 궤적(칼날)과 재료의 충돌을 검사하고 베기 처리를 한다.
 * @param   ingredients 재료 배열 포인터
 * @param   count 재료 배열 크기
 * @param   x1 마우스 궤적 시작점 x 좌표
 * @param   y1 마우스 궤적 시작점 y 좌표
 * @param   x2 마우스 궤적 끝점 x 좌표
 * @param   y2 마우스 궤적 끝점 y 좌표
 * 
 * 마우스가 움직일 때 호출되어 선분(마우스 궤적)과 재료 사각형의 교차를 검사한다.
 * 일반 재료는 베어서 점수를 얻고, 함정(신발/돌)은 게임 오버를 유발한다.
 */
void CheckSlice(Ingredient *ingredients, int count, int x1, int y1, int x2, int y2);


/**
 * @brief   모든 객체의 상태를 업데이트하고 충돌을 확인하는 메인 액션 함수
 * 
 * 이 함수는 게임 루프에서 매 프레임 호출된다.
 * 타이틀 화면이나 게임 오버 상태에서는 업데이트를 수행하지 않는다.
 * - 난이도에 따른 재료 스폰
 * - 모든 재료의 물리 연산 및 상태 업데이트
 */
void update_game(void);


/**
 * @brief   게임 상태를 초기 상태로 리셋한다.
 * 
 * 게임 오버 후 재시작 시 또는 처음 시작할 때 호출된다.
 * 점수, 목숨, 모든 재료와 스택을 초기화하고 BGM을 재생한다.
 */
void ResetGame(void);

#endif 
