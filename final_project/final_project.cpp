// final_project.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#include "Strip.h"
#include "Point.h"
#include <sstream>
#include <string>

#include "final_project.h"

#include "zmouse.h"

//
// Mouse Wheel rotation stuff, only define if we are
// on a version of the OS that does not support
// WM_MOUSEWHEEL messages.
//
#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
    // Message ID for IntelliMouse wheel
#endif

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);






/*--------------------------------------------------------------
 * Configuration
 *--------------------------------------------------------------*/
unsigned int SCREENSIZE_X = 800;
unsigned int SCREENSIZE_Y = 480;








dnl::Point LLWindow = dnl::Point(0,0);
dnl::Point URWindow = dnl::Point(100,100);

void DrawLine(HDC hdc, REAL x1, REAL y1, REAL x2, REAL y2)
{
	Graphics graphics(hdc);

	Pen      pen(Color(255, 0, 0, 255));
	graphics.DrawLine(&pen, x1, y1, x2, y2);
}

VOID OnPaint(HWND hWnd, HDC hdc)
{
	// For more info see
	// http://msdn.microsoft.com/en-us/library/ms533802%28v=VS.85%29.aspx


	// TODO: Get window size (incase it changed) and subtract/add 
	//       to our window size the X inc/dec and Y inc/dec

	Graphics graphics(hdc);

	PWINDOWINFO windowInfo = new WINDOWINFO;
	GetWindowInfo(hWnd, windowInfo);
	RECT windowRect = windowInfo->rcWindow;

	float winX = (float)windowRect.right - windowRect.left;
	float winY = (float)windowRect.bottom - windowRect.top;
	
#ifdef _DEBUG
	// Output window lines
	Pen      pen(Color(0,0,0));
	// Horizontal & Vertical
	graphics.DrawLine(&pen, 0.0, winY/2, winX, winY/2);
	graphics.DrawLine(&pen, winX/2, 0.0, winX/2, winY);
#endif

	// Output text
	FontFamily  fontFamily(L"Helvetica");
	Font        font(&fontFamily, 12, FontStyleRegular, UnitPixel);
	SolidBrush  solidBrush(Color(255, 0, 0, 0));

	std::wstringstream info;
	info << "CONTROLS:" << std::endl;
	info << "Left click to centre the screen on that location." << std::endl;
	info << "UP key / Scroll up to zoom in." << std::endl;
	info << "DOWN key / Scroll down to zoom out." << std::endl;
#ifdef _DEBUG
	info << " " << std::endl;
	info << "DEBUG INFO:" << std::endl;
	info << "URWindow = (" << URWindow.m_x << ", " << URWindow.m_y << ") " << std::endl;
	info << "LLWindow = (" << LLWindow.m_x << ", " << LLWindow.m_y << ") " << std::endl;
	info << "Centre = (" << (URWindow.m_x + LLWindow.m_x) / 2 << ", " << (URWindow.m_y + LLWindow.m_y) / 2 << ") " << std::endl;
#endif

	for(int i = 0; true; i++)
	{
		WCHAR currLine[1000] = {0};
		info.getline(currLine, 1000);
		if(currLine[0] == 0)
		{
			break;
		}

		PointF      pointF(20.0f, i*18.0f + 20.0f);
		graphics.SetTextRenderingHint(TextRenderingHintAntiAlias);
		graphics.DrawString(currLine, -1, &font, pointF, &solidBrush);
	}
	
	// Output graphics

	double width = URWindow.m_x - LLWindow.m_x;
	double height = URWindow.m_y - LLWindow.m_y;
	double multX = winX / width;
	double multY = winY / height;

	PrintManager printMan(LLWindow, URWindow, multX, multY, &hdc);

	Strip myStrip(dnl::Point(10, 10), dnl::Point(200, 200), 50, 80);
	myStrip.print(printMan);

	// Print Grid lines
	printMan.PrintGridY(2000, -1000, 1000, -1000, 1000);
	printMan.PrintGridX(2000, -1000, 1000, -1000, 1000);
	printMan.PrintGridY(500, -1000, 1000, -1000, 1000);
	printMan.PrintGridX(500, -1000, 1000, -1000, 1000);
	printMan.PrintGridY(125, -1000, 1000, -1000, 1000);
	printMan.PrintGridX(125, -1000, 1000, -1000, 1000);
	printMan.PrintGridY(31.25, -1000, 1000, -1000, 1000, &printMan.m_superSeeThroughBlue);
	printMan.PrintGridX(31.25, -1000, 1000, -1000, 1000, &printMan.m_superSeeThroughBlue);

	// Print Starflake
	printMan.PrintLine(dnl::Point(-1000,-1000), dnl::Point(1000,1000));
	printMan.PrintLine(dnl::Point(-500,-1000), dnl::Point(500,1000));
	printMan.PrintLine(dnl::Point(0,-1000), dnl::Point(0,1000));
	printMan.PrintLine(dnl::Point(500,-1000), dnl::Point(-500,1000));
	printMan.PrintLine(dnl::Point(1000,-1000), dnl::Point(-1000,1000));
	printMan.PrintLine(dnl::Point(1000,-500), dnl::Point(-1000,500));
	printMan.PrintLine(dnl::Point(1000,0), dnl::Point(-1000,0));
	printMan.PrintLine(dnl::Point(1000,500), dnl::Point(-1000,-500));


	// Draw origin
	/*dnl::Point originPointLL(originX - 40, originY - 40);
	dnl::Point originPointUR(originX + 40, originY + 40);
	Strip origin(originPointLL, originPointUR, 40, 40);
	origin.print(printMan);

	//DrawRectangle(hdc, 10, 10, 100, 50);

	// Draw Arc
	{
		Pen pen(Color(255, 255, 0, 0));
		GraphicsPath path;
		path.AddArc(175, 50, 50, 50, 0, -180);
		graphics.DrawPath(&pen, &path);
	}

	// Draw two figures
	{
		Point points[] = {Point(40, 60), Point(50, 70), Point(30, 90)};

		Pen pen(Color(255, 255, 0, 0), 2);
		GraphicsPath path;

		// The first figure is started automatically, so there is
		// no need to call StartFigure here.
		path.AddArc(175, 50, 50, 50, 0.0f, -180.0f);
		path.AddLine(100, 0, 250, 20);

		path.StartFigure();
		path.AddLine(50, 20, 5, 90);
		path.AddCurve(points, 3);
		path.AddLine(50, 150, 150, 180);
		path.CloseFigure();

		graphics.DrawPath(&pen, &path);
	}

	// Draw filled shapes
	{
		GraphicsPath path;
		Pen          pen(Color(255, 255, 0, 0), 2);
		SolidBrush   brush(Color(255, 0, 0, 200));

		path.AddLine(10, 10, 100, 40);
		path.AddLine(100, 60, 30, 60);
		path.AddRectangle(Rect(50, 35, 20, 40));
		path.AddEllipse(10, 75, 40, 30);

		graphics.DrawPath(&pen, &path);
		graphics.FillPath(&brush, &path);
	}*/
}

UINT uMSH_MOUSEWHEEL = 0;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	HACCEL				hAccelTable;
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	uMSH_MOUSEWHEEL = RegisterWindowMessage(MSH_MOUSEWHEEL); 
    if ( !uMSH_MOUSEWHEEL )
    {
        MessageBox(NULL, L"Register Mouse Scroll Failed!", L"Error",MB_OK);
        return msg.wParam;
    }

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FINAL_PROJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FINAL_PROJECT));

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style          = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc    = WndProc;
	wndClass.cbClsExtra     = 0;
	wndClass.cbWndExtra     = 0;
	wndClass.hInstance      = hInstance;
	wndClass.hIcon          = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground  = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName   = NULL;
	wndClass.lpszClassName  = TEXT("CS6345 Final Project: Doubly Connected Edge List");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("CS6345 Final Project: Doubly Connected Edge List"),   // window class name
		TEXT("CS6345 Final Project: Doubly Connected Edge List"),  // window caption
		WS_OVERLAPPEDWINDOW,      // window style
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		SCREENSIZE_X/*CW_USEDEFAULT*/,            // initial x size
		SCREENSIZE_Y/*CW_USEDEFAULT*/,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	PWINDOWINFO windowInfo = new WINDOWINFO;
	GetWindowInfo(hWnd, windowInfo);
	RECT windowRect = windowInfo->rcWindow;

	const double width = (windowRect.right - windowRect.left);
	const double height = (windowRect.bottom - windowRect.top);

	LLWindow.m_x = 0 - (width / 2);
	LLWindow.m_y = 0 - (height / 2);
	URWindow.m_x = LLWindow.m_x + width;
	URWindow.m_y = LLWindow.m_y + height;

	ShowWindow(hWnd, 1);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FINAL_PROJECT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FINAL_PROJECT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


const double zoomFactor = 2;

void zoomIn()
{
	double centreX = (LLWindow.m_x + URWindow.m_x) / 2;
	double centreY = (LLWindow.m_y + URWindow.m_y) / 2;

	double width = (URWindow.m_x - LLWindow.m_x) / zoomFactor;
	double height = (URWindow.m_y - LLWindow.m_y) / zoomFactor;

	LLWindow.m_x = centreX - (width/2);
	LLWindow.m_y = centreY - (height/2);
	URWindow.m_x = LLWindow.m_x + width;
	URWindow.m_y = LLWindow.m_y + height;
}

void zoomOut()
{
	double centreX = (LLWindow.m_x + URWindow.m_x) / 2;
	double centreY = (LLWindow.m_y + URWindow.m_y) / 2;

	double width = (URWindow.m_x - LLWindow.m_x) * zoomFactor;
	double height = (URWindow.m_y - LLWindow.m_y) * zoomFactor;

	LLWindow.m_x = centreX - (width/2);
	LLWindow.m_y = centreY - (height/2);
	URWindow.m_x = LLWindow.m_x + width;
	URWindow.m_y = LLWindow.m_y + height;

}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	POINTS ptsCursor;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		OnPaint(hWnd, hdc);
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_LBUTTONUP:
		{	
			PWINDOWINFO windowInfo = new WINDOWINFO;
			GetWindowInfo(hWnd, windowInfo);
			RECT windowRect = windowInfo->rcWindow;

			ptsCursor = MAKEPOINTS(lParam); 
			ptsCursor.x;
			ptsCursor.y;

			double width = URWindow.m_x - LLWindow.m_x;
			double height = URWindow.m_y - LLWindow.m_y;

			double windowWidth = windowRect.right - windowRect.left;
			double windowHeight = windowRect.bottom - windowRect.top;

			double percentOfWindowX = ptsCursor.x / windowWidth;
			double percentOfWindowY = ptsCursor.y / windowHeight;
			double pointInUniverseX = (percentOfWindowX * width) + LLWindow.m_x;
			double pointInUniverseY = (percentOfWindowY * height) + LLWindow.m_y;
			
			LLWindow.m_x = pointInUniverseX - (width/2);
			LLWindow.m_y = pointInUniverseY - (height/2);
			URWindow.m_x = LLWindow.m_x + width;
			URWindow.m_y = LLWindow.m_y + height;

			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_LBUTTONDBLCLK:
		{
			// For more information see
			// http://msdn.microsoft.com/en-us/library/ms645602%28v=VS.85%29.aspx#processing_dblclick
			/*ptsCursor = MAKEPOINTS(lParam); 

			double width = URWindow.m_x - LLWindow.m_x;
			double height = URWindow.m_y - LLWindow.m_y;

			width *= 1.5;
			height *= 1.5;

			LLWindow.m_x = LLWindow.m_x - (width / 4);
			URWindow.m_x = URWindow.m_x + (width / 4);

			LLWindow.m_y = LLWindow.m_y - (height / 4);
			URWindow.m_y = URWindow.m_y + (height / 4);*/

			/*LLWindow.m_x = ptsCursor.x - (width / 2);
			LLWindow.m_y = ptsCursor.y - (height / 2);

			URWindow.m_x = LLWindow.m_x + width;
			URWindow.m_y = LLWindow.m_y + height;*/

			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_RBUTTONDBLCLK:
		{
			/*ptsCursor = MAKEPOINTS(lParam);
			
			double width = URWindow.m_x - LLWindow.m_x;
			double height = URWindow.m_y - LLWindow.m_y;

			width /= 1.5;
			height /= 1.5;

			LLWindow.m_x = LLWindow.m_x + (width / 4);
			URWindow.m_x = URWindow.m_x - (width / 4);

			LLWindow.m_y = LLWindow.m_y + (height / 4);
			URWindow.m_y = URWindow.m_y - (height / 4);*/

			/*LLWindow.m_x = ptsCursor.x - (width / 2);
			LLWindow.m_y = ptsCursor.y - (height / 2);

			URWindow.m_x = LLWindow.m_x + width;
			URWindow.m_y = LLWindow.m_y + height;*/

			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
        case WM_MOUSEWHEEL:
		{
			((short) HIWORD(wParam)< 0) ? zoomOut() : zoomIn();
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_KEYDOWN:
      switch (wParam)
      {
        case VK_HOME:
          // Insert code here to process the HOME key
          // ...
          break;

        case VK_END:
          // Insert code here to process the END key
          // ...
          break;

        case VK_INSERT:
          // Insert code here to process the INS key
          // ...
          break;

        case VK_F2:
          // Insert code here to process the F2 key
          // ...
          break;

        case VK_LEFT:
          // Insert code here to process the LEFT ARROW key
          // ...
          break;

        case VK_RIGHT:
          // Insert code here to process the RIGHT ARROW key
          // ...
          break;

        case VK_UP:
          // Insert code here to process the UP ARROW key
			zoomIn();
			::InvalidateRect(hWnd, NULL, TRUE);
          break;

        case VK_DOWN:
          // Insert code here to process the DOWN ARROW key
			zoomOut();
			::InvalidateRect(hWnd, NULL, TRUE);
          break;

        case VK_DELETE:
          // Insert code here to process the DELETE key
          // ...
          break;
        
        default:
          // Insert code here to process other noncharacter keystrokes
          // ...
          break;
      } 
	default:

	    if( message == uMSH_MOUSEWHEEL )
        {
           ((int)wParam < 0) ? zoomOut() : zoomIn();
			::InvalidateRect(hWnd, NULL, TRUE);
            break;
        }

		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}