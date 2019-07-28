#define UNICODE
#define _CRT_RAND_S

#include <Windows.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include <wchar.h>

#include "render thread.h"

DWORD WINAPI MonteRender(void* lpdata) {
	DWORD retvalue = 0x00000000;

	PassArg* tpa = (PassArg*)lpdata;
	HWND thwnd = tpa->hwnd;

	MSG tmsg = { 0 };
	BOOL rendflag = FALSE;
	int SquareSize = 0;
	int PlotSize = 0;

	int plotcount = 0;
	int plot_in = 0;
	int plot_out = 0;

	DWORD* image = NULL;

	void* pv = nullptr;

	DWORD start = GetTickCount64();

	while (1) {

		while (PeekMessage(&tmsg, NULL, 0, 0, PM_REMOVE) > 0) {
			switch (tmsg.message) {
			case TM_INPUT:
			{
				SquareSize = (int)tmsg.wParam;
				PlotSize = (int)tmsg.lParam;

				DeleteDC(tpa->hdc);
				tpa->hdc = CreateCompatibleDC(NULL);
				DeleteObject(tpa->hBmp);



				free(image);
				image = (DWORD*)calloc(SquareSize * SquareSize, sizeof(DWORD));
				if (image == NULL) {
					MessageBox(thwnd, L"FAILED TO ALLOCATE", L"CAUTION", MB_OK);

					EnableWindow(tpa->hform, TRUE);
					EnableWindow(tpa->hplot, TRUE);
					EnableWindow(tpa->hbutton, TRUE);
					SetWindowText(tpa->hbutton, L"Start");

					break;
				}

				for (int i = 0; i < SquareSize * SquareSize; i++)image[i] = 0x00ffffff;

				BITMAPINFOHEADER bmi = {
					sizeof(BITMAPINFOHEADER), SquareSize, SquareSize, 1, 32, 0, 0, 0, 0, 0, 0
				};

				tpa->hBmp = CreateDIBSection(tpa->hdc, (BITMAPINFO*)& bmi, DIB_RGB_COLORS, &pv, NULL, 0);

				//if (tpa->hBmp == NULL)MessageBox(thwnd, L"FAILED TO CDIBS", L"CAUTION", MB_OK);

				SelectObject(tpa->hdc, tpa->hBmp);

				for (int i = 0; i < SquareSize * SquareSize; i++)((DWORD*)pv)[i] = image[i];

				plotcount = 0;
				plot_in = 0;
				plot_out = 0;
				rendflag = TRUE;
			}
			break;

			case TM_DESTROY:
				goto Fine;
			}
		}


		if (rendflag == TRUE) {
			
			tpa->ploted = plotcount;

			unsigned int plot_x = 0;
			unsigned int plot_y = 0;

			srand(time(NULL));
			rand_s(&plot_x);
			rand_s(&plot_y);

			plot_x %= SquareSize;
			plot_y %= SquareSize;

			float sqsq = SquareSize * SquareSize / 4.0;

			float distance = ((float)plot_x - (float)SquareSize / 2.0) * ((float)plot_x - (float)SquareSize / 2.0) + ((float)plot_y - (float)SquareSize / 2.0) * ((float)plot_y - (float)SquareSize / 2.0);

			wchar_t pist[128] = { 0 };
			swprintf_s(pist, L"pi = %lf", distance);
			//MessageBox(thwnd, pist, L"CAUTION", MB_OK);

			if (distance <= sqsq) {
				((DWORD*)pv)[plot_y * SquareSize + plot_x] = IN_COLOR;
				plot_in++;
			}
			else {
				((DWORD*)pv)[plot_y * SquareSize + plot_x] = OUT_COLOR;
				plot_out++;
			}




			if (GetTickCount64() - start > 100) {
				tpa->PI_NOW = (float)plot_in / (float)plotcount * 4.0;

				InvalidateRect(thwnd, NULL, TRUE);
				UpdateWindow(thwnd);
				start = GetTickCount64();
			}

			plotcount++;


			/* More Slowely */
			//Sleep(1);

			if (plotcount > PlotSize) {
				tpa->PI_NOW = (float)plot_in / (float)plotcount * 4.0;

				InvalidateRect(thwnd, NULL, TRUE);
				UpdateWindow(thwnd);

				EnableWindow(tpa->hform, TRUE);
				EnableWindow(tpa->hplot, TRUE);
				EnableWindow(tpa->hbutton, TRUE);
				SetWindowText(tpa->hbutton, L"Start");

				rendflag = FALSE;
			}
		}
	}

Fine:
	free(image);
	_endthreadex(0);
	return retvalue;
}