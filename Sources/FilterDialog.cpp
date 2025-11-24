/*
 * FilterDialog.cpp
 *
 *  Created on: 2011/01/08
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include <Windows.h>
#include "RESOURCE.H"

INT_PTR CALLBACK callBackBlurRadius(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

bool askBlurRadiusDialog(HINSTANCE inst, HWND hwnd, float * blurradiusinputoutput)
{
  int isokcancel = DialogBoxParam(inst, MAKEINTRESOURCE(IDD_BLUR_BOX), 
            hwnd,  callBackBlurRadius, LPARAM( blurradiusinputoutput ) );

  if (isokcancel == IDOK )   return true;
  return false;
}


// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK callBackBlurRadius(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static float * foc ;

	switch (message)
	{
	case WM_INITDIALOG:
     {
       foc = (float *) lParam ;

       CHAR st[20]="";
       sprintf_s(st, 20, "%.2f", *foc);

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

      if (numberofitemsscanned == 1 && f >= 0.2f && f <= 2.0f )
      {
        *foc = f;
 			  EndDialog(hDlg, LOWORD(wParam));
			  return (INT_PTR)TRUE;
      }
      else
        MessageBox(hDlg, "Wrong argument, a value between 0.2 and 2.0 is expected", "Press ok to retry", MB_OK);
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
