#include <Windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <atlImage.h>
#include <math.h>
#include "resource.h"

#define IDC_CHECK1 101
#define IDC_CHECK2 102
#define IDC_CHECK3 103
#define IDC_CHECK4 104
#define IDC_CHECK5 105
#define IDC_CHECK6 106
#define IDC_CHECK7 107
#define IDC_CHECK8 108

#define IDC_MAP 201

LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//LRESULT CALLBACK ChildProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HINSTANCE g_hinst;
LPCTSTR lpszClass = TEXT("First");

BOOL CollisionCheck(int x, int y, int mx, int my);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow) //메인
{
	HWND hWnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hinst = hInstance;

	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground =
		(HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);

	WndClass.hCursor = LoadCursor(NULL, IDC_HELP);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	WndClass.lpszClassName = L"ChildClass"; // 차일드 윈도우 클래스 이름
	WndClass.lpfnWndProc = (WNDPROC)ChildProc; // 차일드 윈도우 프로시저 지정
	RegisterClassEx(&WndClass);
	//WndClass.lpfnWndProc = (WNDPROC)ChildProc2; // 차일드 윈도우 프로시저 지정
	hWnd = CreateWindow(lpszClass, NULL, WS_OVERLAPPEDWINDOW, 0, 0, 1000, 800, NULL, (HMENU)NULL, hInstance, NULL);

	//WndClass.hCursor = LoadCursor(NULL, IDC_HELP);
	//WndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
	//WndClass.lpszClassName = "ChildClass2"; // 차일드 윈도우 클래스 이름
	//WndClass.lpfnWndProc = (WNDPROC)ChildProc2; // 차일드 윈도우 프로시저 지정
	//RegisterClassEx(&WndClass);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&Message, 0, 0, 0))
	{
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}

struct Picture {
	int index = 0;
};

struct map {
	int index = 0;
	int Background;
};
HDC hdc, memdc, memdc2;

static int Select = 0;
static int CheckCount = 0;
static Picture picture[7];
static Picture Release;
static int selection = 1;

static map Map1[8][8];
static map Map2[8][8];
static map Map3[8][8];

static map Total[24][8];
static BOOL Complete = FALSE;
static int checkT = 0;
OPENFILENAME SFN;    // 파일열기와 저장하기는 동일한 구조체 사용 
TCHAR str[100], lpstrFile[100] = L"";
TCHAR filter[100] = L"Every File(*.*)\0*.*\0Text File \0 *.txt;*.doc \0 ";
OPENFILENAME OFN;

inline LRESULT CALLBACK WndProc(HWND hWnd, UINT iMessage, WPARAM wParam, LPARAM lParam) //CALLBACK(윈도우 핸들, 처리해야 할 메시지의 값,입력장치의 값1,2)
{
	PAINTSTRUCT ps;
	HWND hButton;
	static int check = 0;
	static int count = 0;
	char text[100];
	TCHAR InBuff[1000];
	TCHAR OnBuff1[200];
	TCHAR OnBuff2[200];
	TCHAR OnBuff3[200];
	static HANDLE hFile;
	DWORD c = 10;
	static HWND hCheck[2], CLOSE, board, Comfirm, SelectList, Success, Save, Load;
	TCHAR Items[][15] = { L"1번 그림", L"2번 그림", L"3번 그림", L"4번 그림", L"5번 그림" , L"6번 그림" };
	static int cList[2];
	static DWORD sizeT = 1000;
	static TCHAR info[100];
	static TCHAR info2[100];
	static TCHAR info3[100];

	static TCHAR tempMap1[8][8];
	static TCHAR tempMap2[8][8];
	static TCHAR tempMap3[8][8];
	

	switch (iMessage) {
	case WM_CREATE:
		hdc = GetDC(hWnd);
		//		board1 = CreateWindow("CHILD", "좀 나와바" , WS_CHILD | WS_CHILD | WS_BORDER, 50, 50, 300, 300, hWnd, NULL, NULL, NULL);

		hCheck[0] = CreateWindow(L"button", L"<<<<<", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 400, 50, 50, hWnd, (HMENU)IDC_CHECK1, g_hinst, NULL);
		hCheck[1] = CreateWindow(L"button", L">>>>>", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 530, 400, 50, 50, hWnd, (HMENU)IDC_CHECK2, g_hinst, NULL);
		//	Comfirm = CreateWindow("button", "적용", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 450, 140, 20, hWnd, (HMENU)IDC_CHECK3, g_hinst, NULL);
		Success = CreateWindow(L"button", L"완성", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 480, 140, 20, hWnd, (HMENU)IDC_CHECK6, g_hinst, NULL);
		Save = CreateWindow(L"button", L"저장", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 550, 140, 20, hWnd, (HMENU)IDC_CHECK7, g_hinst, NULL);
		Load = CreateWindow(L"button", L"불러오기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 700, 600, 140, 20, hWnd, (HMENU)IDC_CHECK8, g_hinst, NULL);
		board = CreateWindowEx(WS_EX_WINDOWEDGE, L"ChildClass", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 110, 200, 400, 400, hWnd, (HMENU)IDC_MAP, g_hinst, NULL);
		CLOSE = CreateWindow(L"button", L"종료", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 820, 700, 140, 30, hWnd, (HMENU)IDC_CHECK4, g_hinst, NULL);
		SelectList = CreateWindow(L"listbox", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | LBS_STANDARD, 700, 300, 140, 130, hWnd, (HMENU)IDC_CHECK5, g_hinst, NULL);

		for (int i = 0; i < 6; ++i) {
			SendMessage(SelectList, LB_ADDSTRING, 0, (LPARAM)Items[i]);
		}
		//	hList = CreateWindow("check", "테스트중", WS_CHILD | WS_VISIBLE | BS_BOX


		ReleaseDC(hWnd, hdc);
		break;

	case WM_COMMAND:
		switch (LOWORD(wParam)) {

		case IDC_CHECK1:
			if (Select > 0)
				Select--;
			break;

		case IDC_CHECK2:
			if (Select < 2)
				Select++;
			break;

		case IDC_CHECK3:

			break;

		case IDC_CHECK5:
			if (HIWORD(wParam) == LBN_SELCHANGE) {
				selection = SendMessage(SelectList, LB_GETCURSEL, 0, 0) + 1;
			}

			break;

		case IDC_CHECK6:
			//		hButton = GetDlgItem(hWnd, IDC_CHECK3);
			//		EnableWindow(hButton, FALSE); // 시작버튼 비활성화
			//		Complete = TRUE;

			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j)
					Total[i][j].index = Map1[i][j].index;
			}

			for (int i = 8; i < 16; ++i) {
				for (int j = 0; j < 8; ++j)
					Total[i][j].index = Map2[i - 8][j].index;
			}

			for (int i = 16; i < 24; ++i) {
				for (int j = 0; j < 8; ++j)
					Total[i][j].index = Map3[i - 16][j].index;
			}

			Complete = TRUE;

			break;

		case IDC_CHECK4:
			PostQuitMessage(0);
			break;

		case IDC_CHECK7:
			memset(&OFN, 0, sizeof(OPENFILENAME));  // 초기화  
			SFN.lStructSize = sizeof(OPENFILENAME);
			SFN.hwndOwner = hWnd;
			SFN.lpstrFilter = filter;
			SFN.lpstrFile = lpstrFile;
			SFN.nMaxFile = 256;
			SFN.lpstrInitialDir = L".";
			if (GetSaveFileName(&SFN) != 0) {
				wsprintf(str, L"%s 파일에 저장하시겠습니까 ?", SFN.lpstrFile);
				MessageBox(hWnd, str, L"저장하기 선택", MB_OK);
			}

			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					tempMap1[i][j] = Map1[i][j].index + '0';
					tempMap2[i][j] = Map2[i][j].index + '0';
					tempMap3[i][j] = Map3[i][j].index + '0';
				}
			}
			hFile = CreateFile(lpstrFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0);

			for (int i = 0; i < 8; ++i) {
				lstrcpy(OnBuff1, tempMap1[i]);
				SetFilePointer(hFile, 0, NULL, FILE_END);
				WriteFile(hFile, OnBuff1, lstrlen(OnBuff1) + lstrlen(OnBuff1), &c, NULL); // OutBuff의 내용을 hFile의 끝에 저장
			}
			CloseHandle(hFile);

			hFile = CreateFile(L"map2.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0);
			for (int i = 0; i < 8; ++i) {
				lstrcpy(OnBuff2, tempMap2[i]);
				SetFilePointer(hFile, 0, NULL, FILE_END);
				WriteFile(hFile, OnBuff2, lstrlen(OnBuff2) + lstrlen(OnBuff2), &c, NULL); // OutBuff의 내용을 hFile의 끝에 저장
			}
			CloseHandle(hFile);

			hFile = CreateFile(L"map3.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, 0, 0);
			for (int i = 0; i < 8; ++i) {
				lstrcpy(OnBuff3, tempMap3[i]);
				SetFilePointer(hFile, 0, NULL, FILE_END);
				WriteFile(hFile, OnBuff3, lstrlen(OnBuff3) + lstrlen(OnBuff3), &c, NULL); // OutBuff의 내용을 hFile의 끝에 저장
			}
			CloseHandle(hFile);
			break;

		case IDC_CHECK8:
			memset(&OFN, 0, sizeof(OPENFILENAME)); // 초기화
			OFN.lStructSize = sizeof(OPENFILENAME);
			OFN.hwndOwner = hWnd;
			OFN.lpstrFilter = filter;
			OFN.lpstrFile = lpstrFile;
			OFN.nMaxFile = 256;
			OFN.lpstrInitialDir = L"."; // 초기 디렉토리
			if (GetOpenFileName(&OFN) != 0) {
				wsprintf(str, L"%s 파일을 여시겠습니까 ?", OFN.lpstrFile);
				MessageBox(hWnd, str, L"열기 선택", MB_OK);
			}
			int point = 0;
			hFile = CreateFile(lpstrFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			ReadFile(hFile, info, sizeT, &sizeT, NULL);
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					Map1[i][j].index = info[point++] - '0';
				}
			}
			CloseHandle(hFile);

			point = 0;
			hFile = CreateFile(L"map2.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			ReadFile(hFile, info2, sizeT, &sizeT, NULL);
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					Map2[i][j].index = info2[point++] - '0';
				}
			}
			CloseHandle(hFile);

			point = 0;
			hFile = CreateFile(L"map3.txt", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			ReadFile(hFile, info3, sizeT, &sizeT, NULL);
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					Map3[i][j].index = info3[point++] - '0';
				}
			}
			CloseHandle(hFile);


			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_CHAR:


		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);



		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, iMessage, wParam, lParam);
}


LRESULT CALLBACK ChildProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

	PAINTSTRUCT ps;
	HBITMAP Block[6], BackGround[3];
	static HBITMAP hbit, oldBit1, oldBit2;;
	static int x, y;
	static int sizeX, sizeY;
	static int moveX = 0;
	switch (uMsg) {

	case WM_CREATE:
		//	InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_TIMER:
		switch (wParam) {
		case 1:
			moveX += 1;
			break;
		}
		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_LBUTTONDOWN:
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (Select == 0 && Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (x > i * 50 && x < (i + 1) * 50 && y > j * 50 && y < (j + 1) * 50)
						Map1[i][j].index = selection;
				}
			}
		}
		else if (Select == 1 && Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (x > i * 50 && x < (i + 1) * 50 && y > j * 50 && y < (j + 1) * 50)
						Map2[i][j].index = selection;
				}
			}
		}
		else if (Select == 2 && Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (x > i * 50 && x < (i + 1) * 50 && y > j * 50 && y < (j + 1) * 50)
						Map3[i][j].index = selection;
				}
			}
		}


		InvalidateRect(hWnd, NULL, FALSE);
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		memdc = CreateCompatibleDC(hdc);
		memdc2 = CreateCompatibleDC(memdc);

		hbit = CreateCompatibleBitmap(hdc, 400, 400);

		Block[0] = LoadBitmap(g_hinst, MAKEINTRESOURCE(101));
		Block[1] = LoadBitmap(g_hinst, MAKEINTRESOURCE(102));
		Block[2] = LoadBitmap(g_hinst, MAKEINTRESOURCE(103));
		Block[3] = LoadBitmap(g_hinst, MAKEINTRESOURCE(104));
		Block[4] = LoadBitmap(g_hinst, MAKEINTRESOURCE(105));
		Block[5] = LoadBitmap(g_hinst, MAKEINTRESOURCE(106));

		for (int i = 0; i < 3; ++i)
			BackGround[i] = LoadBitmap(g_hinst, MAKEINTRESOURCE(120 + i));

		oldBit1 = (HBITMAP)SelectObject(memdc, hbit);
		oldBit2 = (HBITMAP)SelectObject(memdc2, NULL);
		//

		if (Select == 0 && Complete == FALSE) {
			(HBITMAP)SelectObject(memdc2, BackGround[0]);
			StretchBlt(memdc, 0, 0, 400, 400, memdc2, 0, 0, 300, 168, SRCCOPY);
		}
		else if (Select == 1 && Complete == FALSE) {
			(HBITMAP)SelectObject(memdc2, BackGround[1]);
			StretchBlt(memdc, 0, 0, 400, 400, memdc2, 0, 0, 276, 182, SRCCOPY);
		}
		else if (Select == 2 && Complete == FALSE) {
			(HBITMAP)SelectObject(memdc2, BackGround[2]);
			StretchBlt(memdc, 0, 0, 400, 400, memdc2, 0, 0, 283, 178, SRCCOPY);
		}
		else if (Complete == TRUE) {
			(HBITMAP)SelectObject(memdc2, BackGround[0]);
			StretchBlt(memdc, 0 - moveX, 0, 400, 400, memdc2, 0, 0, 300, 168, SRCCOPY);
			(HBITMAP)SelectObject(memdc2, BackGround[1]);
			StretchBlt(memdc, 400 - moveX, 0, 400, 400, memdc2, 0, 0, 276, 182, SRCCOPY);
			(HBITMAP)SelectObject(memdc2, BackGround[2]);
			StretchBlt(memdc, 800 - moveX, 0, 400, 400, memdc2, 0, 0, 283, 178, SRCCOPY);
		}

		if (Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				MoveToEx(memdc, (400 / 8) * (i + 1), 0, NULL);
				LineTo(memdc, (400 / 8) * (i + 1), 400);
			}
			for (int i = 0; i < 8; ++i) {
				MoveToEx(memdc, 0, (400 / 8) * (i + 1), NULL);
				LineTo(memdc, 400, (400 / 8) * (i + 1));
			}
		}

		if (Select == 0 && Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (Map1[i][j].index != 0) {
						(HBITMAP)SelectObject(memdc2, Block[Map1[i][j].index - 1]);
						StretchBlt(memdc, i * 50, j * 50, 50, 50, memdc2, 0, 0, 50, 50, SRCCOPY);
					}
				}
			}
		}

		else if (Select == 1 && Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (Map2[i][j].index != 0) {
						(HBITMAP)SelectObject(memdc2, Block[Map2[i][j].index - 1]);
						StretchBlt(memdc, i * 50, j * 50, 50, 50, memdc2, 0, 0, 50, 50, SRCCOPY);
					}
				}
			}
		}

		else if (Select == 2 && Complete == FALSE) {
			for (int i = 0; i < 8; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (Map3[i][j].index != 0) {
						(HBITMAP)SelectObject(memdc2, Block[Map3[i][j].index - 1]);
						StretchBlt(memdc, i * 50, j * 50, 50, 50, memdc2, 0, 0, 50, 50, SRCCOPY);
					}
				}
			}
		}
		
		else if (Complete == TRUE) {
			if (CheckCount == 0)
				SetTimer(hWnd, 1, 10, NULL);

			for (int i = 0; i < 24; ++i) {
				for (int j = 0; j < 8; ++j) {
					if (Total[i][j].index != 0) {
						(HBITMAP)SelectObject(memdc2, Block[Total[i][j].index - 1]);
						StretchBlt(memdc, i * 50 - moveX, j * 50, 50, 50, memdc2, 0, 0, 50, 50, SRCCOPY);
					}
				}
			}
		}



		BitBlt(hdc, 0, 0, 400, 400, memdc, 0, 0, SRCCOPY);
		
		for (int i = 0; i < 6; ++i)
			DeleteObject(Block[i]);

		DeleteObject(oldBit1);
		DeleteObject(oldBit2);
		DeleteObject(hbit);
		DeleteDC(memdc);
		DeleteDC(memdc2);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
