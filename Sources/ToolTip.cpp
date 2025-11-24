#include "ToolTip.h"
#include <Commctrl.h>
#include "Includes.h"

ToolTip::ToolTip()
{
  hwnd_  = NULL ;  hinst_ = NULL ; tthwnd_ = NULL ;
}

void ToolTip::init(HWND hwnd, HINSTANCE hinst)
{
  MUTEX;
  DEBUG("TooTpnit");

  if (tthwnd_ != NULL)
  {
    DestroyWindow(tthwnd_);
    tthwnd_ = NULL;
  }

  hinst_ = hinst ;
  hwnd_  = hwnd  ;
  tthwnd_  = CreateWindow(TOOLTIPS_CLASS, NULL,
                              WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP, // | TTS_BALLOON
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              CW_USEDEFAULT, CW_USEDEFAULT,
                              hwnd_, NULL, hinst_, NULL);
  if (tthwnd_ == NULL)
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR("tool tip component creation failed (in ToolTip::init())");
  }

  MUTEXRELEASE;
}

void ToolTip::add(int action, int x1, int y1, int x2, int y2, char * txt)
{
  MUTEX;

  if (tthwnd_ == NULL)
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR("tool tip component not initialised (in addToolTip())");
  }

  TOOLINFO ti ;
	// Pour utiliser la structure TOOLINFO, il faut indiquer la taille de la structure,
	ZeroMemory(&ti, sizeof(ti));
	ti.cbSize = sizeof(ti);
  ti.uFlags = TTF_SUBCLASS;

	// l'ID du bouton associé
  ti.uId    = (UINT_PTR) action;
  ti.hwnd   = hwnd_  ;
  ti.hinst  = hinst_ ;

  // first init of window
  ti.rect.left   = x1 ;
  ti.rect.top    = y1 ;
  ti.rect.right  = x2 ;
  ti.rect.bottom = y2 ;

  ti.lpszText = txt;

  LRESULT isok1 = SendMessage( tthwnd_, TTM_ADDTOOL, 0, (LPARAM)&ti );

  MUTEXRELEASE;
}


void ToolTip::destroy()
{
  MUTEX;

  if (tthwnd_ != NULL)
  {
    DestroyWindow(tthwnd_);
    tthwnd_ = NULL;
  }

  MUTEXRELEASE;
}

