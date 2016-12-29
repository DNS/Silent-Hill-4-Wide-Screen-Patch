/*
	Silent Hill 4 - Wide Screen Patch
	http://github.com/dns/Silent-Hill-4-Wide-Screen-Patch

	Copyright (c) 2015 by Daniel Sirait

*/

#pragma comment(linker,"\"/manifestdependency:type='win32' \
	name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
	processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#pragma comment(lib, "Comctl32.lib")

//#define _CRT_SECURE_NO_DEPRECATE
//#define _CRT_SECURE_NO_DEPRECATE_GLOBALS
//#define _CRT_OBSOLETE_NO_DEPRECATE
//#define _CRT_NONSTDC_NO_DEPRECATE
//#define _CRT_SECURE_NO_WARNINGS

#undef UNICODE

#include <windows.h>
#include <commctrl.h>

NONCLIENTMETRICSA ncm;

DWORD Width, Height;
FLOAT HFOV, VFOV;
int res_mode;
BOOL delay_write = FALSE;

CHAR res_list[11][20] = {"640x480", "800x600", "1024x768", "1024x576", "1152x648", "1280x720", "1366x768", "1600x900", "1920x1080", "2560x1440", "3840x2160"};

HANDLE thread1, thread_delay;
DWORD thread1_id, thread_delay_id;

HWND hDlgCurrent = NULL;
HFONT hfont1;
HWND hTrack_hfov, hTrack_vfov;
HWND hLeftLabel1, hRightLabel1;
HWND hLeftLabel2, hRightLabel2;
HWND hResLabel;
HWND hCombo;
HWND groupbox1, groupbox2;
HWND hLink1;
HWND button43, button169;

void SetWideScreen ();
void ChangeDisplay (int);
void ReadConfig ();
void WriteConfig ();
void DelayWriteConfig ();
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DestroyChildWindow (HWND, LPARAM);


void SetWideScreen () {
	DWORD base_address;
	SIZE_T bytes_read;
	INT32 buf_int, buf_int_read;
	BOOL result;
	DWORD OldProtect;
	FLOAT buf_float_read;
	HWND hwnd_sh4;
	DWORD exitCode;
	BOOL run_once = FALSE;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	
	
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	result = CreateProcessA("SILENT HILL 4.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (!result) {
		MessageBoxA(NULL, "Can't create process: SILENT HILL 4.exe", "Error", MB_ICONERROR);
		ExitProcess(GetLastError());
	}

	Sleep(2000);	// assume SH4 window have been created after 2 sec (BUG: set to higher value on slower PC)

	hwnd_sh4 = FindWindowA(NULL, "SILENT HILL 4:The Room");
	if (!hwnd_sh4)
		MessageBoxA(NULL, "can't find window \"SILENT HILL 4:The Room\"", "Error", MB_ICONERROR);

	//GetWindowThreadProcessId(hwnd_find, &process_id);


	//hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_QUERY_INFORMATION, FALSE, process_id);

	//base_address = GetProcessBaseAddress(process_id) + memory_offset;		// ASLR: (only for Windows 7 new EXE/DLL compiled with ASLR)

	/*base_address = 0x005D524C;
	VirtualProtectEx(pi.hProcess, base_address, 4, PAGE_EXECUTE_READWRITE, &OldProtect);

	base_address = 0x005D5250;
	VirtualProtectEx(pi.hProcess, base_address, 4, PAGE_EXECUTE_READWRITE, &OldProtect);

	base_address = 0x012E3EF4;
	VirtualProtectEx(pi.hProcess, base_address, 4, PAGE_EXECUTE_READWRITE, &OldProtect);

	base_address = 0x012E3EF8;
	VirtualProtectEx(pi.hProcess, base_address, 4, PAGE_EXECUTE_READWRITE, &OldProtect);
*/
	while (1) {
		GetExitCodeProcess(pi.hProcess, &exitCode);

		if (exitCode != STILL_ACTIVE) break;

		base_address = 0x005D524C;
		ReadProcessMemory(pi.hProcess, base_address, &buf_int_read, 4, &bytes_read);
		if (buf_int_read != Width)
			WriteProcessMemory(pi.hProcess, base_address, &Width, 4, &bytes_read);

		base_address = 0x005D5250;
		ReadProcessMemory(pi.hProcess, base_address, &buf_int_read, 4, &bytes_read);
		if (buf_int_read != Height)
			WriteProcessMemory(pi.hProcess, base_address, &Height, 4, &bytes_read);

		base_address = 0x012E3EF4;
		ReadProcessMemory(pi.hProcess, base_address, &buf_float_read, 4, &bytes_read);
		if (buf_float_read != HFOV)
			WriteProcessMemory(pi.hProcess, base_address, &HFOV, 4, &bytes_read);

		base_address = 0x012E3EF8;
		ReadProcessMemory(pi.hProcess, base_address, &buf_float_read, 4, &bytes_read);
		if (buf_float_read != VFOV)
			WriteProcessMemory(pi.hProcess, base_address, &VFOV, 4, &bytes_read);

		if (run_once == FALSE) {
			// refresh window to apply changes (sh4 bug)
			ShowWindowAsync(hwnd_sh4, SW_MINIMIZE);
			ShowWindowAsync(hwnd_sh4, SW_RESTORE);
			run_once = TRUE;
		}

		Sleep(1000);
	}

	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	
	
	ExitProcess(0);
}


void ChangeDisplay (int index) {
	res_mode = index;
	switch (res_mode) {
		case 0:
			Width = 640;
			Height = 480;
			break;
		case 1:
			Width = 800;
			Height = 600;
			break;
		case 2:
			Width = 1024;
			Height = 768;
			break;
		case 3:
			Width = 1024;
			Height = 576;
			break;
		case 4:
			Width = 1152;
			Height = 648;
			break;
		case 5:
			Width = 1280;
			Height = 720;
			break;
		case 6:
			Width = 1366;
			Height = 768;
			break;
		case 7:
			Width = 1600;
			Height = 900;
			break;
		case 8:
			Width = 1920;
			Height = 1080;
			break;
		case 9:
			Width = 2560;
			Height = 1440;
			break;
		case 10:
			Width = 3840;
			Height = 2160;
			break;
	}

}

void ReadConfig () {
	CHAR sHFOV[34], sVFOV[34];
	
	// 2: "1024x768"
	res_mode = GetPrivateProfileIntA("WidescreenConfig", "res_mode", 2, ".\\WidescreenConfig.ini");
	ChangeDisplay(res_mode);

	// default HVOF & VFOV: 1.0 & 0.78
	GetPrivateProfileStringA("WidescreenConfig", "HFOV", "0.75", sHFOV, 7, ".\\WidescreenConfig.ini");
	GetPrivateProfileStringA("WidescreenConfig", "VFOV", "0.58", sVFOV, 7, ".\\WidescreenConfig.ini");

	HFOV = (float) atof(sHFOV);
	VFOV = (float) atof(sVFOV);

	WriteConfig();
}


void WriteConfig () {
	CHAR sBuf[34];

	sprintf(sBuf, "%d", res_mode);
	WritePrivateProfileStringA("WidescreenConfig", "res_mode", sBuf, ".\\WidescreenConfig.ini");

	sprintf(sBuf, "%.2f", HFOV);
	WritePrivateProfileStringA("WidescreenConfig", "HFOV", sBuf, ".\\WidescreenConfig.ini");

	sprintf(sBuf, "%.2f", VFOV);
	WritePrivateProfileStringA("WidescreenConfig", "VFOV", sBuf, ".\\WidescreenConfig.ini");
}

void DelayWriteConfig () {
	delay_write = TRUE;
	Sleep(3000);
	WriteConfig();
	delay_write = FALSE;
}

void UpdateControl () {
	char sBuf[50];
	SendMessageA(hCombo, CB_SETCURSEL, res_mode, 0);
	sprintf(sBuf, "HFOV: %.2f", HFOV);
	SendMessageA(groupbox1, WM_SETTEXT, 0, (LPARAM) sBuf);
	sprintf(sBuf, "VFOV: %.2f", VFOV);
	SendMessageA(groupbox2, WM_SETTEXT, 0, (LPARAM) sBuf);
}

BOOL CALLBACK DestroyChildWindow (HWND hwnd, LPARAM lParam) {
	DestroyWindow(hwnd);
	SendMessageA(hwnd, WM_DESTROY, 0, 0);
	return TRUE;
}


LRESULT CALLBACK WndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_CREATE:
			//CenterWindow(hwnd);
			
			//hfont1 = CreateFontA(13, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Tahoma");
			
			ncm.cbSize = sizeof(NONCLIENTMETRICSA);
			SystemParametersInfoA(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICSA), &ncm, 0);
			hfont1 = CreateFontIndirectA(&ncm.lfMessageFont);

			groupbox1 = CreateWindowA("BUTTON", "HFOV: ", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 10, 300, 75, hwnd,  (HMENU) NULL, NULL, NULL);
			hLeftLabel1 = CreateWindowA("STATIC", "0.01", WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU) 1001, NULL,  NULL);
			hRightLabel1 = CreateWindowA("STATIC", "2.00", WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU) 1002, NULL,  NULL);
			hTrack_hfov = CreateWindowA("msctls_trackbar32", "Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ | WS_TABSTOP, 60, 40, 200, 30, hwnd, (HMENU) 3001, NULL, NULL);

			SendMessageA(hTrack_hfov, TBM_SETRANGE, TRUE, MAKELONG(1, 200));
			SendMessageA(hTrack_hfov, TBM_SETPAGESIZE, 0, 1);
			SendMessageA(hTrack_hfov, TBM_SETTICFREQ, 50, 0);
			SendMessageA(hTrack_hfov, TBM_SETPOS, TRUE, HFOV * 100);
			SendMessageA(hTrack_hfov, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel1);
			SendMessageA(hTrack_hfov, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel1);

			
			groupbox2 = CreateWindowA("BUTTON", "VFOV: ", WS_CHILD | WS_VISIBLE | BS_GROUPBOX, 10, 95, 300, 75, hwnd,  (HMENU) NULL, NULL, NULL);
			hLeftLabel2 = CreateWindowA("STATIC", "0.01", WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU) 1001, NULL, NULL);
			hRightLabel2 = CreateWindowA("STATIC", "2.00", WS_CHILD | WS_VISIBLE, 0, 0, 30, 30, hwnd, (HMENU) 1002, NULL, NULL);
			hTrack_vfov = CreateWindowA("msctls_trackbar32", "Trackbar Control", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_HORZ | WS_TABSTOP, 60, 130, 200, 30, hwnd, (HMENU) 3002, NULL, NULL);

			SendMessageA(hTrack_vfov, TBM_SETRANGE, TRUE, MAKELONG(1, 200));
			SendMessageA(hTrack_vfov, TBM_SETPAGESIZE, 0, 1);
			SendMessageA(hTrack_vfov, TBM_SETTICFREQ, 50, 0);
			SendMessageA(hTrack_vfov, TBM_SETPOS, TRUE, VFOV * 100);
			SendMessageA(hTrack_vfov, TBM_SETBUDDY, TRUE, (LPARAM) hLeftLabel2);
			SendMessageA(hTrack_vfov, TBM_SETBUDDY, FALSE, (LPARAM) hRightLabel2);

			SendMessageA(hLeftLabel1, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageA(hRightLabel1, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageA(hLeftLabel2, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageA(hRightLabel2, WM_SETFONT, (WPARAM) hfont1, TRUE);

			hResLabel = CreateWindowA("STATIC", "Resolution:", WS_CHILD | WS_VISIBLE, 20, 187, 100, 30, hwnd, (HMENU) 1001, NULL, NULL);
			SendMessageA(hResLabel, WM_SETFONT, (WPARAM) hfont1, TRUE);

			hCombo = CreateWindowA("COMBOBOX", NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP | CBS_HASSTRINGS | CBS_DROPDOWNLIST, 85, 183, 120, 110, hwnd, (HMENU) 5001, NULL, NULL);
			
			//SendMessageA(hCombo, CB_SETITEMHEIGHT, 0, 50);

			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[0]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[1]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[2]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[3]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[4]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[5]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[6]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[7]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[8]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[9]);
			SendMessageA(hCombo, CB_ADDSTRING, 0, (LPARAM) res_list[10]);

			SendMessageA(hCombo, CB_SETCURSEL, res_mode, 0);	// set default index for ComboBox, 1366x768 @ index = 6

			SendMessageA(hCombo, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageA(groupbox1, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageA(groupbox2, WM_SETFONT, (WPARAM) hfont1, TRUE);

			
			button43 = CreateWindowA("BUTTON", "Default 4:3", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 20, 220, 90, 25, hwnd, (HMENU) 4001, NULL, NULL);
			button169 = CreateWindowA("BUTTON", "Default 16:9 (wide)", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 120, 220, 125, 25, hwnd, (HMENU) 4002, NULL, NULL);
			SendMessageA(button43, WM_SETFONT, (WPARAM) hfont1, TRUE);
			SendMessageA(button169, WM_SETFONT, (WPARAM) hfont1, TRUE);


			hLink1 = CreateWindowA("SysLink", "<a id=\"github\" href=\"http://github.com/dns/Silent-Hill-4-Wide-Screen-Patch\">Silent Hill 4 Wide Screen Patch</a> © 2015 by <a id=\"twitter\" href=\"http://twitter.com/SiraitX\">SiraitX</a>", WS_VISIBLE | WS_CHILD | WS_TABSTOP, 30, 260, 260, 15, hwnd, NULL, NULL, NULL);

			SendMessageA(hLink1, WM_SETFONT, (WPARAM) hfont1, TRUE);


			UpdateControl();

			break;
		case WM_COMMAND:
			switch(LOWORD(wParam)) {
				case 4001:
					HFOV = 1.00f;
					VFOV = 0.78;
					res_mode = 2;
					ChangeDisplay(res_mode);
					SendMessageA(hTrack_hfov, TBM_SETPOS, TRUE, HFOV * 100);
					SendMessageA(hTrack_vfov, TBM_SETPOS, TRUE, VFOV * 100);
					UpdateControl();
					WriteConfig();
					break;
				case 4002:
					HFOV = 0.75f;
					VFOV = 0.58f;
					res_mode = 6;
					ChangeDisplay(res_mode);
					SendMessageA(hTrack_hfov, TBM_SETPOS, TRUE, HFOV * 100);
					SendMessageA(hTrack_vfov, TBM_SETPOS, TRUE, VFOV * 100);
					UpdateControl();
					WriteConfig();
					break;
				case 5001:
					// ComboBox msg
					switch (HIWORD(wParam)) {
						LRESULT index;
						case CBN_SELCHANGE:
							index = SendMessageA(hCombo, CB_GETCURSEL, 0, 0);
							//SendMessageA(hLabel, WM_SETTEXT, 0, (LPARAM) res_list[index]);
							
							ChangeDisplay(index);
							WriteConfig();
							break;
						case CBN_SELENDOK:
							//MessageBoxA(NULL, "", "", MB_OK);
							//SetFocus(hCombo);
							break;
					}




			}	// switch(LOWORD(wParam))
			break;
		case WM_HSCROLL:
		{
			LRESULT pos;
			int ctrlID = GetDlgCtrlID((HWND) lParam);

			switch(ctrlID) {
				// HFOV
				case 3001:
					pos = SendMessageA(hTrack_hfov, TBM_GETPOS, 0, 0);
					HFOV = pos * 0.01f;
					UpdateControl();
					if (delay_write == FALSE) {
						thread_delay = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DelayWriteConfig, NULL, 0, &thread_delay_id);
						CloseHandle(thread_delay);
					}
					break;
				// VFOV
				case 3002:
					pos = SendMessageA(hTrack_vfov, TBM_GETPOS, 0, 0);
					VFOV = pos * 0.01f;
					UpdateControl();
					if (delay_write == FALSE) {
						thread_delay = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) DelayWriteConfig, NULL, 0, &thread_delay_id);
						CloseHandle(thread_delay);
					}
					break;
			}
			break;
		}
		case WM_ACTIVATE:
			if (0 == wParam)		// becoming inactive
				hDlgCurrent = NULL;
			else					// becoming active
				hDlgCurrent = hwnd;
			break;
		case WM_NOTIFY:
			switch ( ((LPNMHDR)lParam)->code ) {
				case NM_CLICK:		// msg from SysLink control, Fall through to the next case.
				case NM_RETURN:		// handle mouse click & tabstop [ENTER]
					{
						PNMLINK pNMLink = (PNMLINK) lParam;
						LITEM item = pNMLink->item;
						if ((((LPNMHDR)lParam)->hwndFrom == hLink1)) {
							if (!lstrcmpiW(pNMLink->item.szID, L"github")) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							} else if (!lstrcmpiW(pNMLink->item.szID, L"twitter")) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							}

							/*if (item.iLink == 0) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							} else if (item.iLink == 1) {
								ShellExecuteW(NULL, L"open", item.szUrl, NULL, NULL, SW_SHOW);
							}*/
						}
					}
					break;
			}
			break;
		case WM_KEYDOWN:
			break;
		case WM_SIZE:
			break;
		case WM_SIZING:
			break;
		case WM_EXITSIZEMOVE:
			break;
		case WM_SETFOCUS:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_CLOSE:
			EnumChildWindows(hwnd, DestroyChildWindow, lParam);
			DestroyWindow(hwnd);
			break;
		case WM_DESTROY:
			/* Terminate Silent Hill 4 child process */
			TerminateThread(thread1, 0);
			CloseHandle(thread1);

			DeleteObject(hfont1);

			PostQuitMessage(0);
			break;
		return 0;
	}

	return DefWindowProcA(hwnd, msg, wParam, lParam);
}


INT WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow) {
	MSG msg;
	HWND hwnd;
	WNDCLASSEXA wc;
	INITCOMMONCONTROLSEX iccex;

	ReadConfig();
	WriteConfig();
	
	// alternative: can use SetTimer() & WM_TIMER but it's working on the main thread
	thread1 = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE) SetWideScreen, NULL, 0, &thread1_id);

	ZeroMemory(&wc, sizeof(wc));			// initialize with NULL (other alternative), same as memset()
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra= 0;
	wc.cbWndExtra= 0;
	wc.hInstance = hInstance;
	wc.lpszMenuName= NULL;
	wc.lpszClassName = "Window";
	wc.hbrBackground = (HBRUSH) COLOR_WINDOW;	// default window color
	//wc.hbrBackground = CreateSolidBrush(RGB(255, 0, 0));	// make something different
	wc.hCursor = LoadCursorA(NULL, IDC_ARROW);	// Note: LoadCursor() superseded by LoadImage()
	wc.hIcon = (HICON) LoadImageA(hInstance, MAKEINTRESOURCEA(1), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_DEFAULTSIZE);
	wc.hIconSm = NULL;

	iccex.dwICC = ICC_WIN95_CLASSES | ICC_STANDARD_CLASSES;
	iccex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCommonControlsEx(&iccex);

	if (!RegisterClassExA(&wc)) {
		MessageBoxA(NULL, "Window Registration Failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);
		return -1;
	}
	
	// create main window
	hwnd = CreateWindowExA(WS_EX_CONTROLPARENT, wc.lpszClassName, "Silent Hill 4 - Wide Screen Patch", WS_VISIBLE | WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 328, 330, (HWND) NULL, (HMENU) NULL, hInstance, NULL);
	
	
	ShowWindow(hwnd, nCmdShow);
	//UpdateWindow(hwnd);

	
	while (GetMessageA(&msg, NULL, 0, 0) > 0) {		/* If no error is received... */
		if (!IsDialogMessageA(hDlgCurrent, &msg)) {				/* Disable keyboard shorcut when other window active  */
			TranslateMessage(&msg);
			DispatchMessageA(&msg);		/* Send it to WndProc */
		}
		
	}
	
	return (int) msg.wParam;
}





