// final_project.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "zmouse.h"
#include <objidl.h>
#include <gdiplus.h>
#include <sstream>
#include <string>
#include <fstream>

#include "CommCtrl.h"
#include "OSFDlg.h"
#include <autoserial/autoserial.h>

//using namespace std;

#include "final_project.h"

//#include "afxdlgs.h"

#include "GMLFile.h"
#include "VertexEdgeMap.h"
#include "DoublyConnectedEdgeList.h"
#include "utils.h"

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
    // Message ID for IntelliMouse wheel
#endif

UINT uMSH_MOUSEWHEEL = 0;

//using namespace Gdiplus;
//#pragma comment (lib,"Gdiplus.lib")

#define MAX_LOADSTRING 100

#define ID_LOAD_GML 1001
#define ID_LOAD_DCEL 1002
#define ID_SAVE_AS_DCEL 1003

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
HWND                hWnd;
HWND				hWndStatus;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

/*--------------------------------------------------------------
 * Configuration
 *--------------------------------------------------------------*/

// Small screen configuration:
//unsigned int SCREENSIZE_X = 800;
//unsigned int SCREENSIZE_Y = 480;

// Large screen configuration:
//unsigned int SCREENSIZE_X = 1440;
//unsigned int SCREENSIZE_Y = 720;

// Auto screen configuration
unsigned int SCREENSIZE_X = CW_USEDEFAULT;
unsigned int SCREENSIZE_Y = CW_USEDEFAULT;

// 1080p fullscreen
//unsigned int SCREENSIZE_X = 1800;
//unsigned int SCREENSIZE_Y = 1080;

class Layer
{
public:
	Layer(std::string name, int layerNumber, bool on) 
		: layerNumber(layerNumber), name(name), on(on) { }
	std::string name;
	int layerNumber;
	bool on;
};

std::vector<Layer> layers;
std::vector< std::pair<dnl::Point, dnl::Point> > linesToPrint;

DoublyConnectedEdgeList *dcel = NULL;
GMLFile myGMLFile;
GMLFile myGMLFile2;
VertexEdgeMap edgeMap1;
VertexEdgeMap edgeMap2;

dnl::Point LLWindow = dnl::Point(0,0);
dnl::Point URWindow = dnl::Point(SCREENSIZE_X,SCREENSIZE_Y);

dnl::Point screenToUniverse(HWND hWnd, dnl::Point screenPoint)
{
	PWINDOWINFO windowInfo = new WINDOWINFO;
	GetWindowInfo(hWnd, windowInfo);
	RECT windowRect = windowInfo->rcWindow;
 
	double winX = (double)(windowRect.right - windowRect.left);
	double winY = (double)(windowRect.bottom - windowRect.top);

	double width = URWindow.m_x - LLWindow.m_x;
	double height = URWindow.m_y - LLWindow.m_y;
	double multX = winX / width;
	double multY = winY / height;

	dnl::Point returnPoint(
		(screenPoint.m_x / multX) + LLWindow.m_x,
		((screenPoint.m_y) / multY) + LLWindow.m_y);
	return returnPoint;
}

VOID OnPaint(HWND hWnd, HDC hdc)
{
	// For more info see
	// http://msdn.microsoft.com/en-us/library/ms533802%28v=VS.85%29.aspx

	// TODO: Get window size (incase it changed) and subtract/add 
	//       to our window size the X inc/dec and Y inc/dec

	Gdiplus::Graphics graphics(hdc);

	PWINDOWINFO windowInfo = new WINDOWINFO;
	GetWindowInfo(hWnd, windowInfo);
	RECT windowRect = windowInfo->rcWindow;

	int winWidth = windowRect.right - windowRect.left;
	int winHeight = windowRect.bottom - windowRect.top;
	float winX = (float)windowRect.right - windowRect.left;
	float winY = (float)windowRect.bottom - windowRect.top;
	
#ifdef _DEBUG
	// Output window lines
	Gdiplus::Pen      pen(Gdiplus::Color(0,0,0));
	// Horizontal & Vertical
	graphics.DrawLine(&pen, 0.0, winY/2, winX, winY/2);
	graphics.DrawLine(&pen, winX/2, 0.0, winX/2, winY);
#endif

	// Output graphics
	double width = URWindow.m_x - LLWindow.m_x;
	double height = URWindow.m_y - LLWindow.m_y;
	double multX = winX / width;
	double multY = winY / height;

	PrintManager printMan(LLWindow, URWindow, multX, multY, winWidth, winHeight, &hdc);

		for(unsigned int i = 0; i < layers.size(); i++)
	{
		if(layers[i].on == false)
		{
			continue;
		}

		switch(layers[i].layerNumber)
		{
		case 1:
		{
			if(dcel != NULL)
			{
				dcel->print(printMan, 2);
			}
			break;
		}
		case 2:
		{
			if(dcel != NULL)
			{
				dcel->print(printMan, 1);
			}
			break;
		}
		case 3:
		{
			if(dcel != NULL)
			{
				dcel->print(printMan, 3);
			}
			break;
		}
		case 4:
		{
			if(dcel != NULL)
			{
				dcel->print(printMan, 4);
			}
			break;
		}
		case 5:
		{
			if(dcel != NULL)
			{
				dcel->print(printMan, 5);
			}
			break;
		}
		default:
			break;
		}
	}

	// Debug point panning lines
	for(unsigned int i = 0; i < linesToPrint.size(); i++)
	{
		printMan.PrintLine(linesToPrint[i].first, linesToPrint[i].second);
	}

	// Output text
	std::wstringstream info;
	info << "CONTROLS:" << std::endl;
	info << "Left click and drag to pull the screen." << std::endl;
	info << "Right click to centre the screen on that location." << std::endl;
	info << "UP key / Scroll up to zoom in." << std::endl;
	info << "DOWN key / Scroll down to zoom out." << std::endl;
#ifdef _DEBUG
	info << " " << std::endl;
	info << "DEBUG INFO:" << std::endl;
	info << "URWindow = (" << URWindow.m_x << ", " << URWindow.m_y << ") " << std::endl;
	info << "LLWindow = (" << LLWindow.m_x << ", " << LLWindow.m_y << ") " << std::endl;
	info << "Centre = (" << (URWindow.m_x + LLWindow.m_x) / 2 << ", " << (URWindow.m_y + LLWindow.m_y) / 2 << ") " << std::endl;
#endif
	info << " " << std::endl;
	for(unsigned int i = 0; i < layers.size(); i++)
	{
		info << "[";
		info << ((layers[i].on) ? "ON" : "OFF");
		info << "] ";
		info << "Layer " << i+1 << ": " << layers[i].name.c_str() << std::endl;
	}

	printMan.PrintScreenText(info, dnl::Point(20,20));

}

void getLines(std::ifstream &stream, std::vector<std::string> & lines)
{
	std::string line;
	while (std::getline(stream, line))
	{
   		lines.push_back(line);	
	}
	assert(lines.size() > 0);
}

void centreWindowOnDCEL()
{
	// Centre window on DCEL file boundaries
	if(dcel != NULL)
	{
		PWINDOWINFO windowInfo = new WINDOWINFO;
		GetWindowInfo(hWnd, windowInfo);
		RECT windowRect = windowInfo->rcWindow;

		const double width = (windowRect.right - windowRect.left);
		const double height = (windowRect.bottom - windowRect.top);

		const double aspectRatio = width/height;

		double llX = dcel->m_llX;
		double llY = dcel->m_llY;
		double urX = dcel->m_urX;
		double urY = dcel->m_urY;

		double dataHeight = urY - llY;
		double dataWidth = urX - llX;

		if(aspectRatio > 1)
		{
			// width dominates aspect ratio
			dataHeight = urY - llY;
			dataWidth = dataHeight * aspectRatio;
			
			LLWindow.m_x = llX;
			LLWindow.m_y = llY + dataHeight;
			
			URWindow.m_x = LLWindow.m_x + dataWidth;
			URWindow.m_y = LLWindow.m_y - dataHeight;
		}
		else
		{
			// height dominates aspect ratio
			dataWidth = urX - llX;
			dataHeight = dataWidth * aspectRatio;

			LLWindow.m_x = llX;
			LLWindow.m_y = llY + dataHeight;
			
			URWindow.m_x = LLWindow.m_x + dataWidth;
			URWindow.m_y = LLWindow.m_y - dataHeight;
		}

		// Centre the two boxes

		// Find the centriods
		double dataCentreX = LLWindow.m_x + dataWidth / 2;
		double dataCentreY = LLWindow.m_y + dataHeight / 2;

	}
}

void closeOpenDCEL()
{
	if(dcel != NULL)
	{
		utils::getInstance()->setTextOnStatusBar(L"Closing DCEL.");

		delete dcel;
		layers.clear();
	}
}

bool loadFile(bool bUsingOpen, bool loadingDCEL, std::wstring &string)
{
	TCHAR dcelFilter[] = TEXT ("DCEL (*.dcel)\0*.dcel\0")  \
						TEXT ("GML (*.gml)\0*.gml\0")  \
						TEXT ("GIS Dataset Files (*.gml, *.dcel)\0*.gml;*.dcel\0")  \
                        TEXT ("All Files (*.*)\0*.*\0\0");
	TCHAR gmlFilter[] = TEXT ("GML (*.gml)\0*.gml\0")  \
						TEXT ("DCEL (*.dcel)\0*.dcel\0")  \
						TEXT ("GIS Dataset Files (*.gml, *.dcel)\0*.gml;*.dcel\0")  \
                        TEXT ("All Files (*.*)\0*.*\0\0");
	TCHAR szDefExtention[] = TEXT("dcel\0");

	PTSTR szFilter = loadingDCEL ? dcelFilter : gmlFilter;

	COSFDialog osfDlg;

	if(bUsingOpen)
	{
		if(osfDlg.FileOpenDlg(szFilter, szDefExtention,TEXT("Open GIS Dataset"),  FALSE))
		{
			string = osfDlg.GetFileName();
			return true;
		}
	}
	else
	{
		if(osfDlg.FileSaveDlg(szFilter, szDefExtention, NULL))
		{
			string = osfDlg.GetFileName();
			return true;
		}
	}
	return false;
}

void loadGML()
{
	// Close any file open
	closeOpenDCEL();

	std::wstring file;
	bool userCancelled = !loadFile(true, false, file);
	if(userCancelled)
	{
		return;
	}	

	std::string filename = utils::WStringToString(file);
	//std::string filename("NorthAmerica.gml");

	// Load GML
	std::wstring message(L"Step 1/3: Loading into RAM & Parsing GML File: ");
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(message);

	bool success = myGMLFile.parse(filename);
	if(!success)
	{
		ReportError("Unable to parse GML file: " + filename);
	}

	// Create EdgeMap
	message = L"Step 2/3: Constructing Vertex-EdgeMap for File: ";
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(message);

	success = edgeMap1.construct(&myGMLFile);
	if(!success)
	{
		ReportError("Unable to create edgeMap: " + filename);
	}

	// Create DCEL
	message = L"Step 3/3: Constructing Doubly Connected Edge List for File: ";
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(message);

	dcel = new DoublyConnectedEdgeList();
	success = dcel->construct(edgeMap1);
	if(!success)
	{
		ReportError("Unable to create DCEL: " + filename);
	}

	layers.push_back(Layer("DCEL Areas: " + filename, 1, true));
	layers.push_back(Layer("DCEL Lines: " + filename, 2, false));
	layers.push_back(Layer("DCEL Edge labels: " + filename, 3, false));
	layers.push_back(Layer("DCEL Vertex labels: " + filename, 4, false));
	layers.push_back(Layer("DCEL Face labels: " + filename, 5, false));

	centreWindowOnDCEL();

	message = L"Successfully loaded DCEL from GML: ";
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(message);
}

void loadDCEL()
{
	// Close any file open
	closeOpenDCEL();

	std::wstring file;
	bool userCancelled = !loadFile(true, true, file);
	if(userCancelled == false)
	{
		return;
	}	

	std::string filename = utils::WStringToString(file);
	//("NorthAmerica.gml.dcel");

	std::wstring message(L"Loading DCEL: ");
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(message);

	autoserial::BinaryFileReader bfr(filename.c_str());
	bfr.read((autoserial::ISerializable**)(&dcel));

	layers.push_back(Layer("DCEL Areas: " + filename, 1, true));
	layers.push_back(Layer("DCEL Lines: " + filename, 2, false));
	layers.push_back(Layer("DCEL Edge labels: " + filename, 3, false));
	layers.push_back(Layer("DCEL Vertex labels: " + filename, 4, false));
	layers.push_back(Layer("DCEL Face labels: " + filename, 5, false));

	centreWindowOnDCEL();

	message = L"Successfully loaded DCEL: ";
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(message);
}

void saveAsDCEL()
{
	std::wstring file;
	bool userCancelled = !loadFile(false, true, file);
	if(userCancelled)
	{
		return;
	}	

	std::string filename = utils::WStringToString(file);
	
	//std::string filename("NorthAmerica2.gml.dcel");


	autoserial::BinaryFileWriter bfw(filename.c_str());
    bfw.write(dcel);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

    INITCOMMONCONTROLSEX initctrls;
    initctrls.dwSize = sizeof(initctrls);
    initctrls.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&initctrls);

	utils *utilsInstance = utils::getInstance();

	HACCEL				hAccelTable;
	MSG                 msg;
	WNDCLASS            wndClass;
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
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

	HMENU hMenu, hSubMenu;
    HICON hIcon, hIconSm;

    hMenu = CreateMenu();

    hSubMenu = CreatePopupMenu();
    AppendMenu(hSubMenu, MF_STRING, ID_LOAD_GML, L"Load .&GML File");
    AppendMenu(hSubMenu, MF_STRING, ID_LOAD_DCEL, L"Load .&DCEL File");
    AppendMenu(hSubMenu, MF_STRING, ID_SAVE_AS_DCEL, L"Save as .&DCEL File");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hSubMenu, L"&File");

	hWnd = CreateWindow(
		TEXT("CS6345 Final Project: Doubly Connected Edge List"),   // window class name
		TEXT("CS6345 Final Project: Doubly Connected Edge List"),  // window caption
		WS_OVERLAPPEDWINDOW,		// window style
		CW_USEDEFAULT,				// initial x position
		CW_USEDEFAULT,				// initial y position
		SCREENSIZE_X,				// initial x size
		SCREENSIZE_Y,				// initial y size
		NULL,						// parent window handle
		hMenu,						// window menu handle
		hInstance,					// program instance handle
		NULL);						// creation parameters

	hWndStatus = CreateWindowEx( 
		0L,                              // no extended styles
		STATUSCLASSNAME,                 // status bar
		L"",                              // no text 
		WS_CHILD | WS_BORDER | WS_VISIBLE,  // styles
		-100, -100, 10, 10,              // x, y, cx, cy
		hWnd,                            // parent window
		(HMENU)100,                      // window ID
		hInstance,                           // instance
		NULL);                           // window data

	if (hWndStatus == NULL)
	{
		DWORD errorNumber = GetLastError();
		std::wstring errorMessage(L"Status Bar not created! \nError:");
		char buff[100] = {0};
		std::string errorNum(itoa(errorNumber, buff, 10));
		errorMessage += utils::StringToWString(errorNum);
		MessageBox (NULL, errorMessage.c_str(), NULL, MB_OK );
	}
	else
	{
		utilsInstance->hWndStatus = hWndStatus;
		utilsInstance->statusBarSet = true;
	}

	ShowWindow(hWnd, 1);
	UpdateWindow(hWnd);

	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Gdiplus::GdiplusShutdown(gdiplusToken);
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

bool mouseDown = false;
dnl::Point mouseDownPoint;

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
        case ID_LOAD_GML:
			loadGML();
			::InvalidateRect(hWnd, NULL, TRUE);
            break;
		case ID_LOAD_DCEL:
			loadDCEL();
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_SAVE_AS_DCEL:
			saveAsDCEL();
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
			ptsCursor = MAKEPOINTS(lParam); 
			ptsCursor.x;
			ptsCursor.y;

			// TODO: Handle the PAN operation
			mouseDownPoint.m_x;
			mouseDownPoint.m_y;

			// for now just print a line from where we pressed
			// down the mouse to where we let it up.

			dnl::Point begin = screenToUniverse(hWnd, mouseDownPoint);
			dnl::Point end = screenToUniverse(hWnd, dnl::Point(ptsCursor.x, ptsCursor.y));
			//linesToPrint.push_back( std::pair<dnl::Point, dnl::Point>(begin, end)); 

			const double xChange = end.m_x - begin.m_x;
			const double yChange = end.m_y - begin.m_y;
			LLWindow.m_x -= xChange;
			URWindow.m_x -= xChange;
			LLWindow.m_y -= yChange;
			URWindow.m_y -= yChange;

			mouseDown = false;
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_LBUTTONDOWN:
		{
			ptsCursor = MAKEPOINTS(lParam); 
			ptsCursor.x;
			ptsCursor.y;

			mouseDown = true;
			mouseDownPoint.m_x = ptsCursor.x;
			mouseDownPoint.m_y = ptsCursor.y;
			break;
		}
	case WM_RBUTTONUP:
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
			break;
		}
	case WM_RBUTTONDBLCLK:
		{
			break;
		}
        case WM_MOUSEWHEEL:
		{
			((short) HIWORD(wParam)< 0) ? zoomOut() : zoomIn();
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
	case WM_CHAR: 
	  // Process "displayable" characters
		{
			int ch = (TCHAR) wParam; 
			
			const int layerNumber = ch - '0';
			
			if(layerNumber > 9 || layerNumber < 1)
			{
				break;
			}

			if(layerNumber < layers.size() + 1)
			{
				layers[layerNumber-1].on = !layers[layerNumber-1].on;
				::InvalidateRect(hWnd, NULL, TRUE);
			}

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