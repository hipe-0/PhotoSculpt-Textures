/*
 * PSInterfaceAbout.cpp
 *
 *  Created on: 20 10 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
 
#include "RESOURCE.H"
#include "PSApplication.h"
#include "Includes.h"


// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callBackAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();
  UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
    {
      char txt[256]="Licenced to: ", name [256]= "";
      if (getArmadilloUserName(name))
      {
        strncat(txt,name,255);
        SetDlgItemText(hDlg, ABOUT_LICENCED_TO, txt);
      }
      
		  return (INT_PTR)TRUE;
    }
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

void PSApplication::actionAbout()
{
  ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD();

  DialogBox(interface_.instanceHandle_, MAKEINTRESOURCE(IDD_ABOUTBOX), 
            interface_.windowsHandle_,  callBackAbout);
}
