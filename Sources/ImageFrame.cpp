#include "ImageFrame.h"
#include "Includes.h"
#include "Win32Tools.h"
#include "ImageTools.h"

ImageFrame::ImageFrame(void)
{
}

Image crop8( const Image & i , int pixelsize, int squaresize, int nr)
{
  int count = 0;

  for (int y = 0; y < squaresize ; y++)
    for (int x = 0; x < squaresize ; x++)
    {
      if (count == nr)
        return i. get_crop (x*pixelsize, y*pixelsize, 
                        x*pixelsize+pixelsize-1, 
                        y*pixelsize+pixelsize-1);
      count++;
    }

  THROW_INTERNAL_ERROR("Wrong input parameter value (in crop48)");

  return i;
}

void ImageFrame::init(HINSTANCE h, char * resourcename, int size)
{
  Image i = Win32Tools::loadImageFromResource(h, resourcename);

  size_ = size;
  cornerLU_ = new Image(crop8 ( i,size_,4,   0  ));
  cornerLD_ = new Image(crop8 ( i,size_,4,   12 ));
  cornerRU_ = new Image(crop8 ( i,size_,4,   3  ));
  cornerRD_ = new Image(crop8 ( i,size_,4,   15 ));

  borderLU_ = new Image(crop8 ( i,size_,4,   4  ));
  borderLD_ = new Image(crop8 ( i,size_,4,   8  ));
  borderRU_ = new Image(crop8 ( i,size_,4,   7  ));
  borderRD_ = new Image(crop8 ( i,size_,4,   11 ));
  borderUL_ = new Image(crop8 ( i,size_,4,   1  ));
  borderUR_ = new Image(crop8 ( i,size_,4,   2  ));
  borderDL_ = new Image(crop8 ( i,size_,4,   13 ));
  borderDR_ = new Image(crop8 ( i,size_,4,   14 ));

  up_    = new Image(i.get_crop(size_*2, 0,       size_*2,   size_-1  ));
  down_  = new Image(i.get_crop(size_*2, size_*3, size_*2,   size_*4-1));
  left_  = new Image(i.get_crop(0,       size_*2, size_-1,   size_*2  ));
  right_ = new Image(i.get_crop(size_*3, size_*2, size_*4-1, size_*2  ));
}

void ImageFrame::draw(Image &i, int x1, int y1, int x2, int y2)
{
  if (i.is_empty())   return;
  if (x2-x1<size_*2 || y2-y1<size_*2) return;

  // draws corners
  ImageTools::drawImageOverlay(i, *cornerLU_, x1-size_,   y1-size_);
  ImageTools::drawImageOverlay(i, *cornerRU_, x2+1,       y1-size_);
  ImageTools::drawImageOverlay(i, *cornerLD_, x1-size_,   y2+1);
  ImageTools::drawImageOverlay(i, *cornerRD_, x2+1,       y2+1);

  // draw elements closest to corners
  ImageTools::drawImageOverlay(i, *borderLU_, x1-size_,   y1);
  ImageTools::drawImageOverlay(i, *borderRU_, x2+1,       y1);
  ImageTools::drawImageOverlay(i, *borderLD_, x1-size_,   y2+1-size_);
  ImageTools::drawImageOverlay(i, *borderRD_, x2+1,       y2+1-size_);

  ImageTools::drawImageOverlay(i, *borderUL_, x1,         y1-size_);
  ImageTools::drawImageOverlay(i, *borderUR_, x2+1-size_, y1-size_);
  ImageTools::drawImageOverlay(i, *borderDL_, x1,         y2+1);
  ImageTools::drawImageOverlay(i, *borderDR_, x2+1-size_, y2+1);

  // draw top and bottom horizontal bar
  if (x1+size_ < x2-size_)
    for (int x = x1+size_; x <= x2-size_ ; x++)
    {
      ImageTools::drawImageOverlay(i, *up_ ,   x, y1-size_);
      ImageTools::drawImageOverlay(i, *down_ , x, y2+1);
    }

  // draw left and right vertical bar
  if (y1+size_ < y2-size_)
    for (int y = y1+size_; y <= y2-size_ ; y++)
    {
      ImageTools::drawImageOverlay(i, *left_  , x1-size_, y);
      ImageTools::drawImageOverlay(i, *right_ , x2+1,     y);
    }
}

void ImageFrame::drawAtPosAndSize(Image &i, int posx, int posy, int sizex, int sizey)
{
  draw(i, posx, posy, posx+sizex-1, posy+sizey-1);
}
