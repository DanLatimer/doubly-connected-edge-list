// final_project.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "zmouse.h"
#include <objidl.h>
#include <gdiplus.h>
#include <sstream>
#include <string>
#include <fstream>
#include <time.h>

#include "CommCtrl.h"
#include "OSFDlg.h"
#include "InputBox.h"
#include <autoserial/autoserial.h>

//using namespace std;

#include "final_project.h"

//#include "afxdlgs.h"

#include "GMLFile.h"
#include "VertexEdgeMap.h"
#include "DoublyConnectedEdgeList.h"
#include "utils.h"
#include "Clock.h"

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
#define ID_CLOSE 1004
#define ID_QUERY_FACE_EDGES 1005
#define ID_QUERY_VERTEX_EDGES 1006
#define ID_ORTHOGONAL_RANGE_SEARCH 1007

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

// Orthogonal Range Search Variables
bool acceptingPointsForRangeSearch = false;
bool orthogonalRangeSearchRectangleDefined = false;
int rangeSearchPointsCollected = 0;
//data
dnl::Point rangeSearchPoint1;
dnl::Point rangeSearchPoint2;
std::set<int> faceIndicies;
std::set<int> faceEdgeIndicies;



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
	Layer(std::string name, int layerNumber, bool on, int edgeType = 0, int index = 0) 
		: layerNumber(layerNumber), name(name), on(on), edgeType(edgeType), index(index) { }
	std::string name;
	int layerNumber;
	int edgeType;
	int index;
	bool on;
};

std::vector<Layer> layers;
std::vector< std::pair<dnl::Point, dnl::Point> > linesToPrint;

DoublyConnectedEdgeList *dcel = NULL;
GMLFile *myGMLFile = NULL;
VertexEdgeMap *edgeMap = NULL;

dnl::Point LLWindow = dnl::Point(0,0);
dnl::Point URWindow = dnl::Point(SCREENSIZE_X,SCREENSIZE_Y);

CInputBox *m_myinputbox;

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
		{
			// Vertex Edges or Face Edges
			if(layers[i].edgeType == 1)
			{
				// Print Edges around Face
				if(dcel != NULL)
				{
					dcel->print(printMan, 6, layers[i].index);
				}
			}
			else if(layers[i].edgeType == 2)
			{
				// Print Edges around Vertex
				if(dcel != NULL)
				{
					dcel->print(printMan, 7, layers[i].index);
				}
			}
			else if(layers[i].edgeType == 3)
			{
				// Orthogonal Range Search
				if(dcel != NULL)
				{
					// print search rectangle
					printMan.PrintRectangle(rangeSearchPoint1, rangeSearchPoint2);
					
					std::set<int>::iterator iter;
					for(iter = faceEdgeIndicies.begin(); iter != faceEdgeIndicies.end(); iter++ )
					{
						const dnl::Point &begin = dcel->m_VERTEX[dcel->m_edges[*iter].vertex1];
						const dnl::Point &end = dcel->m_VERTEX[dcel->m_edges[*iter].vertex2];

						printMan.PrintLine(begin, end, &printMan.m_solidRed, 2);
					}
					
//std::set<int> faceIndicies;
//std::set<int> faceEdgeIndicies;

				}
			}
			else
			{
				// do nothing
			}
			break;
		}
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
		utils::getInstance()->setTextOnStatusBar(2, L"Closing DCEL.");

		delete dcel;
		dcel = NULL;
		layers.clear();
	}

	if(myGMLFile != NULL)
	{
		delete myGMLFile;
		myGMLFile = NULL;
	}

	if(edgeMap != NULL)
	{
		delete edgeMap;
		edgeMap = NULL;
	}
}

std::string pathToFilename(const std::string &str)
{
	size_t found;
	found=str.find_last_of("/\\");
	return str.substr(found+1);
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
		if(osfDlg.FileOpenDlg(szFilter, szDefExtention,TEXT("Open GIS Dataset..."),  FALSE))
		{
			string = osfDlg.GetFileName();
			return true;
		}
	}
	else
	{
		if(osfDlg.FileSaveDlg(szFilter, szDefExtention, TEXT("Save As...")))
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
	std::string fileOnly = pathToFilename(filename);

	// Load GML
	std::wstring message(L"Step 1/3: Loading into RAM & Parsing GML File: ");
	message += utils::StringToWString(fileOnly).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);

	myGMLFile = new GMLFile(); 
	bool success = myGMLFile->parse(filename);
	if(!success)
	{
		ReportError("Unable to parse GML file: " + filename);
	}

	// Create EdgeMap
	message = L"Step 2/3: Constructing Vertex-EdgeMap for File: ";
	message += utils::StringToWString(fileOnly).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);

	edgeMap = new VertexEdgeMap();
	success = edgeMap->construct(myGMLFile);
	if(!success)
	{
		ReportError("Unable to create edgeMap: " + filename);
	}

	// Create DCEL
	message = L"Step 3/3: Constructing Doubly Connected Edge List for File: ";
	message += utils::StringToWString(fileOnly).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);

	dcel = new DoublyConnectedEdgeList();
	success = dcel->construct(*edgeMap);
	if(!success)
	{
		ReportError("Unable to create DCEL: " + filename);
	}

	layers.push_back(Layer("DCEL Areas: " + fileOnly, 1, true));
	layers.push_back(Layer("DCEL Lines: " + fileOnly, 2, false));
	layers.push_back(Layer("DCEL Edge labels: " + fileOnly, 3, false));
	layers.push_back(Layer("DCEL Vertex labels: " + fileOnly, 4, false));
	layers.push_back(Layer("DCEL Face labels: " + fileOnly, 5, false));

	centreWindowOnDCEL();

	message = L"Successfully loaded DCEL from GML: ";
	message += utils::StringToWString(fileOnly).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);
}

void loadDCEL()
{
	// Close any file open
	closeOpenDCEL();

	std::wstring file;
	bool userCancelled = !loadFile(true, true, file);
	if(userCancelled)
	{
		return;
	}	

	std::string filename = utils::WStringToString(file);
	std::string fileOnly = pathToFilename(filename);

	std::wstring message(L"Loading DCEL: ");
	message += utils::StringToWString(fileOnly).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);

	autoserial::BinaryFileReader bfr(filename.c_str());
	bfr.read((autoserial::ISerializable**)(&dcel));

	layers.push_back(Layer("DCEL Areas: " + fileOnly, 1, true));
	layers.push_back(Layer("DCEL Lines: " + fileOnly, 2, false));
	layers.push_back(Layer("DCEL Edge labels: " + fileOnly, 3, false));
	layers.push_back(Layer("DCEL Vertex labels: " + fileOnly, 4, false));
	layers.push_back(Layer("DCEL Face labels: " + fileOnly, 5, false));

	centreWindowOnDCEL();

	dcel->constructKDTree();

	message = L"Successfully loaded DCEL: ";
	message += utils::StringToWString(fileOnly).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);
}

void saveAsDCEL()
{
	std::wstring file = utils::StringToWString(layers[0].name);
	bool userCancelled = !loadFile(false, true, file);
	if(userCancelled)
	{
		return;
	}	

	std::string filename = utils::WStringToString(file);

	std::wstring message(L"Saving DCEL As: ");
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);

	autoserial::BinaryFileWriter bfw(filename.c_str());
    bfw.write(dcel);

	message = L"Successfully saved DCEL As: ";
	message += utils::StringToWString(filename).c_str();
	utils::getInstance()->setTextOnStatusBar(2, message);
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	m_myinputbox = new CInputBox(hInstance);

    INITCOMMONCONTROLSEX initctrls;
    initctrls.dwSize = sizeof(initctrls);
    initctrls.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&initctrls);

	LLWindow.m_x = -180;
	LLWindow.m_y = 180;

	URWindow.m_x = LLWindow.m_x + 360;
	URWindow.m_y = LLWindow.m_y - 360;

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

	HMENU hMenu, hFileSubMenu, hQuerySubMenu;
    HICON hIcon, hIconSm;

    hMenu = CreateMenu();

    hFileSubMenu = CreatePopupMenu();
    AppendMenu(hFileSubMenu, MF_STRING, ID_LOAD_GML, L"Load .&GML File");
    AppendMenu(hFileSubMenu, MF_STRING, ID_LOAD_DCEL, L"Load .&DCEL File");
    AppendMenu(hFileSubMenu, MF_STRING, ID_SAVE_AS_DCEL, L"&Save as .DCEL File");
    AppendMenu(hFileSubMenu, MF_STRING, ID_CLOSE, L"&Close File");
    AppendMenu(hFileSubMenu, MF_STRING, IDM_EXIT, L"&Exit");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hFileSubMenu, L"&File");

	hQuerySubMenu = CreatePopupMenu();
    AppendMenu(hQuerySubMenu, MF_STRING, ID_QUERY_FACE_EDGES, L"Edges of &Face");
    AppendMenu(hQuerySubMenu, MF_STRING, ID_QUERY_VERTEX_EDGES, L"Edges on &Vertex");
    AppendMenu(hQuerySubMenu, MF_STRING, ID_ORTHOGONAL_RANGE_SEARCH, L"Orthogonal &Range Search");
    AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT)hQuerySubMenu, L"&Query DCEL");

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

		int statwidths[] = {100, 250, -1};
		SendMessage(hWndStatus, SB_SETPARTS, sizeof(statwidths)/sizeof(int), (LPARAM)statwidths);
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

void performOrthogonalRangeSearch()
{
	Clock totalTimer;
	double latMin, latMax, longMin, longMax;
	utils::getBoundingBox(rangeSearchPoint1, rangeSearchPoint2, latMin, longMin, latMax, longMax);

	std::vector<KDTreeNode*> nodesFound;
	
	double rangeSearchTime;
	bool success = false;
	{
		Clock findEdgeTimer;

		nodesFound = dcel->m_KDTree->Search(latMin, longMin, latMax, longMax);

		rangeSearchTime = findEdgeTimer.getMillisecondsElapsed();
	}

	// Find all edges incident on verticies found
	std::set<int> vertexEdgeIndicies;
	int vertexEdgesExamined = 0;
	double findVertexEdgesTime = 0;
	for(unsigned int i = 0; i < nodesFound.size(); i++)
	{
		std::vector<DirectedEdge> currentEdges;

		Clock findVertexEdgesTimer;
		success = dcel->findEdgesOfVertex(nodesFound[i]->m_vertexIndex, currentEdges);
		findVertexEdgesTime += findVertexEdgesTimer.getMillisecondsElapsed();

		assert(success);

		vertexEdgesExamined += currentEdges.size();
		for(unsigned int i = 0; i < currentEdges.size(); i++)
		{
			vertexEdgeIndicies.insert(currentEdges[i].edge);
		}
	}

	// Find all faces on those edges
	Clock findFacesOfEdges;
	faceIndicies.clear();
	std::set<int>::iterator iter;
	for(iter = vertexEdgeIndicies.begin(); iter != vertexEdgeIndicies.end(); iter++)
	{
		faceIndicies.insert(dcel->m_edges[*iter].face1);
		faceIndicies.insert(dcel->m_edges[*iter].face2);
	}
	const double findFacesOfEdgesTime = findFacesOfEdges.getMillisecondsElapsed();


	// Acquire all edges on those faces
	double findFaceEdgesTime = 0;
	int faceEdgesExamined = 0;
	faceEdgeIndicies.clear();
	for(iter = faceIndicies.begin(); iter != faceIndicies.end(); iter++)
	{
		std::vector<DirectedEdge> currentEdges;

		Clock findFaceEdgesTimer;
		success = dcel->findEdgesOfFace(*iter, currentEdges);
		findFaceEdgesTime += findFaceEdgesTimer.getMillisecondsElapsed();

		assert(success);

		faceEdgesExamined += currentEdges.size();
		for(unsigned int i = 0; i < currentEdges.size(); i++)
		{
			faceEdgeIndicies.insert(currentEdges[i].edge);
		}
	}

	const double totalTime = totalTimer.getMillisecondsElapsed();


	std::wstring queryReport;
	if(success)
	{
		// Orthogonal Range Query
		queryReport = L"Step 1: Querying verticies within search rectangle (";
			queryReport += utils::StringToWString(utils::parseDouble(latMin));
			queryReport += L", ";
			queryReport += utils::StringToWString(utils::parseDouble(longMin));
			queryReport += L") (";
			queryReport += utils::StringToWString(utils::parseDouble(latMax));
			queryReport += L", ";
			queryReport += utils::StringToWString(utils::parseDouble(longMax));
			queryReport += L").\n";
		queryReport += L"Time expended: ";
		queryReport += utils::StringToWString(utils::parseDouble(rangeSearchTime));
			queryReport += L" milliseconds\n";
		queryReport += L"Total verticies: ";
			queryReport += utils::StringToWString(utils::parseLong(dcel->m_VERTEX.size()));
			queryReport += L"\n";
		queryReport += L"verticies examined: TBD";
			//queryReport += utils::StringToWString(utils::parseLong(edges.size()));
			queryReport += L"\n";
		queryReport += L"Verticies found: ";
			queryReport += utils::StringToWString(utils::parseLong(nodesFound.size()));
			queryReport += L"\n\n";

		// Find edges Incident on Verticies
		queryReport += L"Step 2: Querying edges incident on those verticies.\n";
		queryReport += L"Time expended: ";
		queryReport += utils::StringToWString(utils::parseDouble(findVertexEdgesTime));
			queryReport += L" milliseconds\n";
		queryReport += L"Total edges: ";
			queryReport += utils::StringToWString(utils::parseLong(dcel->m_edges.size()));
			queryReport += L"\n";
		queryReport += L"Edges examined: ";
			queryReport += utils::StringToWString(utils::parseLong(vertexEdgesExamined));
			queryReport += L"\n";
		queryReport += L"Unique edges found: ";
			queryReport += utils::StringToWString(utils::parseLong(vertexEdgeIndicies.size()));
			queryReport += L"\n\n";

		// Find faces on those edges
		queryReport += L"Step 3: Create a set of faces that are on either side of those edges.\n";
		queryReport += L"Time expended: ";
		queryReport += utils::StringToWString(utils::parseDouble(findFacesOfEdgesTime));
			queryReport += L" milliseconds\n";
		queryReport += L"Total faces: ";
			queryReport += utils::StringToWString(utils::parseLong(dcel->m_firstOccuranceOfFace.size()));
			queryReport += L"\n";
		queryReport += L"Faces examined: ";
			queryReport += utils::StringToWString(utils::parseLong(vertexEdgeIndicies.size() * 2));
			queryReport += L"\n";
		queryReport += L"Unique faces found: ";
			queryReport += utils::StringToWString(utils::parseLong(faceIndicies.size()));
			queryReport += L"\n\n";


		// Find edges around on both faces of those edges
		queryReport += L"Step 4: Querying edges surrounding those faces.\n";
		queryReport += L"Time expended: ";
		queryReport += utils::StringToWString(utils::parseDouble(findFaceEdgesTime));
			queryReport += L" milliseconds\n";
		queryReport += L"Total edges: ";
			queryReport += utils::StringToWString(utils::parseLong(dcel->m_edges.size()));
			queryReport += L"\n";
		queryReport += L"Edges examined: ";
			queryReport += utils::StringToWString(utils::parseLong(faceEdgesExamined));
			queryReport += L"\n";
		queryReport += L"Unique edges found: ";
			queryReport += utils::StringToWString(utils::parseLong(faceEdgeIndicies.size()));
			queryReport += L"\n\n";

		// Total time
		queryReport += L"Time audit.\n";
		queryReport += L"Total time expended: ";
		queryReport += utils::StringToWString(utils::parseDouble(totalTime));
			queryReport += L" milliseconds\n";
		queryReport += L"Total data structure query times: ";
		queryReport += utils::StringToWString(utils::parseDouble(rangeSearchTime + findVertexEdgesTime + findFaceEdgesTime));
			queryReport += L" milliseconds\n";
	}
	else
	{
		queryReport = L"Query Failed.";
	}

	MessageBox(hWnd, queryReport.c_str(), L"Orthogonal Range Query Report", 0);

	std::string layerName("Orthogonal range search (");
	layerName += utils::parseDouble(latMin);
	layerName += ", ";
	layerName += utils::parseDouble(longMin);
	layerName += ") (";
	layerName += utils::parseDouble(latMax);
	layerName += ", ";
	layerName += utils::parseDouble(longMax);
	layerName += ")";
	layers.push_back(Layer(layerName, layers.size()+1, true, 3, 0));
	::InvalidateRect(hWnd, NULL, TRUE);
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
		case ID_CLOSE:
			closeOpenDCEL();
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		case ID_QUERY_FACE_EDGES:
		{
			if(dcel == NULL)
			{
				MessageBox(hWnd, L"There is no Doubly Connected Edge List open.", L"Error", 0);
				break;
			}

			wchar_t inputbuffer[100] = {0};

			const int numberOfFaces = dcel->m_firstOccuranceOfFace.size();
			std::wstring message(L"Enter the face number (0 to ");
			message += utils::StringToWString(utils::parseLong(numberOfFaces-1));
			message += L").";

			const int result = m_myinputbox->ShowInputBox(
				0, L"Query Face's Edges", message.c_str(), inputbuffer, 20);
			if(result != S_OK)
			{
				// Add a layer that only print's that face's edges/edge labels
				int faceIndex = utils::parseInt(utils::WStringToString(inputbuffer));
				
				if(faceIndex < 0 || faceIndex >= numberOfFaces)
				{
					std::wstring errorMsg(L"Face index provided \"");
					errorMsg += inputbuffer;
					errorMsg += L"\" is outside of range (0 to ";
					errorMsg += utils::StringToWString(utils::parseLong(numberOfFaces-1));
					errorMsg += L")";
					MessageBox(hWnd, errorMsg.c_str(), L"Error", 0);
					break;
				}

				std::vector<DirectedEdge> edges;

				double elapsedTime;
				bool success = false;
				{
					LARGE_INTEGER frequency;        // ticks per second
					LARGE_INTEGER t1, t2;           // ticks

					// get ticks per second
					QueryPerformanceFrequency(&frequency);

					// start timer
					QueryPerformanceCounter(&t1);

					// Perform Timed Action
					success = dcel->findEdgesOfFace(faceIndex, edges);

					// stop timer
					QueryPerformanceCounter(&t2);

					// compute and print the elapsed time in millisec
					elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
				}

				std::wstring queryReport;
				if(success)
				{
					queryReport = L"Querying edges of face ";
						queryReport += inputbuffer;
						queryReport += L" completed successfully!\n\n";
					queryReport += L"Time expended: ";
					queryReport += utils::StringToWString(utils::parseDouble(elapsedTime));
						queryReport += L" milliseconds\n";
					queryReport += L"Total edges: ";
						queryReport += utils::StringToWString(utils::parseLong(dcel->m_edges.size()));
						queryReport += L"\n";
					queryReport += L"Edges examined: ";
						queryReport += utils::StringToWString(utils::parseLong(edges.size()));
						queryReport += L"\n";
				}
				else
				{
					queryReport = L"Query Failed.";
				}

				MessageBox(hWnd, queryReport.c_str(), L"Query Report", 0);

				std::string layerName("Edges around Face #");
				layerName += utils::WStringToString(inputbuffer);
				layers.push_back(Layer(layerName, layers.size()+1, true, 1, faceIndex));
			}
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case ID_QUERY_VERTEX_EDGES:
		{
			if(dcel == NULL)
			{
				MessageBox(hWnd, L"There is no Doubly Connected Edge List open.", L"Error", 0);
				break;
			}

			wchar_t inputbuffer[100] = {0};

			const int numberOfVerticies = dcel->m_firstOccuranceOfVertex.size();
			std::wstring message(L"Enter the vertex number (0 to ");
			message += utils::StringToWString(utils::parseLong(numberOfVerticies-1));
			message += L").";

			const int result = m_myinputbox->ShowInputBox(
				0, L"Query Vertex's Edges", message.c_str(), inputbuffer, 20);
			if(result != S_OK)
			{
				// Add a layer that only print's that vertex's edges/edge labels
				int vertexIndex = utils::parseInt(utils::WStringToString(inputbuffer));
				
				if(vertexIndex < 0 || vertexIndex >= numberOfVerticies)
				{
					std::wstring errorMsg(L"Vertex index provided \"");
					errorMsg += inputbuffer;
					errorMsg += L"\" is outside of range (0 to ";
					errorMsg += utils::StringToWString(utils::parseLong(numberOfVerticies-1));
					errorMsg += L")";
					MessageBox(hWnd, errorMsg.c_str(), L"Error", 0);
					break;
				}

				std::vector<DirectedEdge> edges;
				double elapsedTime;
				bool success = false;
				{
					Clock findEdgeTimer;

					success = dcel->findEdgesOfVertex(vertexIndex, edges);

					elapsedTime = findEdgeTimer.getMillisecondsElapsed();
				}

				std::wstring queryReport;
				if(success)
				{
					queryReport = L"Querying edges incident on vertex ";
						queryReport += inputbuffer;
						queryReport += L" completed successfully!\n\n";
					queryReport += L"Time expended: ";
					queryReport += utils::StringToWString(utils::parseDouble(elapsedTime));
						queryReport += L" milliseconds\n";
					queryReport += L"Total edges: ";
						queryReport += utils::StringToWString(utils::parseLong(dcel->m_edges.size()));
						queryReport += L"\n";
					queryReport += L"Edges examined: ";
						queryReport += utils::StringToWString(utils::parseLong(edges.size()));
						queryReport += L"\n";
				}
				else
				{
					queryReport = L"Query Failed.";
				}

				MessageBox(hWnd, queryReport.c_str(), L"Query Report", 0);

				std::string layerName("Edges around Vertex #");
				layerName += utils::WStringToString(inputbuffer);
				layers.push_back(Layer(layerName, layers.size()+1, true, 2, vertexIndex));
			}
			::InvalidateRect(hWnd, NULL, TRUE);
			break;
		}
		case ID_ORTHOGONAL_RANGE_SEARCH:
		{
			if(dcel == NULL)
			{
				MessageBox(hWnd, L"There is no Doubly Connected Edge List open.", L"Error", 0);
				break;
			}

			if(dcel->m_KDTree == NULL)
			{
				// Construct KDTree for the first time
				MessageBox(hWnd, L"The datastructure to perform range search (KDTree) has not yet been "
					L"constructed. This will only happen once per file. Press Okay to construct.", L"Information", 0);
				dcel->constructKDTree();
			}

			// Remove previous orthogonal range search if one exists
			for(unsigned int i = 0; i < layers.size(); i++)
			{
				if(layers[i].edgeType == 3)
				{
					layers.erase(layers.begin() + i);
					i--;
				}
			}

			std::wstring errorMsg(
				L"Starting orthogonal range search point collection.\n\n"
				L"Please click on the map in the two points that defines your search rectangle.\n\n"
				L"You will not see the rectangle as you are defining it since time for implementation was limitted. "
				L"The status bar, however will be updated to show your progress.");
			MessageBox(hWnd, errorMsg.c_str(), L"Orthogonal Range Search Query Rectange Defining.", 0);

			acceptingPointsForRangeSearch = true;
			rangeSearchPointsCollected = 0;

			utils::getInstance()->setTextOnStatusBar(2, L"Click map to choose first point of your rectangle. (Lat1, Long1, Lat2, Long2)");
			break;
		}
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

			dnl::Point begin = screenToUniverse(hWnd, mouseDownPoint);
			dnl::Point end = screenToUniverse(hWnd, dnl::Point(ptsCursor.x, ptsCursor.y));

			// May be collecting points for range search
			if(acceptingPointsForRangeSearch)
			{
				if(rangeSearchPointsCollected == 0)
				{
					rangeSearchPoint1 = end;
					rangeSearchPointsCollected++;

					std::wstring message(L"Click map to choose second point of your rectangle. (");
					message += utils::StringToWString(utils::parseDouble(rangeSearchPoint1.m_x));
					message += L", ";
					message += utils::StringToWString(utils::parseDouble(rangeSearchPoint1.m_y));
					message += L", Lat2, Long2)";
					utils::getInstance()->setTextOnStatusBar(2, message);
				}
				else if(rangeSearchPointsCollected == 1)
				{
					rangeSearchPoint2 = end;

					// reset global variables
					orthogonalRangeSearchRectangleDefined = true;
					acceptingPointsForRangeSearch = false;
					rangeSearchPointsCollected = 0;

					std::wstring message(L"Orthogonal Range Search Rectangle choosen (");
					message += utils::StringToWString(utils::parseDouble(rangeSearchPoint1.m_x));
					message += L", ";
					message += utils::StringToWString(utils::parseDouble(rangeSearchPoint1.m_y));
					message += L", ";
					message += utils::StringToWString(utils::parseDouble(rangeSearchPoint2.m_x));
					message += L", ";
					message += utils::StringToWString(utils::parseDouble(rangeSearchPoint2.m_y));
					message += L")";
					utils::getInstance()->setTextOnStatusBar(2, message);

					performOrthogonalRangeSearch();
				}
				else
				{
					assert(0);
				}
				break;
			}

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
	case WM_MOUSEMOVE:
	{
		POINT coord;
		GetCursorPos(&coord);
		ScreenToClient(hWnd, &coord);
		dnl::Point currentMousePosition = screenToUniverse(hWnd, dnl::Point(coord.x, coord.y));

		std::wstring message(L"(");
		message += utils::StringToWString(utils::parseDouble(currentMousePosition.m_x));
		message += L", ";
		message += utils::StringToWString(utils::parseDouble(currentMousePosition.m_y));
		message += L")";
		utils::getInstance()->setTextOnStatusBar(1, message);
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