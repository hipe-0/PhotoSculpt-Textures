#include "ProgressDialog.h"
#include "RESOURCE.H"
#include "Includes.h"

INT_PTR CALLBACK callBackProgress(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

HINSTANCE ProgressDialog::handleinstance_     = NULL ;
HWND      ProgressDialog::handleParentWindow_ = NULL ;
HWND      ProgressDialog::handleNewWindow_    = NULL ;
HWND      ProgressDialog::handleText_         = NULL ;
int       ProgressDialog::numberOfCalls_      = 0    ;

void ProgressDialog::init(HINSTANCE handleinstance, HWND handlewindow)
{
  if (handleinstance == NULL || handlewindow == NULL)
    THROW_INTERNAL_ERROR("Internal error, progress dialog window received wrong arguments");

  handleinstance_     = handleinstance ;
  handleParentWindow_ = handlewindow   ;
  registerDialogClass_();
}

ProgressDialog::ProgressDialog()
{
  isThisInstanceShowing_ = false;
  show();
}

ProgressDialog::ProgressDialog(bool isset)
{
  isThisInstanceShowing_ = false;
  if (isset) 
    show();
  // note: do not call constructor ProgressDialog() as doing this fires-off the destructor 
  //         and close the window right away
}

ProgressDialog::ProgressDialog(char* texttodisplay )
{
  isThisInstanceShowing_ = false;
  show();
  setText(texttodisplay);
}

ProgressDialog::~ProgressDialog()
{
  hide();
}

void ProgressDialog::show()
{
  if (handleinstance_ == NULL || handleParentWindow_ == NULL)
    THROW_INTERNAL_ERROR("Internal error, progress dialog window not initialised");

  if (!isThisInstanceShowing_)
  {
    isThisInstanceShowing_ = true;
    numberOfCalls_ ++;

    if (numberOfCalls_ == 1) // only the first call creates a window, more calls are ignored
    {
      EnableWindow(handleParentWindow_, FALSE);

      handleNewWindow_= NULL  ;

      createDialogBox_();
    }  
  }
}

void ProgressDialog::hide()
{
  if (handleinstance_ == NULL || handleParentWindow_ == NULL)
    THROW_INTERNAL_ERROR("Internal error, progress dialog window not initialised");

  if (isThisInstanceShowing_ == true)
  {
    numberOfCalls_ --;

    if (numberOfCalls_ == 0)
    {
      // the main window must be enabled BEFORE the child window is destroyed. 
      // If not another window is getting the keyboard focus
      EnableWindow (handleParentWindow_, TRUE); 
      DestroyWindow(handleNewWindow_);
      handleNewWindow_ = NULL;
      handleText_      = NULL;
    }
  }
}

bool ProgressDialog::isDisplaying()
{
  return numberOfCalls_ > 0 ;
}

LRESULT CALLBACK DialogProc( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
  static HFONT hFont;

  switch(msg)  
  {
  case WM_CREATE:
  {
    hFont = CreateFont (15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
        OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Arial"));

    if (!hFont)
      THROW_INTERNAL_ERROR("Font creation failed (in DialogProc())");

    SendMessage(hwnd, WM_SETFONT, WPARAM(hFont), TRUE);

    ProgressDialog::handleText_ = CreateWindow( TEXT("static"), TEXT("Please wait..."),
                  WS_VISIBLE | WS_CHILD ,
                  20, 5, 120, 25,
                  hwnd, (HMENU) 1, NULL, NULL);

    SendMessage(ProgressDialog::handleText_, WM_SETFONT, WPARAM(hFont), TRUE);

    HWND h2 = CreateWindow( TEXT("button"), TEXT(STR(PROGRESS_WAIT)),
                  WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                  50, 30, 80, 25,
                  hwnd, (HMENU) 1, NULL, NULL);
    SendMessage(h2, WM_SETFONT, WPARAM(hFont), TRUE);

	  break;
  }
  case WM_COMMAND:
    DestroyWindow(hwnd);
    DeleteObject(hFont);
  	break;

  case WM_CLOSE:
    DestroyWindow(hwnd);
    break;       
  }

  return (DefWindowProc(hwnd, msg, wParam, lParam));
}


void ProgressDialog::registerDialogClass_() 
{
  WNDCLASSEX wc = {0};
  wc.cbSize           = sizeof(WNDCLASSEX);
  wc.lpfnWndProc      = (WNDPROC) DialogProc;
  wc.hInstance        = handleinstance_;
  wc.hbrBackground    = GetSysColorBrush(COLOR_3DFACE);
  wc.lpszClassName    = TEXT("DialogClass");
	wc.hIcon			      = LoadIcon(handleinstance_, MAKEINTRESOURCE(IDI_SMALL));
  RegisterClassEx(&wc);
}

void ProgressDialog::createDialogBox_()
{
  RECT rect;
  BOOL isok = GetWindowRect(handleParentWindow_, &rect);
  if (!isok)
    THROW_INTERNAL_ERROR("Could not getWindowRect (in CreateDialogBox_())");

  DWORD top    = rect.top    ;
  DWORD bottom = rect.bottom ;
  DWORD left   = rect.left   ;
  DWORD right  = rect.right  ;

  const int W = 180 , H = 90;
  const int X = (rect.left + rect.right - W) /2 ;
  const int Y = (rect.top + rect.bottom - H) /2 ;
  //WS_CHILD | WS_VISIBLE | SS_OWNERDRAW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS

  handleNewWindow_ = CreateWindowEx(WS_EX_DLGMODALFRAME ,
        TEXT("DialogClass"), TEXT(STR(PROGRESS_IN_PROGRESS)), 
        WS_VISIBLE | WS_SYSMENU | WS_CAPTION,
        X, Y, W, H, 
        NULL, NULL, handleinstance_,  NULL);

  ShowWindow(handleNewWindow_, SW_SHOW);
  UpdateWindow(handleNewWindow_);
}

void ProgressDialog::setText(char* text)
{
  if (handleText_ != NULL)
    SendMessage(handleText_, WM_SETTEXT, 0, WPARAM(text));
}