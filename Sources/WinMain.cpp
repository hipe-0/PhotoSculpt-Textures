/* WinMain.cpp
 * Created on: 28.07.2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#include "stdafx.h"
#include "Image.h"
#include "PSApplication.h"
#include <process.h>
#include <Commctrl.h>
#include "Vector.h"
#include "SmartPointer.h"
#include "Includes.h"

using namespace std;

// Global variable: Global so it's always here and never get destroyed
PSApplication  MainApp               ;
ThreadId       GlobalWindowsThreadID ;

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine, int show)
{
  DEBUG_CLEAN_FILE();
  DEBUG(Winain)
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);

  MSG msg;
  HACCEL hAccelTable;

  try
  {
    InitCommonControls ();// this will turn on the xp styles, using data from the manifest 

	  MainApp.createMainInterface (hInstance, show);

    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_PHOTOSCULPT));
    
    if (strlen(lpCmdLine) > 0)
      MainApp.loadFromCommandLine();

    if (!GlobalOptions.isSoftwareMode_)
      if (!DirectXControl::testLoadDxDll())
        THROW_MESSAGE_TO_USER(DIRECTX2);
  } 
  CATCH_BLOC("Error initialising the application");

  try
  {
    DEBUG_WRITE("  Gessge"); // Boucle de messages principale :
	  
	  while (GetMessage(&msg, NULL, 0, 0))
	  {
		  if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		  {
			  TranslateMessage(&msg);
			  DispatchMessage(&msg);
		  }
	  }
  } 
  CATCH_BLOC("Error running the application");
  
  return int( msg.wParam );
}

void unusedfunction() // avoid an compiler error message due to the tmpfile function
{
  FILE* destroy = tmpfile();
  fclose(destroy);
}


