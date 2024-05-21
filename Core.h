#ifndef _CORE_H_
#define _CORE_H_
#pragma warning(disable : 28251) //C28251

/***
	< 프로젝트 속성 >
	
	1) 구성 속성-고급-문자 집합 : 유니코드 문자 집합 사용
	2) 구성 속성-링커-시스템-하위 시스템 : 창(/SUBSYSTEM:WINDOWS)
***/

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

/***
	2024-05-21 : 김형준 수정
	외부 콘솔에서 호출로 실행이 아닌 직접 실행 시 하위 프로세스 실행되기 전 cmd 창이 잠시 Flashing되어 
	파일 배포 및 실행 시 창을 숨기기 위함

	1. 타겟 SDK 버전 업그레이드
	2. 부모 콘솔 (호출자)에 연결 실패할 경우 콘솔 할당 하지 않음
	
	---

	원본 : https://github.com/wenshui2008/RunHiddenConsole
***/

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <io.h>
#include <locale.h>
#include <iostream>

#include "Utils.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int);
#endif