#include "Core.h"

/// <summary>
/// 현재 프로세스의 이름과 확장자 반환
/// </summary>
/// <returns>현재 프로세스의 이름과 확장자</returns>
const TCHAR* utils::GetSelfProcessNameWithExt()
{
	static TCHAR selfProcessName[FILENAME_MAX] = { NULL, };
	static BOOL isSelfProcessNameAssigned = false;

	if (!isSelfProcessNameAssigned)
	{
		const TCHAR* sep = _T("\\"); //구분자
		TCHAR* token = NULL;
		TCHAR* context = NULL; //분리 된 문자열에 대한 두 번째 문자열의 시작 주소

		TCHAR fullPathBuffer[MAX_PATH] = { NULL, }; //전체 경로 버퍼

		// https://docs.microsoft.com/ko-kr/windows/win32/api/psapi/nf-psapi-getprocessimagefilenamea?redirectedfrom=MSDN
		GetProcessImageFileName(GetCurrentProcess(), fullPathBuffer, MAX_PATH); //전체 경로를 포함한 이름 할당
		token = _tcstok_s(fullPathBuffer, sep, &context); //구분자로 분리

		while (token != NULL)
		{
			token = _tcstok_s(NULL, sep, &context);
			if (context != NULL && (*context) == NULL) //더 이상 분리 할 문자가 없으면
				break;
		}

		if (token == NULL)
			throw std::runtime_error(std::string(__func__) + std::string(" : 현재 프로세스의 이름과 확장자 획득 실패"));

		_tcscpy_s(selfProcessName, token);
		isSelfProcessNameAssigned = true;

	}

	return selfProcessName;
}

/// <summary>
/// 현재 프로세스를 콘솔에 연결
/// </summary>
/// <returns>true : 성공, false : 실패</returns>
bool utils::AttachToConsole()
{
	// https://stackoverflow.com/questions/30098229/win32-application-write-output-to-console-using-printf
	if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) //부모 콘솔에 연결 실패할 경우
	{
		if (GetLastError() != ERROR_ACCESS_DENIED) //현재 프로세스가 콘솔에 이미 연결되어 있으면
		{
			if (AttachConsole(GetCurrentProcessId()) == 0) //현재 프로세스에 연결 실패할 경우
			{
				if (GetLastError() != ERROR_ACCESS_DENIED) //현재 프로세스가 콘솔에 이미 연결되어 있으면
					return false;
			}
		}
	}
	else
	{
		// std::cout, std::clog, std::cerr, std::cin
		FILE* fDummy;
		freopen_s(&fDummy, "CONOUT$", "w", stdout);
		freopen_s(&fDummy, "CONOUT$", "w", stderr);
		freopen_s(&fDummy, "CONIN$", "r", stdin);

		std::cout.clear();
		std::clog.clear();
		std::cerr.clear();
		std::cin.clear();

		// std::wcout, std::wclog, std::wcerr, std::wcin
		HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
		SetStdHandle(STD_ERROR_HANDLE, hConOut);
		SetStdHandle(STD_INPUT_HANDLE, hConIn);

		std::wcout.clear();
		std::wclog.clear();
		std::wcerr.clear();
		std::wcin.clear();
	}

	return true;
}

/// <summary>
/// 새 콘솔 할당 및 표준 입력, 표준 출력, 표준 오류 스트림 재 설정
/// </summary>
/// <returns>true : 성공, false : 실패</returns>
bool utils::AttachToNewConsole()
{
	DWORD errorCode = AllocConsole(); //새 콘솔 할당
	if (errorCode == 0)
		return false;

	//링커-하위 시스템 창(/SUBSYSTEM:WINDOWS)로 빌드 시 콘솔 입력 및 출력이 초기화되지 않음
	// https://learn.microsoft.com/en-us/cpp/build/reference/subsystem-specify-subsystem?redirectedfrom=MSDN&view=msvc-170
	// https://learn.microsoft.com/en-us/windows/console/console-screen-buffers

	// std::cout, std::clog, std::cerr, std::cin
	FILE* fDummy;
	freopen_s(&fDummy, "CONOUT$", "w", stdout);
	freopen_s(&fDummy, "CONOUT$", "w", stderr);
	freopen_s(&fDummy, "CONIN$", "r", stdin);
	
	std::cout.clear();
	std::clog.clear();
	std::cerr.clear();
	std::cin.clear();
	
	// std::wcout, std::wclog, std::wcerr, std::wcin
	HANDLE hConOut = CreateFile(_T("CONOUT$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE hConIn = CreateFile(_T("CONIN$"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	SetStdHandle(STD_OUTPUT_HANDLE, hConOut);
	SetStdHandle(STD_ERROR_HANDLE, hConOut);
	SetStdHandle(STD_INPUT_HANDLE, hConIn);
	
	std::wcout.clear();
	std::wclog.clear();
	std::wcerr.clear();
	std::wcin.clear();
	
	return (errorCode != 0) ? true : false;
}

/// <summary>
/// 멀티바이트 문자열 (MBCS)을 유니코드 문자열 (한 문자당 2바이트)로 변환
/// </summary>
/// <param name="srcStr">멀티바이트 문자열 (MBCS)</param>
/// <returns>변환 된 유니코드 문자열 (한 문자당 2바이트)</returns>
wchar_t* utils::CharToWChar(const char* srcStr)
{
	if (srcStr == NULL)
		throw std::invalid_argument(std::string(__func__) + std::string(" : Invalid Args"));

	wchar_t* retVal = new wchar_t[strlen(srcStr) + 1];
	size_t numOfCharConverted = 0; //변환 된 문자 수

	// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mbstowcs-s-mbstowcs-s-l?view=msvc-170
	mbstowcs_s(&numOfCharConverted, retVal, strlen(srcStr) + 1,
		srcStr, strlen(srcStr));

	return retVal;
}

/// <summary>
/// 유니코드 문자열 (한 문자당 2바이트)을 멀티바이트 문자열 (MBCS)로 변환
/// </summary>
/// <param name="srcStr">유니코드 문자열 (한 문자당 2바이트)</param>
/// <returns>변환 된 멀티바이트 문자열 (MBCS)</returns>
char* utils::WCharToChar(const wchar_t* srcStr)
{
	if (srcStr == NULL)
		throw std::invalid_argument(std::string(__func__) + std::string(" : Invalid Args"));

	char* retVal = new char[wcslen(srcStr) + 1];
	size_t numOfWCharConverted = 0; //변환 된 문자 수

	// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mbstowcs-s-mbstowcs-s-l?view=msvc-170
	wcstombs_s(&numOfWCharConverted, retVal, wcslen(srcStr) + 1,
		srcStr, wcslen(srcStr));

	return retVal;
}