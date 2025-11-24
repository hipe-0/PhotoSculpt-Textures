/*
 * ResourceManager.cpp
 *
 *  Created on: 22 juillet 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */


#include "Win32Tools.h"
#include "Includes.h"
#include <Psapi.h>

Image Win32Tools::loadImageFromHBitmap(HBITMAP hBitmap) // untested with 32 bit hbitmap !
{
  if (hBitmap == NULL)
    THROW_INTERNAL_ERROR( "Could not retrieve Bitmap (in loadImageFromHBitmap())" );

  // fills the BITMAP structure with information
  DIBSECTION dibsection;
  int value=GetObject(hBitmap, sizeof(dibsection), &dibsection);
  if (value==0) 
    THROW_INTERNAL_ERROR( "Could not retrieve Bitmap information (in loadImageFromHBitmap())" );
  if (dibsection.dsBm.bmBitsPixel!=24 &&  dibsection.dsBm.bmBitsPixel!=32)
    THROW_INTERNAL_ERROR( "Unexpected Bitmap format (in loadImageFromHBitmap())" );
  if (dibsection.dsBm.bmBits == NULL)
    THROW_INTERNAL_ERROR( "no data (in loadImageFromHBitmap())" );

  unsigned char* sourceptr = (unsigned char*) dibsection.dsBm.bmBits ;

  Image result (dibsection.dsBm.bmWidth, dibsection.dsBm.bmHeight, 1, 3);

  for (int y=dibsection.dsBm.bmHeight-1; y>=0 ; y--)
  {
    for (int x=0; x<dibsection.dsBm.bmWidth ; x++)
    {
      unsigned char blue  = sourceptr[0]; //load & 0x0000FF ;
      unsigned char green = sourceptr[1]; //(load & 0x00FF00 ) >> 8;
      unsigned char red   = sourceptr[2]; //(load & 0xFF0000 ) >> 16;

      if (dibsection.dsBm.bmBitsPixel == 24)
        sourceptr += 3;
      else if (dibsection.dsBm.bmBitsPixel == 32)
        sourceptr += 4;

      result(x,y,0,0)= red;
      result(x,y,0,1)= green;
      result(x,y,0,2)= blue;
    }

    int odd = int(sourceptr) % 4;
    if ( odd == 1 )
      sourceptr += 3 ;
    if ( odd == 2 )
      sourceptr += 2 ;
    if ( odd == 3 )
      sourceptr += 1 ;
  }

  return result;
}


Image Win32Tools::loadImageFromResource(HINSTANCE programInstanceHandle, char * resourceName)
{
  HBITMAP hBitmap = (HBITMAP) LoadImage(programInstanceHandle, resourceName, 
    IMAGE_BITMAP, 0, 0, 
    LR_CREATEDIBSECTION | LR_DEFAULTSIZE | LR_VGACOLOR);

  if (hBitmap == NULL)
    THROW_INTERNAL_ERROR( "Could not retrieve Bitmap (in loadImageFromResource())" );

  Image result = loadImageFromHBitmap(hBitmap);

  DeleteObject(hBitmap);

  return result;
}


bool Win32Tools::copyImageToDC(Image const &image, HDC hdc, unsigned char alpha)
{
  // Only standard r g b images are accepted
  if (image.dimv() != 3 || image.dimz() !=1)
    THROW_INTERNAL_ERROR( "internal error, wrong parameters using copyImageToDC" );

  BITMAPINFO bmi;

  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = image.dimx();
  bmi.bmiHeader.biHeight = -(int)image.dimy();
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 1;
  bmi.bmiHeader.biYPelsPerMeter = 1;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;

  unsigned int* data = new unsigned int[image.dimx()*image.dimy()];

  if (data==NULL)
    return false;

  unsigned int* ptrd = data;

  const unsigned char
    *data1 = image.data,
    *data2 = image.ptr(0,0,0,1),
    *data3 = image.ptr(0,0,0,2);

  for (unsigned int xy = image.dimx()*image.dimy(); xy>0; --xy)
    *(ptrd++) = (alpha<<24)    | 
                (*(data1++)*alpha/255)<<16 | 
                (*(data2++)*alpha/255)<<8  | 
                (*(data3++)*alpha/255)     ;

  int isOk = SetDIBitsToDevice(hdc,0,0,image.dimx(),image.dimy(),0,0,0,
                            image.dimy(),data,&bmi,DIB_RGB_COLORS);

  delete [] data;

  if (isOk>0)
    return true;
  else
    return false;
}

bool Win32Tools::copyImageToDC(Image const &image, HDC hdc, const Image& alphamask)
{
  // Only standard r g b images are accepted
  if (image.dimv() != 3 || image.dimz() !=1)
    THROW_INTERNAL_ERROR( "internal error, wrong parameters using copyImageToDC" );
  if (image.dimx() == 0 || image.dimy() == 0)
    THROW_INTERNAL_ERROR( "internal error, empty parameters using copyImageToDC" );
  //if (image.dimx() != alphamask.dimx() || image.dimy() != alphamask.dimy() )
    //THROW_INTERNAL_ERROR( "internal error, wrong mask using copyImageToDC" );

  BITMAPINFO bmi;

  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = image.dimx();
  bmi.bmiHeader.biHeight = -(int)image.dimy();
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32;
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = 0;
  bmi.bmiHeader.biXPelsPerMeter = 1;
  bmi.bmiHeader.biYPelsPerMeter = 1;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;

  unsigned int* data = new unsigned int[image.dimx()*image.dimy()];

  if (data==NULL)
    return false;

  unsigned int* ptrd = data;

  const unsigned char
    *data1 = image.data,
    *data2 = image.ptr(0,0,0,1),
    *data3 = image.ptr(0,0,0,2),
    *alphadata = alphamask.data;

  for (unsigned int xy = image.dimx()*image.dimy(); xy>0; --xy)
      *(ptrd++) = ((unsigned char)*(alphadata++)  << 24) | 
                    (*(data1++)*(*alphadata)/255) << 16  | 
                    (*(data2++)*(*alphadata)/255) << 8   | 
                    (*(data3++)*(*alphadata)/255)        ;

  int isOk = SetDIBitsToDevice(hdc,0,0,image.dimx(),image.dimy(),0,0,0,
                            image.dimy(),data,&bmi,DIB_RGB_COLORS);

  delete [] data;

  if (isOk>0)
    return true;
  else
    return false;
}
/*
#include "Limit.h"

bool Win32Tools::copyImageToDC(const Image  &image, const Image& alpha, HDC hdc, int x1, int y1, int x2, int y2)
{
  // Only standard r g b images are accepted
  if (image.dimv() != 3 || image.dimz() !=1)
    THROW_INTERNAL_ERROR( "internal error, wrong parameters using copyImageToDC" );
  if (image.dimx() == 0 || image.dimy() == 0)
    THROW_INTERNAL_ERROR( "internal error, empty parameters using copyImageToDC" );
  //if (image.dimx() != alpha.dimx() || image.dimy() != alpha.dimy() )
    //THROW_INTERNAL_ERROR( "internal error, alpha size and image size don't match using copyImageToDC" );

  limit(x1, 0, image.dimx()-1);
  limit(x2, 0, image.dimx()-1);
  limit(y1, 0, image.dimy()-1);
  limit(y2, 0, image.dimy()-1);

  if (x2<x1) x2=x1;
  if (y2<y1) y2=y1;

  int sizex = x2-x1+1;
  int sizey = y2-y1+1;

  BITMAPINFO bmi;

  bmi.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth         = sizex;
  bmi.bmiHeader.biHeight        = sizey;
  bmi.bmiHeader.biPlanes        = 1;
  bmi.bmiHeader.biBitCount      = 32;
  bmi.bmiHeader.biCompression   = BI_RGB;
  bmi.bmiHeader.biSizeImage     = 0;
  bmi.bmiHeader.biXPelsPerMeter = 1;
  bmi.bmiHeader.biYPelsPerMeter = 1;
  bmi.bmiHeader.biClrUsed       = 0;
  bmi.bmiHeader.biClrImportant  = 0;

  unsigned int* data = new unsigned int[sizex * sizey];

  if (data==NULL)
    return false;

  unsigned int* ptrd = data;

  for (int y = y1; y <=y2; y++)
  {
    const unsigned char
      *data1     = image.ptr(x1,y,0,0),
      *data2     = image.ptr(x1,y,0,1),
      *data3     = image.ptr(x1,y,0,2),
      *alphadata = alpha.ptr(x1,y,0,0);

    for (int x = 0; x < sizex; x++)
        *(ptrd++) = ((unsigned char)*(alphadata++)  << 24) |
                      (*(data1++)*(*alphadata)/255) << 16  |
                      (*(data2++)*(*alphadata)/255) << 8   |
                      (*(data3++)*(*alphadata)/255)        ;
  }

  int isOk = SetDIBitsToDevice(hdc,x1,y1,sizex,sizey,0,0,0,
                               sizey,data,&bmi,DIB_RGB_COLORS);

  delete [] data;

  if (isOk>0)
    return true;
  else
    return false;
}*/

// Warning you should use BOOL DeleteObject( HGDIOBJ hObject ); when bitmap no longer needed
HBITMAP Win32Tools::getHBITMAPfromImage(const Image & image, HDC currenthdc)
{

  int sizex=image.dimx();
  int sizey=image.dimy();

  HDC hDCMem = CreateCompatibleDC (currenthdc);
  
  //HBITMAP result_ = CreateBitmap(sizex, sizey, 1, 48, 0); // doesn't work

  HBITMAP result_ = CreateCompatibleBitmap(currenthdc, sizex, sizey);
  
  BITMAP b;
  int nrofbytesstored=GetObject(result_, sizeof(b), &b);
  
  SelectObject(hDCMem, result_);
  
  copyImageToDC(image,hDCMem);
  
  DeleteDC (hDCMem);

  return result_;
}


bool Win32Tools::copyHBITMAPToClipboard(HBITMAP hBitmap, HWND hwnd)
{ 
  HDC hdc = CreateCompatibleDC(0);
  
  BITMAP b;
  int nrofbytesstored=GetObject(hBitmap, sizeof(b), &b);

  if (nrofbytesstored==0) 
    return false;

  // Calculate the size of required buffer.
  DWORD BitmapImageSize = b.bmHeight * b.bmWidth *
      ( b.bmBitsPixel / 8 );


  BITMAPINFO bmi;

  bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
  bmi.bmiHeader.biWidth = b.bmWidth ;
  bmi.bmiHeader.biHeight = b.bmHeight ;
  bmi.bmiHeader.biPlanes = 1;
  bmi.bmiHeader.biBitCount = 32; //32
  bmi.bmiHeader.biCompression = BI_RGB;
  bmi.bmiHeader.biSizeImage = BitmapImageSize;
  bmi.bmiHeader.biXPelsPerMeter = 1;
  bmi.bmiHeader.biYPelsPerMeter = 1;
  bmi.bmiHeader.biClrUsed = 0;
  bmi.bmiHeader.biClrImportant = 0;
 
  // reserves a space in global memory
  GLOBALHANDLE hGlobal = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + BitmapImageSize); 

  // beginning of buffer
  BYTE * startGlobalBuffer = (BYTE *) GlobalLock (hGlobal);
  // inside buffer , location of the bitmap pixels
  BYTE * bitmapBuffer = startGlobalBuffer + sizeof(BITMAPINFOHEADER);

  // First, here copy the bitmap info header to global memory block
  CopyMemory(startGlobalBuffer,&bmi,sizeof(BITMAPINFOHEADER));

  // Second
  // copies all bitmap bits to the global mem
  int isOk=GetDIBits(hdc, hBitmap , 0 , b.bmHeight, bitmapBuffer , &bmi , DIB_RGB_COLORS);
  if (isOk==0) 
  {
    DeleteDC(hdc);
    GlobalFree(hGlobal);
    return false;
  }

  DeleteDC(hdc);

  int isOk2 = OpenClipboard(hwnd);
  if (isOk2==0) 
  {
    DeleteDC(hdc);
    GlobalFree(hGlobal);
    return false;
  }

  int isOk3 = EmptyClipboard();
  if (isOk3==0)
  {
    DeleteDC(hdc);
    GlobalFree(hGlobal);
    CloseClipboard();
    return false;
  }  

  HANDLE isOk4 = SetClipboardData(CF_DIB,hGlobal);
  if (isOk4==0)
  {
    DeleteDC(hdc);
    GlobalFree(hGlobal);
    CloseClipboard();
    return false;
  }
  GlobalUnlock(hGlobal);
  DeleteDC(hdc);  
  
  int isOk5=CloseClipboard();
  if (isOk5==0)
    return false;

  return true;
}

bool Win32Tools::copyImageToClipboard(const Image & image, HWND hwnd)
{ 
  // reads the current device context
  HDC hdc=GetDC(hwnd);

  // creates a brand new hbitmap from the image
  HBITMAP hBitmap = Win32Tools::getHBITMAPfromImage(image, hdc);

  // kills the dc, now that the hbitmap is created
  ReleaseDC(hwnd,hdc);

  if (hBitmap==0) 
    return false;

  // copies the hbitmap to the clipboard
  bool isok = Win32Tools::copyHBITMAPToClipboard ( hBitmap, hwnd);

  DeleteObject(hBitmap);

  return isok;
}

Image Win32Tools::loadImageFromClipboard(HWND hwnd)
{ 
  int isOk = OpenClipboard(hwnd);
  if (isOk == 0) return Image();
  
  HBITMAP hBitmap = (HBITMAP)GetClipboardData(CF_BITMAP);

  if (hBitmap==0) 
  {
    CloseClipboard();
    return Image();
  }

  Image result = loadImageFromHBitmap(hBitmap);

  int isOk5=CloseClipboard();
  if (isOk5==0)
    return Image();

  DeleteObject(hBitmap);

  return result;
}


UINT64 Win32Tools::getCPUID(void)
{
   DWORD dwStandard = 0; 
   DWORD dwFeature = 0; 
     
   _asm { 
        mov eax, 1 
        cpuid 
        mov dwStandard, eax 
        mov dwFeature, edx 
    }
    return( ((UINT64)(dwFeature) << 32) | ((UINT64)(dwStandard)));
}

// used to log the maximum used memory in mb
int Win32Tools::maxMemUsed_ = -1;

int Win32Tools::getMemUsedMb()
{
  HANDLE process = GetCurrentProcess();
  PROCESS_MEMORY_COUNTERS counters;
  GetProcessMemoryInfo(process,&counters,sizeof(counters));
  int totalused = int( counters.WorkingSetSize / 1024 / 1024 );
  
  if (totalused > maxMemUsed_)
    maxMemUsed_ = totalused;

  return totalused;
}

void Win32Tools::spyMemUsed()
{
  int memused = getMemUsedMb();
  if (memused > maxMemUsed_)
    maxMemUsed_ = memused;
}

void Win32Tools::clearMemUsed() // forget any older value
{
  maxMemUsed_ = getMemUsedMb();
}

int Win32Tools::getMaxMemUsedMb()
{
  if (maxMemUsed_ == -1)
    maxMemUsed_ = getMemUsedMb();

  return maxMemUsed_;
}

int Win32Tools::getMemChargePercent()
{
  MEMORYSTATUSEX statex;
  statex.dwLength = sizeof (statex);
  GlobalMemoryStatusEx (&statex);
  //int availPhys = int( statex.ullAvailPhys / 1024 / 1024 );
  //int totalPhys = int( statex.ullTotalPhys / 1024 / 1024 );
  int load      = int( statex.dwMemoryLoad );

  return load;
}

int Win32Tools::getNumberOfCores()
{
  SYSTEM_INFO sysinfo;
  GetSystemInfo( &sysinfo );

  int numCPU = sysinfo.dwNumberOfProcessors;

  return numCPU;
}