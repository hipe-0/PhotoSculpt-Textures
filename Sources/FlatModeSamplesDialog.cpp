/*
 * FlatModeSamplesDialog.cpp
 *
 *  Created on: 2011/01/10
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include <Windows.h>
#include "RESOURCE.H"
#include "MultiTasking.h"
#include "Security.h"

INT_PTR CALLBACK callbackFlatModeOptionsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool FlatModeOptionsDialog(HINSTANCE inst, HWND hwnd, int * numberofsamples)
{
  int isokcancel = DialogBoxParam(inst, MAKEINTRESOURCE(IDD_FLATMODESAMPLE_BOX), 
                                  hwnd,  callbackFlatModeOptionsDialog, LPARAM(numberofsamples) );

  if (isokcancel == IDOK )   return true;
  return false;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callbackFlatModeOptionsDialog(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static int* numberofsamples ;

	switch (message)
	{
	case WM_INITDIALOG:
     {
       numberofsamples = (int*) lParam ;

       CHAR st[20]="";
       sprintf_s(st, 20, "%2d", *numberofsamples);

       SetDlgItemText(hDlg, SAMPLES_EDIT, st);

       return TRUE;
     }

	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK ) // OK was pressed
		{ 
      CHAR st[20];
      GetDlgItemText(hDlg, SAMPLES_EDIT, st, 20);

      int value = 0;
      int numberofitemsscanned = sscanf_s(st, "%d", &value, 20);

      if (numberofitemsscanned == 1 && value >= 3 && value <= 40 )
      {
        *numberofsamples = value;
 			  EndDialog(hDlg, LOWORD(wParam));
			  return (INT_PTR)TRUE;
      }
      else
        MessageBox(hDlg, "Wrong argument, a value between 3 and 40 is expected", "Press ok to retry", MB_OK);
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
