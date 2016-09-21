// Pexor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Pexor.h"
#include "Commctrl.h"
#include "Commdlg.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
HWND hTreeView;
HTREEITEM hRootItem;
//WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
//WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
VOID                AppendLog(HWND hDlg, int nlDDlgItem, LPCWSTR lpString);
BOOL                InitInstance(HINSTANCE, int);
INT_PTR CALLBACK    DlgProc(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

	// Store instance handle in our global variable
	hInst = hInstance;

	DialogBox(NULL, MAKEINTRESOURCE(IDD_PEXOR_DIALOG), NULL, DlgProc);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PEXOR));

    // Main message loop:
	MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


//
//  FUNCTION: AppendLog(HWND hDlg, int nlDDlgItem, LPCWSTR lpString)
//
//  PURPOSE:  Write to log view
//
//
VOID AppendLog(HWND hDlg, int nlDDlgItem, LPCWSTR lpString)
{
	HWND hWnd = GetDlgItem(hDlg, nlDDlgItem);
	int iLength = GetWindowTextLength(hWnd);
	SendMessage(hWnd, EM_SETSEL, iLength, iLength);
	SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)lpString);
	SendMessage(hWnd, WM_VSCROLL, SB_BOTTOM, (LPARAM)NULL);
}

VOID ParsePexFile(HWND hDlg, LPCWSTR lpFile) {
	AppendLog(hDlg, IDC_LOG, L"[11:24] Parsing PEX structure ...\n");

	//TODO: parse & check

	AppendLog(hDlg, IDC_LOG, L"[11:24] PEX is verified\n");

	TVINSERTSTRUCT tvis;

	// Add new node
	memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
	tvis.item.mask = TVIF_TEXT;
	tvis.item.pszText = L"code";
	tvis.hInsertAfter = TVI_LAST;
	tvis.hParent = hRootItem;

	HTREEITEM hTreeSubItem1 = (HTREEITEM) SendMessage(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvis);

	// Add new node
	memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
	tvis.item.mask = TVIF_TEXT | TVIF_STATE; // added extra flag
	tvis.item.pszText = L"text";
	tvis.hInsertAfter = TVI_LAST;
	tvis.hParent = hRootItem;

	HTREEITEM hTreeSubItem2 = (HTREEITEM)SendMessage(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvis);

	// Add new node
	memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
	tvis.item.mask = TVIF_TEXT | TVIF_STATE; // added extra flag
	tvis.item.pszText = L"estub";
	tvis.hInsertAfter = TVI_LAST;
	tvis.hParent = hRootItem;

	HTREEITEM hTreeSubItem3 = (HTREEITEM)SendMessage(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvis);

	TreeView_Expand(hTreeView, hRootItem, TVE_EXPAND);

	// Enable controls
	EnableWindow(GetDlgItem(hDlg, IDOK), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_KEY), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_COMBO2), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_TREE1), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_INFO), TRUE);
	EnableWindow(GetDlgItem(hDlg, IDC_EDIT_RES), TRUE);
}

INT_PTR CALLBACK DefaultDlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDM_EXIT || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, 0);
			return (INT_PTR)TRUE;
		}
	}
	return (INT_PTR)FALSE;
}

// Message handler for the main dialog
INT_PTR CALLBACK DlgProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
	case WM_INITDIALOG:
		{
			// Set the dialog icon
			HICON hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_SMALL));
			SendMessage(hDlg, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);

			// Write next step to log
			AppendLog(hDlg, IDC_LOG, L"[11:24] Pexor started\n");
			AppendLog(hDlg, IDC_LOG, L"[11:24] Load a PEX file to continue ...\n");

			// Initialize tree
			hTreeView = GetDlgItem(hDlg, IDC_TREE1);

			DWORD dwStyle = GetWindowLong(hTreeView, GWL_STYLE);
			dwStyle |= TVS_HASBUTTONS | TVS_HASLINES;
			SetWindowLongPtr(hTreeView, GWL_STYLE, dwStyle);

			// Add root item
			TVINSERTSTRUCT tvis;
			memset(&tvis, '\0', sizeof(TVINSERTSTRUCT));
			tvis.item.mask = TVIF_TEXT;
			tvis.item.pszText = L"PEX Image";
			tvis.hInsertAfter = TVI_LAST;
			tvis.hParent = TVI_ROOT;

			hRootItem = (HTREEITEM)SendMessage(hTreeView, TVM_INSERTITEM, 0, (LPARAM)&tvis);

			// Bind tree root item to tree view
			TVITEM tvi;
			tvi.hItem = hRootItem;
			tvi.mask = TVIF_STATE;
			tvi.stateMask = TVIS_STATEIMAGEMASK;
			tvi.state = 0;
			TreeView_SetItem(hTreeView, &tvi);

			// Fill operation listbox
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)L"XOR (Recommended)");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)L"XTEA");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)L"AES");
			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_ADDSTRING, 0, (LPARAM)L"NONE");

			SendMessage(GetDlgItem(hDlg, IDC_COMBO2), CB_SETCURSEL, 2, 0);
		}
		return (INT_PTR)TRUE;

    case WM_COMMAND:
		if (LOWORD(wParam) == IDM_EXIT || LOWORD(wParam) == IDCANCEL)
		{
			DestroyWindow(hDlg);
			PostQuitMessage(0);
			return (INT_PTR)TRUE;
		}
		if (LOWORD(wParam) == IDM_ABOUT)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hDlg, DefaultDlgProc);
			break;
		}
		if (LOWORD(wParam) == IDOK)
		{
			INT result = MessageBox(hDlg, L"An executable file will be generated that can damage systems, or cause harm. If you continue you acknowledge the consequences.\n\nDo you want to proceed?", L"Confirmation", MB_YESNO | MB_ICONEXCLAMATION);
			if (result != IDYES)
			{
				break;
			}
			
			OPENFILENAME ofn;       // common dialog box structure
			wchar_t szFile[260];    // buffer for file name

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Executable (.exe)\0*.EXE\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST;

			// Display the Save dialog box
			if (GetSaveFileName(&ofn) == TRUE)
			{
				AppendLog(hDlg, IDC_LOG, L"[11:24] Writing executable to ");
				AppendLog(hDlg, IDC_LOG, szFile);
				AppendLog(hDlg, IDC_LOG, L"\n");
			}

			break;
		}
		if (LOWORD(wParam) == ID_FILE_OPENCONFIG)
		{
			OPENFILENAME ofn;       // common dialog box structure
			wchar_t szFile[260];    // buffer for file name

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"Config (*.ini)\0*.INI\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box
			if (GetOpenFileName(&ofn) == TRUE)
			{
				AppendLog(hDlg, IDC_LOG, L"[11:24] Loaded settings from config\n");
			}
			break;
		}
		if (LOWORD(wParam) == ID_FILE_OPENPEX)
		{
			OPENFILENAME ofn;       // common dialog box structure
			wchar_t szFile[260];    // buffer for file name

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = hDlg;
			ofn.lpstrFile = szFile;
			ofn.lpstrFile[0] = '\0';
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = L"PEX Image (*.pex)\0*.PEX\0Executable (*.exe)\0*.EXE\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			// Display the Open dialog box
			if (GetOpenFileName(&ofn) == TRUE)
			{
				AppendLog(hDlg, IDC_LOG, L"[11:24] Loaded PEX image from ");
				AppendLog(hDlg, IDC_LOG, ofn.lpstrFile);
				AppendLog(hDlg, IDC_LOG, L"\n");

				/*HWND ProgressBar = GetDlgItem(hDlg, IDC_PROGRESS);
				SendMessage(ProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0, 220));
				SendMessage(ProgressBar, PBM_SETSTEP, 10, 0);
				SendMessage(ProgressBar, PBM_STEPIT, 0, 0);*/

				ParsePexFile(hDlg, ofn.lpstrFile);
			}
			break;
		}
		if (LOWORD(wParam) == IDC_EDIT_INFO)
		{
			DialogBox(hInst, MAKEINTRESOURCE(IDD_INFO), hDlg, DefaultDlgProc);
			break;
		}
    }
    return (INT_PTR)FALSE;
}
