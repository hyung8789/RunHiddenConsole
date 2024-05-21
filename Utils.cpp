#include "Core.h"

/// <summary>
/// ���� ���μ����� �̸��� Ȯ���� ��ȯ
/// </summary>
/// <returns>���� ���μ����� �̸��� Ȯ����</returns>
const TCHAR* utils::GetSelfProcessNameWithExt()
{
	static TCHAR selfProcessName[FILENAME_MAX] = { NULL, };
	static BOOL isSelfProcessNameAssigned = false;

	if (!isSelfProcessNameAssigned)
	{
		const TCHAR* sep = _T("\\"); //������
		TCHAR* token = NULL;
		TCHAR* context = NULL; //�и� �� ���ڿ��� ���� �� ��° ���ڿ��� ���� �ּ�

		TCHAR fullPathBuffer[MAX_PATH] = { NULL, }; //��ü ��� ����

		// https://docs.microsoft.com/ko-kr/windows/win32/api/psapi/nf-psapi-getprocessimagefilenamea?redirectedfrom=MSDN
		GetProcessImageFileName(GetCurrentProcess(), fullPathBuffer, MAX_PATH); //��ü ��θ� ������ �̸� �Ҵ�
		token = _tcstok_s(fullPathBuffer, sep, &context); //�����ڷ� �и�

		while (token != NULL)
		{
			token = _tcstok_s(NULL, sep, &context);
			if (context != NULL && (*context) == NULL) //�� �̻� �и� �� ���ڰ� ������
				break;
		}

		if (token == NULL)
			throw std::runtime_error(std::string(__func__) + std::string(" : ���� ���μ����� �̸��� Ȯ���� ȹ�� ����"));

		_tcscpy_s(selfProcessName, token);
		isSelfProcessNameAssigned = true;

	}

	return selfProcessName;
}

/// <summary>
/// ���� ���μ����� �ֿܼ� ����
/// </summary>
/// <returns>true : ����, false : ����</returns>
bool utils::AttachToConsole()
{
	// https://stackoverflow.com/questions/30098229/win32-application-write-output-to-console-using-printf
	if (AttachConsole(ATTACH_PARENT_PROCESS) == 0) //�θ� �ֿܼ� ���� ������ ���
	{
		if (GetLastError() != ERROR_ACCESS_DENIED) //���� ���μ����� �ֿܼ� �̹� ����Ǿ� ������
		{
			if (AttachConsole(GetCurrentProcessId()) == 0) //���� ���μ����� ���� ������ ���
			{
				if (GetLastError() != ERROR_ACCESS_DENIED) //���� ���μ����� �ֿܼ� �̹� ����Ǿ� ������
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
/// �� �ܼ� �Ҵ� �� ǥ�� �Է�, ǥ�� ���, ǥ�� ���� ��Ʈ�� �� ����
/// </summary>
/// <returns>true : ����, false : ����</returns>
bool utils::AttachToNewConsole()
{
	DWORD errorCode = AllocConsole(); //�� �ܼ� �Ҵ�
	if (errorCode == 0)
		return false;

	//��Ŀ-���� �ý��� â(/SUBSYSTEM:WINDOWS)�� ���� �� �ܼ� �Է� �� ����� �ʱ�ȭ���� ����
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
/// ��Ƽ����Ʈ ���ڿ� (MBCS)�� �����ڵ� ���ڿ� (�� ���ڴ� 2����Ʈ)�� ��ȯ
/// </summary>
/// <param name="srcStr">��Ƽ����Ʈ ���ڿ� (MBCS)</param>
/// <returns>��ȯ �� �����ڵ� ���ڿ� (�� ���ڴ� 2����Ʈ)</returns>
wchar_t* utils::CharToWChar(const char* srcStr)
{
	if (srcStr == NULL)
		throw std::invalid_argument(std::string(__func__) + std::string(" : Invalid Args"));

	wchar_t* retVal = new wchar_t[strlen(srcStr) + 1];
	size_t numOfCharConverted = 0; //��ȯ �� ���� ��

	// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mbstowcs-s-mbstowcs-s-l?view=msvc-170
	mbstowcs_s(&numOfCharConverted, retVal, strlen(srcStr) + 1,
		srcStr, strlen(srcStr));

	return retVal;
}

/// <summary>
/// �����ڵ� ���ڿ� (�� ���ڴ� 2����Ʈ)�� ��Ƽ����Ʈ ���ڿ� (MBCS)�� ��ȯ
/// </summary>
/// <param name="srcStr">�����ڵ� ���ڿ� (�� ���ڴ� 2����Ʈ)</param>
/// <returns>��ȯ �� ��Ƽ����Ʈ ���ڿ� (MBCS)</returns>
char* utils::WCharToChar(const wchar_t* srcStr)
{
	if (srcStr == NULL)
		throw std::invalid_argument(std::string(__func__) + std::string(" : Invalid Args"));

	char* retVal = new char[wcslen(srcStr) + 1];
	size_t numOfWCharConverted = 0; //��ȯ �� ���� ��

	// https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/mbstowcs-s-mbstowcs-s-l?view=msvc-170
	wcstombs_s(&numOfWCharConverted, retVal, wcslen(srcStr) + 1,
		srcStr, wcslen(srcStr));

	return retVal;
}