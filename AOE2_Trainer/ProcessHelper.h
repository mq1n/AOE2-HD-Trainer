#pragma once
#include "main.h"

namespace NProcessHelper
{
	DWORD GetProcessIdFromProcessName(const std::string & szProcessName)
	{
		auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (!hSnapshot || hSnapshot == INVALID_HANDLE_VALUE)
			return 0;

		PROCESSENTRY32 pt;
		pt.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnapshot, &pt)) {
			do {
				if (strstr(pt.szExeFile, szProcessName.c_str())) {
					CloseHandle(hSnapshot);
					return pt.th32ProcessID;
				}
			} while (Process32Next(hSnapshot, &pt));
		}

		CloseHandle(hSnapshot);
		return 0;
	}

	DWORD GetBaseAddress(DWORD dwTargetPID)
	{
		auto hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwTargetPID);
		if (!hSnapshot || hSnapshot == INVALID_HANDLE_VALUE)
			return 0;

		MODULEENTRY32 me;
		memset(&me, 0, sizeof(me));
		me.dwSize = sizeof(me);

		DWORD dwBaseAddress = 0;
		if (Module32First(hSnapshot, &me))
			dwBaseAddress = reinterpret_cast<DWORD>(me.modBaseAddr);

		CloseHandle(hSnapshot);
		return dwBaseAddress;
	}
}

