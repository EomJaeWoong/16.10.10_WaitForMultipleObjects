#include <Windows.h>
#include <process.h>
#include <time.h>
#include <stdio.h>

int g_Data = 0;
int g_Connect = 0;
bool b_Shutdown = false;

unsigned __stdcall AccpetThread(LPVOID accpetParam);
unsigned __stdcall DisconnectThread(LPVOID disconnectParam);
unsigned __stdcall UpdateThread(LPVOID updateParam);

void main()
{
	DWORD dwTick = 0, dwResult;

	HANDLE hObject[5];
	unsigned int uiAcceptId, uiDisconnectId, uiUpdateId;

	hObject[0] = (HANDLE)_beginthreadex(
		NULL,
		0, 
		AccpetThread, 
		0, 
		0, 
		&uiAcceptId);

	hObject[1] = (HANDLE)_beginthreadex(
		NULL,
		0,
		DisconnectThread,
		0,
		0,
		&uiDisconnectId);

	for (int iCnt = 2; iCnt < 5; iCnt++)
	{
		hObject[iCnt] = (HANDLE)_beginthreadex(
			NULL,
			0,
			UpdateThread,
			0,
			0,
			&uiUpdateId);
	}

	while (!b_Shutdown)
	{
		if (dwTick > 20)	
			b_Shutdown = true;

		dwTick++;
		Sleep(1000);

		printf("Main Thread - g_Connect : %d\n", g_Connect);
	}

	WaitForMultipleObjects(5, hObject, TRUE, INFINITE);
}

unsigned __stdcall AccpetThread(LPVOID accpetParam)
{
	srand(time(NULL));

	DWORD dwCycleTick;

	while (1){
		if (b_Shutdown)	return 0;

		dwCycleTick = rand() % 1000 <= 100 ? rand() % 1000 : 0;

		if (dwCycleTick != 0)
		{
			Sleep(dwCycleTick);
			InterlockedIncrement((LONG *)&g_Connect);
		}
	}
}

unsigned __stdcall DisconnectThread(LPVOID disconnectParam)
{
	srand(time(NULL) + 500);

	DWORD dwCycleTick;
	CRITICAL_SECTION cs;

	InitializeCriticalSection(&cs);

	while (1){
		if (b_Shutdown)	return 0;

		dwCycleTick = rand() % 1000 <= 100 ? rand() % 1000 : 0;

		EnterCriticalSection(&cs);

		if (dwCycleTick != 0)
		{
			Sleep(dwCycleTick);
			g_Connect--;

			if (g_Connect < 0)
				g_Connect = 0;
		}

		LeaveCriticalSection(&cs);
	}
	DeleteCriticalSection(&cs);
}

unsigned __stdcall UpdateThread(LPVOID updateParam)
{
	while (1)
	{
		if (b_Shutdown)	return 0;

		Sleep(10);
		InterlockedIncrement((LONG *)&g_Data);

		if (g_Data % 1000 == 0)
			printf("Update Thread - g_Data : %d\n", g_Data);
	}
}