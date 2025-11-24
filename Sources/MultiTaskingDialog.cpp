/*
 * MultiTaskingDialog.cpp
 *
 *  Created on: 2011/01/10
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include <Windows.h>
#include "RESOURCE.H"

#include "MultiTasking.h"

INT_PTR CALLBACK callbackMultitaskingOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool MultiTaskingOptionsDialog(HINSTANCE inst, HWND hwnd, int * numberofcores)
{
  int isokcancel = DialogBoxParam(inst, MAKEINTRESOURCE(IDD_MULTITHREAD_BOX), 
                                  hwnd,  callbackMultitaskingOptions, LPARAM(numberofcores) );

  if (isokcancel == IDOK )   return true;
  return false;
}


// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callbackMultitaskingOptions(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int* numberofcores ;

	switch (message)
	{
	case WM_INITDIALOG:
     {
       numberofcores = (int*) lParam ;

       CHAR st[20]="";
       sprintf_s(st, 20, "%2d", *numberofcores);

       SetDlgItemText(hDlg, CORES_EDIT, st);

       return TRUE;
     }

	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK ) // OK was pressed
		{ 
      CHAR st[20];
      GetDlgItemText(hDlg, CORES_EDIT, st, 20);

      int value = 0;
      int numberofitemsscanned = sscanf_s(st, "%d", &value, 20);

      if (numberofitemsscanned == 1 && value >= 1 && value <= 10 )
      {
        *numberofcores = value;
 			  EndDialog(hDlg, LOWORD(wParam));
			  return (INT_PTR)TRUE;
      }
      else
        MessageBox(hDlg, "Wrong argument, a value between 1 and 10 is expected", "Press ok to retry", MB_OK);
		}
    else if ( LOWORD(wParam) == IDCANCEL )
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
