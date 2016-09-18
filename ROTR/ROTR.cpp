// ROTR.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "ROTR.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
INT_PTR CALLBACK    ClbProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	DialogBox(NULL, MAKEINTRESOURCE(IDD_MAIN), NULL, ClbProc);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
	    DispatchMessage(&msg);
    }

    return (int) msg.wParam;
}


// ROTR string encode
static void Rot13_encode(wchar_t str[])
{
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 'a' && str[i] <= 'm') {
			str[i] += 13;
		}
		else if (str[i] > 'm' && str[i] <= 'z') {
			str[i] -= 13;
		}
		else if (str[i] >= 'A' && str[i] <= 'M') {
			str[i] += 13;
		}
		else if (str[i] > 'M' && str[i] <= 'Z') {
			str[i] -= 13;
		}
		else if (str[i] == '\\') {
			str[i] = '{';
		}
		else if (str[i] == ')') {
			str[i] = '}';
		}
		else if (str[i] == ')') {
			str[i] = '[';
		}
		else if (str[i] == '/') {
			str[i] = '?';
		}
		else if (str[i] == ' ') {
			str[i] = '&';
		}
		else if (str[i] == '_') {
			str[i] = '*';
		}
		else if (str[i] == '.') {
			str[i] = '^';
		}
	}
}


// ROTR string decode
static void Rot13_decode(wchar_t str[]) {
	for (int i = 0; str[i] != '\0'; i++) {
		if (str[i] >= 'a' && str[i] <= 'm') {
			str[i] += 13;
		}
		else if (str[i] > 'm' && str[i] <= 'z') {
			str[i] -= 13;
		}
		else if (str[i] >= 'A' && str[i] <= 'M') {
			str[i] += 13;
		}
		else if (str[i] > 'M' && str[i] <= 'Z') {
			str[i] -= 13;
		}
		else if (str[i] == '{') {
			str[i] = '\\';
		}
		else if (str[i] == '}') {
			str[i] = '(';
		}
		else if (str[i] == '[') {
			str[i] = ')';
		}
		else if (str[i] == '?') {
			str[i] = '/';
		}
		else if (str[i] == '&') {
			str[i] = ' ';
		}
		else if (str[i] == '*') {
			str[i] = '_';
		}
		else if (str[i] == '^') {
			str[i] = '.';
		}
	}
}


// Message handler for about box.
INT_PTR CALLBACK ClbProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
		// Get the owner window and dialog box rectangles. 
		HWND hwndOwner;
		RECT rc, rcDlg, rcOwner;

		if ((hwndOwner = GetParent(hDlg)) == NULL)
		{
			hwndOwner = GetDesktopWindow();
		}

		GetWindowRect(hwndOwner, &rcOwner);
		GetWindowRect(hDlg, &rcDlg);
		CopyRect(&rc, &rcOwner);

		// Offset the owner and dialog box rectangles so that right and bottom 
		// values represent the width and height, and then offset the owner again 
		// to discard space taken up by the dialog box. 

		OffsetRect(&rcDlg, -rcDlg.left, -rcDlg.top);
		OffsetRect(&rc, -rc.left, -rc.top);
		OffsetRect(&rc, -rcDlg.right, -rcDlg.bottom);

		// The new position is the sum of half the remaining space and the owner's 
		// original position. 

		SetWindowPos(hDlg,
			HWND_TOP,
			rcOwner.left + (rc.right / 2),
			rcOwner.top + (rc.bottom / 2),
			0, 0,          // Ignores size arguments. 
			SWP_NOSIZE);

		CheckDlgButton(hDlg, IDC_ENCODE, 1 );

		return (INT_PTR)TRUE;

    case WM_COMMAND:
		if (LOWORD(wParam) == IDOK)
		{
			TCHAR input[1024];

			GetDlgItemText(hDlg, IDC_INPUT, input, 1024);

			IsDlgButtonChecked(hDlg, IDC_ENCODE) ? Rot13_encode(input) : Rot13_decode(input);

			SetDlgItemText(hDlg, IDC_OUTPUT, input);
		}

		if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
			PostQuitMessage(0);
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
