#include "main.h"
#include "BasicMemory.h"
#include "ProcessHelper.h"

enum EResources
{
	EResources_Food,
	EResources_Wood,
	EResources_Stone,
	EResources_Gold
};

std::shared_ptr <CMemory>	g_memoryHelper;
HANDLE						g_hProcess			= INVALID_HANDLE_VALUE;
std::uint32_t				g_ulBaseAddress		= 0UL;

const std::uintptr_t		g_pResourceBase		= 0x9C22BC;
const std::uint32_t			g_offsetList[]		= { 0x4, 0x184, 0x8, 0x3C };

static bool IncreaseResource(std::uint32_t ulOffset)
{
	auto pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(g_ulBaseAddress + g_pResourceBase);
	for (std::size_t i = 0; i < _countof(g_offsetList); ++i)
		pTarget = g_memoryHelper->ReadMemory<std::uintptr_t>(pTarget + g_offsetList[i]);

	auto oldValue = g_memoryHelper->ReadMemory<float>(pTarget + ulOffset);
	if (oldValue < 0.f || oldValue > 1000000.f)
		return false;

	auto newValueRet = g_memoryHelper->WriteMemory<float>(pTarget + ulOffset, oldValue + RESOURCE_INCREASE_AMOUNT);
	return newValueRet;
}


typedef bool(__cdecl* TIncreaseFunctionType)(std::uint32_t ulOffset);
void OnKeyPress(DWORD dwKey, TIncreaseFunctionType pIncreaseFunction, std::uint32_t ulParam)
{
	if (GetAsyncKeyState(dwKey) & 0x8000)
	{
		while (GetAsyncKeyState(dwKey) & 0x8000)
			Sleep(1);

		Beep(750, 250);
		pIncreaseFunction(ulParam);
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
		OnKeyPress(VK_F5, IncreaseResource, EResources_Food  * sizeof(float) /* 0x0 */);
		OnKeyPress(VK_F6, IncreaseResource, EResources_Wood  * sizeof(float) /* 0x4 */);
		OnKeyPress(VK_F7, IncreaseResource, EResources_Stone * sizeof(float) /* 0x8 */);
		OnKeyPress(VK_F8, IncreaseResource, EResources_Gold  * sizeof(float) /* 0xC */);

		Sleep(1);
	}

	CloseHandle(g_hProcess);
	return EXIT_SUCCESS;
}

