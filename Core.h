#ifndef _CORE_H_
#define _CORE_H_
#pragma warning(disable : 28251) //C28251

/***
	< ������Ʈ �Ӽ� >
	
	1) ���� �Ӽ�-���-���� ���� : �����ڵ� ���� ���� ���
	2) ���� �Ӽ�-��Ŀ-�ý���-���� �ý��� : â(/SUBSYSTEM:WINDOWS)
***/

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

/***
	2024-05-21 : ������ ����
	�ܺ� �ֿܼ��� ȣ��� ������ �ƴ� ���� ���� �� ���� ���μ��� ����Ǳ� �� cmd â�� ��� Flashing�Ǿ� 
	���� ���� �� ���� �� â�� ����� ����

	1. Ÿ�� SDK ���� ���׷��̵�
	2. �θ� �ܼ� (ȣ����)�� ���� ������ ��� �ܼ� �Ҵ� ���� ����
	
	---

	���� : https://github.com/wenshui2008/RunHiddenConsole
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