#define UNICODE

#include <Windows.h>
#include <wchar.h>
#include <process.h>

#include "render thread.h"

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

const wchar_t CLASS_NAME[] = L"CLASS";

#define ID_FORM 10001
#define ID_BUTTON 10002



HINSTANCE hGlobalInstance;

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR nCmdLine, _In_ int nCmdShow) {
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(nCmdLine);

	hGlobalInstance = hInstance;

	WNDCLASSEX wc = {
		sizeof(WNDCLASSEX), CS_VREDRAW | CS_HREDRAW, WindowProc,
		0, 0, hInstance,
		NULL, (HCURSOR)LoadCursor(NULL, IDC_ARROW), (HBRUSH)GetStockObject(WHITE_BRUSH),
		NULL, CLASS_NAME, NULL
	};

	RegisterClassEx(&wc);

	HWND hwnd = CreateWindowEx(
		0, CLASS_NAME, L"Monte-Carlo Simuration", WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		50, 50, 960, 525,
		NULL, NULL, hInstance, NULL
	);

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0) > 0) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (int)msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	static HANDLE ht = NULL;
	static unsigned thId;
	static PassArg pa;

	static RECT rc = { 0 };
	static int iWidth = 0;
	static int iHeight = 0;

	static HWND hbutton = NULL;
	static HWND hform = NULL;
	static HWND hplot = NULL;

	static int size = 0;
	static int psize = 0;

	switch (uMsg) {
	case WM_ERASEBKGND:
		return -1;

	case WM_CREATE:
	{


		hform = CreateWindowEx(
			0, L"EDIT", L"Input Square Size", ES_NUMBER | WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, 0, 0,
			hwnd, (HMENU)ID_FORM, hGlobalInstance, NULL
		);

		hplot = CreateWindowEx(
			0, L"EDIT", L"Input Plot Size", ES_NUMBER | WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, 0, 0,
			hwnd, (HMENU)ID_FORM, hGlobalInstance, NULL
		);

		hbutton = CreateWindowEx(
			0, L"BUTTON", L"Start", BS_CENTER | BS_VCENTER | WS_VISIBLE | WS_BORDER | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
			0, 0, 0, 0,
			hwnd, (HMENU)ID_BUTTON, hGlobalInstance, NULL
		);

		pa.hform = hform;
		pa.hplot = hplot;
		pa.hbutton = hbutton;


		pa.hwnd = hwnd;

		ht = (HANDLE)_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)MonteRender, &pa, 0, &thId);

		if (ht != NULL) {
			DWORD dwExCode = 0x0;
			DWORD tstart = GetTickCount64();
			while (GetExitCodeThread(ht, &dwExCode) && dwExCode == STILL_ACTIVE && !PostThreadMessage(thId, WM_NULL, 0, 0))
			{
				if (GetTickCount64() - tstart > 10000) {
					SendMessage(hwnd, WM_DESTROY, 0, 0);
					break;
				}
				Sleep(1);
			}
		}

	}
	break;

	case WM_SIZE:
	{
		RECT tmprc = { 0 };
		GetClientRect(hwnd, &tmprc);
		int tmpWidth = tmprc.right - tmprc.left;
		int tmpHeight = tmprc.bottom - tmprc.top;

		if ((iHeight != tmpHeight) || (iWidth != tmpWidth)) {
			iHeight = tmpHeight;
			iWidth = tmpWidth;

			MoveWindow(hform, iWidth / 2 + 150, 100, 300, 30, TRUE);
			MoveWindow(hplot, iWidth / 2 + 150, 150, 300, 30, TRUE);
			MoveWindow(hbutton, iWidth / 2 + 150, 200, 100, 30, TRUE);
			InvalidateRect(hwnd, NULL, TRUE);
			UpdateWindow(hwnd);
		}
	}
	break;

	case WM_COMMAND:
	{
		switch (wParam) {
		case ID_BUTTON:
		{
			EnableWindow(hform, FALSE);
			EnableWindow(hplot, FALSE);
			EnableWindow(hbutton, FALSE);
			wchar_t innum[20] = { 0 };
			GetWindowText(hform, innum, 20);
			size = _wtoi(innum);

			wchar_t innum2[20] = { 0 };
			GetWindowText(hplot, innum2, 20);
			psize = _wtoi(innum2);

			if (size < 60001 && size > 0 && psize < 1000000001 && psize > 0) {
				PostThreadMessage(thId, TM_INPUT, size, psize);
				SetWindowText(hbutton, L"Calculating...");
			}
			else {
				MessageBox(hwnd, L"Invalid Value\r\nFill in the number from 1 to 60000\r\nPlot size fill in under 1000000000", L"Error", MB_OK);
				EnableWindow(hform, TRUE);
				EnableWindow(hplot, TRUE);
				EnableWindow(hbutton, TRUE);
			}

		}
		break;
		}
	}
	break;

	case WM_PAINT:
	{
		PAINTSTRUCT ps = {};
		HDC hdc = BeginPaint(hwnd, &ps);
		FillRect(hdc, &ps.rcPaint, (HBRUSH)GetStockObject(WHITE_BRUSH));
		if (pa.hBmp == NULL)TextOut(hdc, iWidth / 2 + 150, 50, L"Image is Unavailable", lstrlen(L"Image is Unavailable"));
		else StretchBlt(hdc, 15, (iHeight - iWidth / 2 - 30) / 2, iWidth / 2 - 30, iWidth / 2 - 30, pa.hdc, 0, 0, size, size, SRCCOPY);
		wchar_t cmes[128] = { 0 };
		wsprintf(cmes, L"plot %d/%d", pa.ploted, psize);
		TextOut(hdc, iWidth / 2 + 150, 350, cmes, lstrlen(cmes));

		wchar_t pist[128] = { 0 };
		swprintf_s(pist, L"pi = %lf", pa.PI_NOW);
		TextOut(hdc, iWidth / 2 + 150, 400, pist, lstrlen(pist));
		EndPaint(hwnd, &ps);
	}
	break;

	case WM_DESTROY:
		PostThreadMessage(thId, TM_DESTROY, 0, 0);
		WaitForSingleObject(ht, INFINITE);
		PostQuitMessage(0);
		return 0;

	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
