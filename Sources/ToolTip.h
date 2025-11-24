#ifndef TOOLTIP_H_
#define TOOLTIP_H_

#include <windows.h>
#include "Mutex.h"

class ToolTip
{
public:
  ToolTip();
  void init(HWND hwnd, HINSTANCE hinst);
  void add(int action, int x1, int y1, int x2, int y2, char * txt);
  void destroy();

private:
  HWND      hwnd_ , tthwnd_;
  HINSTANCE hinst_;
  Mutex     mutex_;
};

class FakeToolTip
{
public:
  FakeToolTip() {}
  void init(HWND hwnd, HINSTANCE hinst) {}
  void add(int action, int x1, int y1, int x2, int y2, char * txt) {}
  void destroy() {}
};

#endif