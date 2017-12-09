#include "main.h"
#include "BasicMemory.h"
#include "ProcessHelper.h"

std::shared_ptr <CMemory>	g_memoryHelper;
HANDLE						g_hProcess		= INVALID_HANDLE_VALUE;
std::uint32_t				g_ulBaseAddress = 0UL;

void IncreaseWood()
{
	auto pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(g_ulBaseAddress + 0x9C22BC); // https://vgy.me/RNktMU.png
	DWORD offsetList[] = { 0x4, 0x184, 0x8, 0x3C };
	for (std::size_t i = 0; i < _countof(offsetList); ++i)
		pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(pTarget + offsetList[i]);

	auto oldValue = g_memoryHelper->ReadMemory<float>(pTarget + 0x4);
	// printf("%f\n", oldValue);
	auto newValueRet = g_memoryHelper->WriteMemory<float>(pTarget + 0x4, oldValue + RESOURCE_INCREASE_AMOUNT);
}
void IncreaseFood()
{
	auto pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(g_ulBaseAddress + 0x6D23EC); // https://vgy.me/evIGLJ.png
	DWORD offsetList[] = { 0xC8, 0x184, 0x8, 0x3C };
	for (std::size_t i = 0; i < _countof(offsetList); ++i)
		pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(pTarget + offsetList[i]);

	auto oldValue = g_memoryHelper->ReadMemory<float>(pTarget);
	// printf("%f\n", oldValue);
	auto newValueRet = g_memoryHelper->WriteMemory<float>(pTarget, oldValue + RESOURCE_INCREASE_AMOUNT);
}
void IncreaseGold()
{
	auto pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(g_ulBaseAddress + 0x9C22BC); // https://vgy.me/UtQS8W.png
	DWORD offsetList[] = { 0x4, 0x184, 0xC, 0x48 };
	for (std::size_t i = 0; i < _countof(offsetList); ++i)
		pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(pTarget + offsetList[i]);

	auto oldValue = g_memoryHelper->ReadMemory<float>(pTarget + 0xC);
	// printf("%f\n", oldValue);
	auto newValueRet = g_memoryHelper->WriteMemory<float>(pTarget + 0xC, oldValue + RESOURCE_INCREASE_AMOUNT);
}
void IncreaseStone()
{
	auto pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(g_ulBaseAddress + 0x9C22BC); // https://vgy.me/5icSGu.png
	DWORD offsetList[] = { 0x4, 0x184, 0x8, 0x3C };
	for (std::size_t i = 0; i < _countof(offsetList); ++i)
		pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(pTarget + offsetList[i]);

	auto oldValue = g_memoryHelper->ReadMemory<float>(pTarget + 0x8);
	// printf("%f\n", oldValue);
	auto newValueRet = g_memoryHelper->WriteMemory<float>(pTarget + 0x8, oldValue + RESOURCE_INCREASE_AMOUNT);
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

