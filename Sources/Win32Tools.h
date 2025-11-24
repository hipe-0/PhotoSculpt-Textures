/*
 * ResourceManager.h
 *
 *  Created on: 22 juillet 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once
#include "Image.h"

class Win32Tools
{
public:
  static Image   loadImageFromResource  (HINSTANCE programInstanceHandle, 
                                         char * resourceName);
  static Image   loadImageFromHBitmap   (HBITMAP hBitmap);
  static bool    copyImageToDC          (Image const &image,  HDC hdc, unsigned char alpha=255);
  static bool    copyImageToDC          (Image const &image, HDC hdc, const Image& alphamask);
  static bool    copyImageToDC          (const Image  &image, const Image& alpha, HDC hdc, 
                                         int x1, int y1, int x2, int y2);
  static bool    copyImageToClipboard   (const Image & image, HWND hwnd);
  static bool    copyHBITMAPToClipboard (HBITMAP hBitmap,     HWND hwnd);
  static Image   loadImageFromClipboard (HWND hwnd);
  static HBITMAP getHBITMAPfromImage    (const Image & image, HDC currenthdc);
  static int     getMemUsedMb           ();
  static void    spyMemUsed             ();
  static void    clearMemUsed           ();
  static int     getMaxMemUsedMb        ();

  static int     getMemChargePercent    ();
  static UINT64  getCPUID               ();
  static int     getNumberOfCores       ();

private:
  static int maxMemUsed_;
};


