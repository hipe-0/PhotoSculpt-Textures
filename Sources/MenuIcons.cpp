
//********************************************************************
#define STRICT_TYPED_ITEMIDS    // In case you use IDList, you want this on for better type safety.
#include <windows.h>
#include <windowsx.h>           // For WM_COMMAND handling macros
#include <shlobj.h>             // For shell
#include <shlwapi.h>            // QISearch, easy way to implement QI
#include <commctrl.h>
#include <uxtheme.h>
#include "resource.h"

#pragma comment(lib, "shlwapi.lib") // Default link libs do not include this.
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")

// Set up common controls v6 the easy way.
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

HINSTANCE g_hinst = 0;

// AddIconToMenuItem and supporting functions.
// Note: BufferedPaintInit/BufferedPaintUnInit should be called to
// improve performance.
// In this sample they are called in _OnInitDlg/_OnDestroyDlg.
// In a full application you would call these during WM_NCCREATE/WM_NCDESTROY.

typedef DWORD ARGB;

void InitBitmapInfo(__out_bcount(cbInfo) BITMAPINFO *pbmi, ULONG cbInfo, LONG cx, LONG cy, WORD bpp)
{
    ZeroMemory(pbmi, cbInfo);
    pbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbmi->bmiHeader.biPlanes = 1;
    pbmi->bmiHeader.biCompression = BI_RGB;

    pbmi->bmiHeader.biWidth = cx;
    pbmi->bmiHeader.biHeight = cy;
    pbmi->bmiHeader.biBitCount = bpp;
}

HRESULT Create32BitHBITMAP(HDC hdc, const SIZE *psize, __deref_opt_out void **ppvBits,
                           __out HBITMAP* phBmp)
{
    *phBmp = NULL;

    BITMAPINFO bmi;
    InitBitmapInfo(&bmi, sizeof(bmi), psize->cx, psize->cy, 32);

    HDC hdcUsed = hdc ? hdc : GetDC(NULL);
    if (hdcUsed)
    {
        *phBmp = CreateDIBSection(hdcUsed, &bmi, DIB_RGB_COLORS, ppvBits, NULL, 0);
        if (hdc != hdcUsed)
        {
            ReleaseDC(NULL, hdcUsed);
        }
    }
    return (NULL == *phBmp) ? E_OUTOFMEMORY : S_OK;
}

HRESULT AddBitmapToMenuItem(HMENU hmenu, int iItem, BOOL fByPosition, HBITMAP hbmp)
{
    HRESULT hr = E_FAIL;

    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_BITMAP;
    mii.hbmpItem = hbmp;
    if (SetMenuItemInfo(hmenu, iItem, fByPosition, &mii))
    {
        hr = S_OK;
    }

    return hr;
}

HRESULT ConvertToPARGB32(HDC hdc, __inout ARGB *pargb, HBITMAP hbmp, SIZE& sizImage, int cxRow)
{
    BITMAPINFO bmi;
    InitBitmapInfo(&bmi, sizeof(bmi), sizImage.cx, sizImage.cy, 32);

    HRESULT hr = E_OUTOFMEMORY;
    HANDLE hHeap = GetProcessHeap();
    void *pvBits = HeapAlloc(hHeap, 0, bmi.bmiHeader.biWidth * 4 * bmi.bmiHeader.biHeight);
    if (pvBits)
    {
        hr = E_UNEXPECTED;
        if (GetDIBits(hdc, hbmp, 0, bmi.bmiHeader.biHeight, pvBits, &bmi, DIB_RGB_COLORS) == bmi.bmiHeader.biHeight)
        {
            ULONG cxDelta = cxRow - bmi.bmiHeader.biWidth;
            ARGB *pargbMask = static_cast<ARGB *>(pvBits);

            for (ULONG y = bmi.bmiHeader.biHeight; y; --y)
            {
                for (ULONG x = bmi.bmiHeader.biWidth; x; --x)
                {
                    if (*pargbMask++)
                    {
                        // transparent pixel
                        *pargb++ = 0;
                    }
                    else
                    {
                        // opaque pixel
                        *pargb++ |= 0xFF000000;
                    }
                }

                pargb += cxDelta;
            }

            hr = S_OK;
        }

        HeapFree(hHeap, 0, pvBits);
    }

    return hr;
}

bool HasAlpha(__in ARGB *pargb, SIZE& sizImage, int cxRow)
{
    ULONG cxDelta = cxRow - sizImage.cx;
    for (ULONG y = sizImage.cy; y; --y)
    {
        for (ULONG x = sizImage.cx; x; --x)
        {
            if (*pargb++ & 0xFF000000)
            {
                return true;
            }
        }

        pargb += cxDelta;
    }

    return false;
}

HRESULT ConvertBufferToPARGB32(HPAINTBUFFER hPaintBuffer, HDC hdc, HICON hicon, SIZE& sizIcon)
{
    RGBQUAD *prgbQuad;
    int cxRow;
    HRESULT hr = GetBufferedPaintBits(hPaintBuffer, &prgbQuad, &cxRow);
    if (SUCCEEDED(hr))
    {
        ARGB *pargb = reinterpret_cast<ARGB *>(prgbQuad);
        if (!HasAlpha(pargb, sizIcon, cxRow))
        {
            ICONINFO info;
            if (GetIconInfo(hicon, &info))
            {
                if (info.hbmMask)
                {
                    hr = ConvertToPARGB32(hdc, pargb, info.hbmMask, sizIcon, cxRow);
                }

                DeleteObject(info.hbmColor);
                DeleteObject(info.hbmMask);
            }
        }
    }

    return hr;
}

HRESULT AddIconToMenuItem(HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon, BOOL fAutoDestroy, __out_opt HBITMAP *phbmp)
{
    HRESULT hr = E_OUTOFMEMORY;
    HBITMAP hbmp = NULL;

    SIZE sizIcon;
    sizIcon.cx = GetSystemMetrics(SM_CXSMICON);
    sizIcon.cy = GetSystemMetrics(SM_CYSMICON);

    RECT rcIcon;
    SetRect(&rcIcon, 0, 0, sizIcon.cx, sizIcon.cy);

    HDC hdcDest = CreateCompatibleDC(NULL);
    if (hdcDest)
    {
        hr = Create32BitHBITMAP(hdcDest, &sizIcon, NULL, &hbmp);
        if (SUCCEEDED(hr))
        {
            hr = E_FAIL;

            HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcDest, hbmp);
            if (hbmpOld)
            {
                BLENDFUNCTION bfAlpha = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
                BP_PAINTPARAMS paintParams = {0};
                paintParams.cbSize = sizeof(paintParams);
                paintParams.dwFlags = BPPF_ERASE;
                paintParams.pBlendFunction = &bfAlpha;

                HDC hdcBuffer;
                HPAINTBUFFER hPaintBuffer = BeginBufferedPaint(hdcDest, &rcIcon, BPBF_DIB, &paintParams, &hdcBuffer);
                if (hPaintBuffer)
                {
                    if (DrawIconEx(hdcBuffer, 0, 0, hicon, sizIcon.cx, sizIcon.cy, 0, NULL, DI_NORMAL))
                    {
                        // If icon did not have an alpha channel, we need to convert buffer to PARGB.
                        hr = ConvertBufferToPARGB32(hPaintBuffer, hdcDest, hicon, sizIcon);
                    }

                    // This will write the buffer contents to the
// destination bitmap.
                    EndBufferedPaint(hPaintBuffer, TRUE);
                }

                SelectObject(hdcDest, hbmpOld);
            }
        }

        DeleteDC(hdcDest);
    }

    if (SUCCEEDED(hr))
    {
        hr = AddBitmapToMenuItem(hmenu, iMenuItem, fByPosition, hbmp);
    }

    if (FAILED(hr))
    {
        DeleteObject(hbmp);
        hbmp = NULL;
    }

    if (fAutoDestroy)
    {
        DestroyIcon(hicon);
    }

    if (phbmp)
    {
        *phbmp = hbmp;
    }

    return hr;
}


HBITMAP BitmapFromIcon(HICON hIcon)
{
   HDC hDC = CreateCompatibleDC(NULL);
   HBITMAP hBitmap = CreateCompatibleBitmap(hDC, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON));
   HBITMAP hOldBitmap = (HBITMAP)SelectObject(hDC, hBitmap);
   DrawIcon(hDC, 0, 0, hIcon);
   SelectObject(hDC, hOldBitmap);
   DeleteDC(hDC);

   DIBSECTION bitmap;
   int value=GetObject(hBitmap, sizeof(bitmap), &bitmap);

   return hBitmap;
}

bool isCapableMenuIcons()
{
  HINSTANCE hDLL  = LoadLibrary("UxTheme.dll");

  if(hDLL != 0)
  {
    FARPROC function =  ::GetProcAddress(hDLL,"GetBufferedPaintBits");

    FreeLibrary(hDLL);

    if (function != 0)
      return true;
    else
      return false;
  }
  else 
    return false;
}