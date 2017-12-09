#include "main.h"
#include "BasicMemory.h"
#include "ProcessHelper.h"

std::shared_ptr <CMemory>	g_memoryHelper;
HANDLE						g_hProcess		= INVALID_HANDLE_VALUE;
std::uint32_t				g_ulBaseAddress = 0UL;

bool IncreaseResource(std::uint32_t ulPointer, std::uint32_t ulOffsetList[], std::uint32_t dwLastOffset = 0x0, std::size_t uiOffsetListSize = 4)
{
	auto pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(g_ulBaseAddress + ulPointer);
	for (std::size_t i = 0; i < uiOffsetListSize; ++i)
		pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(pTarget + ulOffsetList[i]);

	auto oldValue = g_memoryHelper->ReadMemory<float>(pTarget + dwLastOffset);
	if (oldValue < 0.f || oldValue > 1000000.f)
		return false;

	auto newValueRet = g_memoryHelper->WriteMemory<float>(pTarget + dwLastOffset, oldValue + RESOURCE_INCREASE_AMOUNT);
	return newValueRet;
}

void IncreaseWood()
{
	std::uint32_t offsetList[] = { 0x4, 0x184, 0x8, 0x3C };
	IncreaseResource(0x9C22BC, offsetList, 0x4);
}
void IncreaseFood()
{
	std::uint32_t offsetList[] = { 0xC8, 0x184, 0x8, 0x3C };
	IncreaseResource(0x6D23EC, offsetList);
}
void IncreaseGold()
{
	std::uint32_t offsetList[] = { 0x4, 0x184, 0xC, 0x48 };
	IncreaseResource(0x9C22BC, offsetList, 0xC);
}
void IncreaseStone()
{
	std::uint32_t offsetList[] = { 0x4, 0x184, 0x8, 0x3C };
	IncreaseResource(0x9C22BC, offsetList, 0x8);
}

typedef VOID(__cdecl* TIncreaseFunctionType)(void);
void OnKeyPress(DWORD dwKey, TIncreaseFunctionType pIncreaseFunction)
{
	if (GetAsyncKeyState(dwKey) & 0x8000)
	{
		while (GetAsyncKeyState(dwKey) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		pIncreaseFunction();
	}
}

int main()
{
	printf("Target process: %s\n", TARGET_PROCESS_NAME);

	auto dwTargetPID = NProcessHelper::GetProcessIdFromProcessName(TARGET_PROCESS_NAME);
	if (!dwTargetPID)
	{
		printf("Target process not found! Last Error: %u\n", GetLastError());
		std::cin.get();
		return EXIT_FAILURE;
	}
	printf("Target PID: %u\n", dwTargetPID);

	g_hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwTargetPID);
	if (!g_hProcess || g_hProcess == INVALID_HANDLE_VALUE)
	{
		printf("OpenProcess fail! Error: %u\n", GetLastError());
		std::cin.get();
		return EXIT_FAILURE;
	}
	printf("Target Handle: %p\n", g_hProcess);

	g_memoryHelper = std::make_shared<CMemory>(g_hProcess);
	if (!g_memoryHelper || !g_memoryHelper.get())
	{
		printf("g_memoryHelper can not created! Error: %u\n", GetLastError());
		std::cin.get();
		return EXIT_FAILURE;
	}
	printf("g_memoryHelper %p\n", g_memoryHelper.get());

	g_ulBaseAddress = NProcessHelper::GetBaseAddress(dwTargetPID);
	if (!g_ulBaseAddress)
	{
		printf("Target base not found! Last Error: %u\n", GetLastError());
		std::cin.get();
		return EXIT_FAILURE;
	}
	printf("Target base at: 0x%X\n", g_ulBaseAddress);

	while (true)
	{
		OnKeyPress(VK_F5, IncreaseWood);
		OnKeyPress(VK_F6, IncreaseFood);
		OnKeyPress(VK_F7, IncreaseGold);
		OnKeyPress(VK_F8, IncreaseStone);

		Sleep(1);
	}

	CloseHandle(g_hProcess);
	return EXIT_SUCCESS;
}

