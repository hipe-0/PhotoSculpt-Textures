/*
 * FocalDialog.cpp
 *
 *  Created on: 2011/01/08
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include <Windows.h>
#include "RESOURCE.H"

INT_PTR CALLBACK callBackFocal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool askFocalDialog(HINSTANCE inst, HWND hwnd, float * focalinputoutput)
{
  int isokcancel = DialogBoxParam(inst, MAKEINTRESOURCE(IDD_FOCAL_BOX), 
            hwnd,  callBackFocal, LPARAM( focalinputoutput ) );

  if (isokcancel == IDOK )   return true;
  return false;
}


// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callBackFocal(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static float * foc ;

	switch (message)
	{
	case WM_INITDIALOG:
     {
       foc = (float *) lParam ;

       CHAR st[20]="";
       sprintf_s(st, 20, "%.1f", *foc);

       SetDlgItemText(hDlg, FOCAL_EDIT, st);

       return TRUE;
     }

	case WM_COMMAND:
		if ( LOWORD(wParam) == IDOK ) // OK was pressed
		{ 

      CHAR st[20];
      GetDlgItemText(hDlg, FOCAL_EDIT, st, 20);

      float f = 0.0f;
      int numberofitemsscanned = sscanf_s(st, "%f", &f, 20);

      if (numberofitemsscanned == 1 && f >= 20.0f && f <= 500.0f )
      {
        *foc = f;
 			  EndDialog(hDlg, LOWORD(wParam));
			  return (INT_PTR)TRUE;
      }
      else
        MessageBox(hDlg, "Wrong argument, a value between 20mm and 500mm and is expected", "Press ok to retry", MB_OK);
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
