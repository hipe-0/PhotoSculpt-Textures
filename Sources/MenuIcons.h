

HRESULT AddBitmapToMenuItem(HMENU hmenu, int iItem,     BOOL fByPosition, HBITMAP hbmp);
HRESULT AddIconToMenuItem  (HMENU hmenu, int iMenuItem, BOOL fByPosition, HICON hicon, BOOL fAutoDestroy, __out_opt HBITMAP *phbmp);
HBITMAP BitmapFromIcon(HICON hIcon);
bool isCapableMenuIcons();