/*
 * DialogWindow.cpp
 *
 *  Created on: 06 feb. 2011
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
#include "DialogWindow.h"
#include "Error.h"
#include "Security.h"

static DialogWindow* myInterfacePtr;

bool DialogWindow::isPosStored_ = false ;
int  DialogWindow::savedSizeX_ , DialogWindow::savedSizeY_ ;
int  DialogWindow::savedX_     , DialogWindow::savedY_     ;

LRESULT CALLBACK DialogLoop(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	switch (message)
	{
    case WM_INITDIALOG:    break;

  	case WM_PAINT:

      hdc = BeginPaint(hWnd, &ps);
      
      myInterfacePtr -> displayToWindow();
      
		  EndPaint(hWnd, &ps);
		  break;

    case WM_LBUTTONDOWN:
      myInterfacePtr -> mouseButton_ = 1 ;
      myInterfacePtr -> detectAction();
      break;

    case WM_LBUTTONUP:
      myInterfacePtr -> mouseButton_ = 0 ;
      myInterfacePtr -> detectAction();
      break;

    case WM_MOUSEMOVE: 
      myInterfacePtr -> mouseX_ = LOWORD(lParam);
      myInterfacePtr -> mouseY_ = HIWORD(lParam);

      myInterfacePtr -> detectAction();
      myInterfacePtr -> requestRedraw();
      break;
        
    case WM_SIZE:
    {
      // Client area Size
      int windowSizeX=LOWORD(lParam);
      int windowSizeY=HIWORD(lParam);

      myInterfacePtr -> resizeMainWindow(windowSizeX, windowSizeY);
      myInterfacePtr -> requestRedraw();

      break;
    }

    case WM_CLOSE:    myInterfacePtr->destroy();                 break;
	  case WM_DESTROY:  PostQuitMessage(0);	                       break;
    case WM_COMMAND:  myInterfacePtr->doAction(LOWORD(wParam));	 break;
    
	  default:       	 return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

void DialogWindow::InitInstance(HINSTANCE hInstance, HWND parenthwnd, int nCmdShow)
{
  interfacehInst_ = hInstance; // Stocke le handle d'instance dans la variable globale

  DWORD style =  WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_THICKFRAME | WS_MAXIMIZEBOX ;

  interfacehWnd_ = CreateWindow(szWindowClass_, szTitle_, style,
                      savedX_, savedY_, 
                      savedSizeX_, 
                      savedSizeY_, 
                      parenthwnd, NULL, hInstance, NULL);

  if (!interfacehWnd_)
    THROW_INTERNAL_ERROR( "Windows could not create the Crop window" );

  // create a static window for the main screen
  paintZonehWnd_ = CreateWindow(TEXT("STATIC"), TEXT(""), 
                WS_CHILD | WS_VISIBLE | SS_OWNERDRAW ,
                0, 0,          // Position 
                savedSizeX_, savedSizeY_,      // dimensions
                interfacehWnd_, NULL, hInstance, NULL);
  isActive_ = true;

  if (!paintZonehWnd_)
    THROW_INTERNAL_ERROR( "Windows could not create a static" );

  ShowWindow(interfacehWnd_, nCmdShow);
  UpdateWindow(interfacehWnd_);
}

ATOM DialogWindow::registerClass(HINSTANCE hInstance, int titleressource, 
                                 int windowclassressource, int iconressource )
{
	WNDCLASSEX wcex;

  ZeroMemory(&wcex, sizeof(WNDCLASSEX));

	wcex.cbSize       = sizeof(WNDCLASSEX);
	wcex.style			  = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc	= DialogLoop;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			  = LoadIcon(hInstance, MAKEINTRESOURCE(iconressource));
	wcex.hCursor		  = LoadCursor(NULL, IDC_ARROW);
	wcex.lpszMenuName	= NULL;
	wcex.lpszClassName= szWindowClass_;
	wcex.hIconSm		  = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	// Initialise les chaînes globales
	LoadString(hInstance, titleressource,       szTitle_,       MAX_LOADSTRING);
	LoadString(hInstance, windowclassressource, szWindowClass_, MAX_LOADSTRING);

	return RegisterClassEx(&wcex);
}

DialogWindow::DialogWindow(HINSTANCE hInstance, HWND parenthwnd, int titleressource, 
                           int windowclassressource, int iconressource, int acceleratorressource)
{
  myInterfacePtr  = this;
  isActive_ = false ;

  if (!isPosStored_)
  {
    RECT r;
    GetWindowRect(parenthwnd, &r);
    savedSizeX_     = r.right  - r.left ;
    savedSizeY_     = r.bottom - r.top ;
    savedX_         = r.left ;
    savedY_         = r.top ;

    // moves the window on the left hand side of main window
    savedX_ -= savedSizeX_;
    if (savedX_<0) // if it interstects the left corner of screen, then let it touch the border
      savedX_ = 0 ;
  }

  if (savedSizeX_ < 560) savedSizeX_ = 560;
  if (savedSizeX_ < 500) savedSizeX_ = 500;

  interfaceImage_ = Image(savedSizeX_,savedSizeY_,1,3);
  interfacehInst_ = hInstance;
  interfaceParenthwnd_ = parenthwnd;
  mouseButton_    = mouseX_ = mouseY_ = 0 ;

	registerClass(hInstance, titleressource, windowclassressource, iconressource);
  ressourceAccelerator_ = acceleratorressource;

}

void DialogWindow::launchDialog(bool ismodal)
{
	MSG msg;
	HACCEL hAccelTable;
  isModal_ = ismodal;

	// Effectue l'initialisation de l'application :
	InitInstance (interfacehInst_, interfaceParenthwnd_, 1);

  if (isModal_)
    EnableWindow(interfaceParenthwnd_, FALSE);

  hAccelTable = LoadAccelerators(interfacehInst_, MAKEINTRESOURCE(ressourceAccelerator_));

	// Boucle de messages principale :
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

  //SetForegroundWindow(interfaceParenthwnd_);
}


void DialogWindow::resizeMainWindow(int newSizeX, int newSizeY)
{
  // resizes the STATIC window control
  MoveWindow(paintZonehWnd_, 0, 0, newSizeX, newSizeY, TRUE);
  interfaceImage_.resize(newSizeX, newSizeY,-100,-100,0);

  // saves this size in case one run edit mode again
  savedSizeX_=getWindowSizeX_();
  savedSizeY_=getWindowSizeY_();
}

void DialogWindow::destroy()
{
  // save windows position
  RECT r;
  GetWindowRect(interfacehWnd_, &r);
  savedSizeX_  = r.right  - r.left ;
  savedSizeY_  = r.bottom - r.top ;
  savedX_      = r.left ;
  savedY_      = r.top  ;
  isPosStored_ = true   ;

  // reactivates the main application window BEFORE destroying the child window 
  // to avoid keyboard focus to go to another app
  if (isModal_)
    EnableWindow(interfaceParenthwnd_, TRUE);

  DestroyWindow(interfacehWnd_);
  isActive_ = false;
}

void DialogWindow::requestRedraw()
{
  // tells window that the current main window is invalidated : it will trigger a redraw
  // but it may not be immediate
  InvalidateRect(interfacehWnd_, NULL, FALSE);
}

void DialogWindow::displayToWindow ()     { }
void DialogWindow::doAction        (int)  { }
void DialogWindow::detectAction    ()     { }

int DialogWindow::getClientSizeX_()
{
  RECT r;
  GetClientRect(interfacehWnd_, &r);

  return r.right-r.left+1;
}

int DialogWindow::getClientSizeY_()
{
  RECT r;
  GetClientRect(interfacehWnd_, &r);

  return  r.bottom-r.top+1;
}

int DialogWindow::getWindowSizeX_()
{
  RECT r;
  GetWindowRect(interfacehWnd_, &r);

  return r.right-r.left+1;
}

int DialogWindow::getWindowSizeY_()
{
  RECT r;
  GetWindowRect(interfacehWnd_, &r);

  return  r.bottom-r.top+1;
}

bool DialogWindow::isActive() const
{
  return isActive_;
}