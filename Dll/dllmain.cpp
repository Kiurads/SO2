#include <windows.h>

BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved
)

{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		if (lpReserved == NULL)

			break;
		/*case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;*/
	case DLL_PROCESS_DETACH:
		if (lpReserved == NULL)

			break;
	}
	return TRUE;
}