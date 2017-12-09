#pragma once

class CMemory
{
public:
	CMemory(HANDLE hProcess) :
		m_hProcess(hProcess)
	{
	}
	~CMemory() = default;

	template <typename T>
	T ReadMemory(DWORD dwTargetAddress)
	{
		T ret;
		ReadProcessMemory(m_hProcess, reinterpret_cast<LPCVOID>(dwTargetAddress), &ret, sizeof(T), nullptr);
		return ret;
	}

	template <typename T>
	bool WriteMemory(DWORD dwTargetAddress, T Value)
	{
		SIZE_T dwWrittenByteCount;
		auto bWriteRet = WriteProcessMemory(m_hProcess, reinterpret_cast<LPVOID>(dwTargetAddress), &Value, sizeof(T), &dwWrittenByteCount);
		return bWriteRet;
	}

private:
	HANDLE m_hProcess;
};

