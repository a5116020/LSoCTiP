#pragma once

#include <Windows.h>

#define TM_CREATE WM_APP + WM_CREATE
#define TM_INPUT WM_APP + WM_SIZE
#define TM_DESTROY WM_APP + WM_DESTROY

constexpr int CLR_RED = 0x00ff0000;
constexpr int CLR_GREEN = 0x0000ff00;
constexpr int CLR_BLUE = 0x000000ff;
constexpr int CLR_BLACK = 0x00000000;

#define IN_COLOR CLR_RED
#define OUT_COLOR CLR_BLUE

typedef struct _tagPassArg {
	HWND hwnd = NULL;
	HWND hform = NULL;
	HWND hplot = NULL;
	HWND hbutton = NULL;
	HDC hdc = NULL;
	HBITMAP hBmp = NULL;
	int ploted = 0;
	float PI_NOW = 0;
}PassArg;

DWORD WINAPI MonteRender(void* lpdata);