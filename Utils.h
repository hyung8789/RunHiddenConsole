#ifndef _UTILS_H_
#define _UTILS_H_

#include <windows.h>
#include <memory>
#include <stdlib.h>
#include <psapi.h>

#define LENGTH(array) ((sizeof(array)) / (sizeof(array[0])))

namespace utils
{
	const TCHAR* GetSelfProcessNameWithExt();
	bool AttachToConsole();
	bool AttachToNewConsole();

	wchar_t* CharToWChar(const char*);
	char* WCharToChar(const wchar_t*);
}
#endif