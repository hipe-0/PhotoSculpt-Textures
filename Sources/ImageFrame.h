#pragma once

#include "Image.h"

class ImageFrame
{
public:
  ImageFrame(void);
  void init(HINSTANCE h, char * resourcename, int size);
  void draw(Image &i, int x1, int y1, int x2, int y2);
  void drawAtPosAndSize(Image &i, int posx, int posy, int sizex, int sizey);

private:
  PImage cornerLU_ ,  cornerLD_ ,  cornerRU_ ,  cornerRD_ ;
  PImage borderLU_ ,  borderLD_ ,  borderRU_ ,  borderRD_ ;
  PImage borderUL_ ,  borderUR_ ,  borderDL_ ,  borderDR_ ;
  PImage up_ , down_ , left_ , right_ ;
  int size_;
};
