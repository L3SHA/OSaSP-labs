#ifndef UNICODE
#define UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <new>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
using namespace std;

struct StateInfo {
	int clientWidth, clientHeight;
	int colCount, rowCount;
	vector<vector<string>> content;
};

const int HEIGHT_STEP = 5;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const wchar_t CLASS_NAME[] = L"Sample Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create state variable

	StateInfo* pState = new (std::nothrow) StateInfo;

	if (pState == NULL)
	{
		return 0;
	}

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		L"Learn to Program Windows",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

		NULL,       // Parent window
		NULL,       // Menu
		hInstance,  // Instance handle
		pState        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 1;
	}

	ShowWindow(hwnd, nCmdShow);

	// Run the message loop.

	MSG msg = { };
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

inline StateInfo* GetAppState(HWND hwnd)
{
	LONG_PTR ptr = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	StateInfo* pState = reinterpret_cast<StateInfo*>(ptr);
	return pState;
}


inline void split(const string& str, vector<string>& vector)
{
	istringstream iss(str);
	copy(istream_iterator<string>(iss), istream_iterator<string>(), back_inserter(vector));
}

inline void GetContentFromFile(StateInfo* pState)
{
	string line;
	ifstream file("content.txt");
	if (file.is_open())
	{
		while (getline(file, line))
		{
			vector<string> vector;
			split(line, vector);
			pState->content.push_back(vector);
		}
		file.close();
	}
	pState->rowCount = pState->content.size();
	pState->colCount = pState->content[0].size();
}

inline BOOL DrawLine(HDC hdc, int x1, int y1, int x2, int y2)
{
	POINT pt;
	MoveToEx(hdc, x1, y1, &pt);
	return LineTo(hdc, x2, y2);
}

inline void DrawTable(HDC hdc, StateInfo* pState)
{
	int colWidth = pState->clientWidth / pState->colCount;
	int rowHeight = pState->clientHeight / pState->rowCount;
	for (int i = 1; i < pState->colCount; i++)
	{
		int x = colWidth * i;
		DrawLine(hdc, x, 0, x, pState->clientHeight);
	}
	for (int i = 1; i < pState->rowCount; i++)
	{
		int y = rowHeight * i;
		DrawLine(hdc, 0, y, pState->clientWidth, y);
	}
}

inline void DrawCell(HDC hdc, TCHAR text[], int colWidth, int rowHeight, int i, int j, HFONT hfont)
{
	int left = colWidth * (i - 1);
	int top = rowHeight * (j - 1);
	int right = colWidth * i;
	int bottom = rowHeight * j;
	RECT* pRect = new RECT{ left, top, right, bottom };
	DrawText(hdc, text, -1, pRect, DT_CALCRECT);
	int fontHeight = rowHeight;
	HFONT oldFont = hfont;
	while (pRect->right > right) {
		*pRect = { left, top, right, bottom };
		fontHeight -= HEIGHT_STEP;
		//fontHeight = fontHeight / 4;
		hfont = CreateFont(fontHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
			ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_SWISS, L"Jetbrains Mono");
		HFONT prevFont = (HFONT)SelectObject(hdc, hfont);
		 
		DrawText(hdc, text, -1, pRect, DT_CALCRECT);
		DeleteObject(prevFont);

	}
	*pRect = { left, top, right, bottom };
	DrawText(hdc, text, -1, pRect, DT_CENTER | DT_SINGLELINE | DT_VCENTER);
	delete pRect;
	HFONT oldObj = (HFONT)SelectObject(hdc, oldFont);
	DeleteObject(oldObj);
}

inline TCHAR* StringToTChar(string str)
{
	int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	TCHAR* text = new TCHAR[length];
	MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, text, length);
	return text;
}

inline void DrawCells(HDC hdc, StateInfo* pState)
{
	int colWidth = pState->clientWidth / pState->colCount;
	int rowHeight = pState->clientHeight / pState->rowCount;
	HFONT hFont = NULL;
	for (int i = 1; i <= pState->colCount; i++)
	{
		for (int j = 1; j <= pState->rowCount; j++)
		{
			hFont = CreateFont(rowHeight, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
				ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
				DEFAULT_PITCH | FF_SWISS, L"Jetbrains Mono");
			HFONT prevFont = (HFONT)SelectObject(hdc, hFont);
			TCHAR* text = StringToTChar(pState->content[j - 1][i - 1]);
			DrawCell(hdc, text, colWidth, rowHeight, i, j, hFont);
			delete text;
			DeleteObject(prevFont);
		}
	}
	DeleteObject(hFont);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	StateInfo* pState;
	if (uMsg == WM_CREATE)
	{
		CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
		pState = reinterpret_cast<StateInfo*>(pCreate->lpCreateParams);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pState);

		RECT winRect;

		if (GetClientRect(hwnd, &winRect)) {
			pState->clientWidth = winRect.right - winRect.left;
			pState->clientHeight = winRect.bottom - winRect.top;
		}

		GetContentFromFile(pState);
	}
	else
	{
		pState = GetAppState(hwnd);
	}

	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
		DrawCells(hdc, pState);
		DrawTable(hdc, pState);

		EndPaint(hwnd, &ps);
	}
	return 0;

	case WM_GETMINMAXINFO:
	{
		MINMAXINFO FAR* lpMinMaxInfo;
		lpMinMaxInfo = (MINMAXINFO FAR*) lParam;
		lpMinMaxInfo->ptMinTrackSize.x = 400;
		lpMinMaxInfo->ptMinTrackSize.y = 400;
	}
		return 0;

	case WM_SIZE:
	{
		pState->clientWidth = LOWORD(lParam);
		pState->clientHeight = HIWORD(lParam);
		InvalidateRect(hwnd, NULL, false);
	}
		return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}