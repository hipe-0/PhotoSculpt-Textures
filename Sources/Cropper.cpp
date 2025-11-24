/*
 * Cropper.cpp
 *
 *  Created on: 29 sept 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "Cropper.h"
#include "limit.h"
#include "Includes.h"
#include "Color.h"
#include "Square.h"

Cropper::Cropper()
{
  cancel();
}

void Cropper::cancel()
{
  x1_ = y1_ = 0.0f;
  x2_ = y2_ = 1.0f;
  x3_ = y3_ = x4_ = y4_ = -1.0f;

  isSquare_      = false;
  isMode2n_      = false;
  isSeamless_    = false;
  isPerspective_ = false;
  frameSize_     = INITIAL_CROP_BLEND_FRAME ;
}

IntMap Cropper::crop_(const IntMap &m, int x1, int y1, int x2, int y2)
{
  limit (x1, 0, m.getSizeX()-1);  limit (x2, 0, m.getSizeX()-1);
  limit (y1, 0, m.getSizeY()-1);  limit (y2, 0, m.getSizeY()-1);

  int sizex = x2-x1+1;  int sizey = y2-y1+1;

  if (x2<x1) sizex=1;
  if (y2<y1) sizey=1;

  IntMap v(sizex,sizey);

  for   (int y=0; y<sizey; y++)
    for (int x=0; x<sizex; x++)
      v(x,y) = m(x+x1, y+y1);

  return v;
}

float distance(int x1, int y1, int x2, int y2)
{
  return sqrtf( float ((square(x2-x1)+square(y2-y1))));
}
float distanceF(float x1, float y1, float x2, float y2)
{
  return sqrtf( float ((square(x2-x1)+square(y2-y1))));
}


IntMap Cropper::crop_(const IntMap &m, int x1, int y1, int x2, int y2,
                        int x3, int y3, int x4, int y4, bool issquare)
{
  limit (x1, 0, m.getSizeX()-1);  limit (y1, 0, m.getSizeY()-1);  
  limit (x2, 0, m.getSizeX()-1);  limit (y2, 0, m.getSizeY()-1);
  limit (x3, 0, m.getSizeX()-1);  limit (y3, 0, m.getSizeY()-1);  
  limit (x4, 0, m.getSizeX()-1);  limit (y4, 0, m.getSizeY()-1);

  float dx1 = distance (x1,y1, x2,y2);
  float dx2 = distance (x3,y3, x4,y4);
  float dy1 = distance (x1,y1, x3,y3);
  float dy2 = distance (x2,y2, x4,y4);
  int sizex = int(round((dx1+dx2)/2.0f));
  int sizey = int(round((dy1+dy2)/2.0f));

  if (issquare)
    if (sizex<sizey)
      sizey = sizex;
    else 
      sizex = sizey;

  IntMap v(sizex,sizey);

  for   (int y=0; y<sizey; y++)
    for (int x=0; x<sizex; x++)
    {
      float fx = float(x) / float (sizex-1);
      float fy = float(y) / float (sizey-1);

      float readx = (x1 * (1.0f - fx) + x2 * fx) * (1.0f - fy) + (x3 * (1.0f - fx) + x4 * fx) * fy ;
      float ready = (y1 * (1.0f - fy) + y3 * fy) * (1.0f - fx) + (y2 * (1.0f - fy) + y4 * fy) * fx ;

      limit (readx, 0.0f, float(m.getSizeX()-1));
      limit (ready, 0.0f, float(m.getSizeY()-1));

      // linear interpolation is computed
      int oxround=int(readx);
      int oyround=int(ready);

      //on interpole le tableau de correspondances
      // those values are between 0 and 1
      // they correspond to the position between the square of 4 values
      float subx=readx-float(oxround);
      float suby=ready-float(oyround);

      // gets all the 4 corner values of the square on the old map that we're pointing at
      // 00=upper left corner, 01=upper right corner, 10= lower left, 11= lower right
      int x00 = m(oxround  ,oyround  );
      int x01 = x00, x10 = x00, x11 = x00;
      if (oxround+1 < m.getSizeX())
        x01 = m( oxround+1, oyround  );

      if (oyround+1 < m.getSizeY())
        x10 = m( oxround  , oyround+1 );

      if ( oxround+1 < m.getSizeX() && oyround+1 < m.getSizeY() )
        x11 = m( oxround+1, oyround+1 );

      // Here are the interpolated values
      float dispx= (x00*(1.0f-subx)+x01*subx)*(1.0f-suby) + (x10*(1.0f-subx)+x11*subx)*suby;

      v(x,y) = round(dispx);
    }

  return v;
}

Image Cropper::crop_(const Image &i, int x1, int y1, int x2, int y2)
{
  limit (x1, 0, i.dimx()-1);
  limit (x2, 0, i.dimx()-1);
  limit (y1, 0, i.dimy()-1);
  limit (y2, 0, i.dimy()-1);

  int sizex = x2-x1+1;
  int sizey = y2-y1+1;

  if (x2<x1) sizex=1;
  if (y2<y1) sizey=1;

  Image crop = i.get_crop(x1,y1,x2,y2);

  return crop;
}

Image Cropper::crop_(const Image &i, int x1, int y1, int x2, int y2, 
                     int x3, int y3, int x4, int y4, bool issquare)
{
  int sourcesizex = i.dimx();
  int sourcesizey = i.dimy();

  limit (x1, 0, i.dimx()-1);  limit (y1, 0, i.dimy()-1);
  limit (x2, 0, i.dimx()-1);  limit (y2, 0, i.dimy()-1);
  limit (x3, 0, i.dimx()-1);  limit (y3, 0, i.dimy()-1);
  limit (x4, 0, i.dimx()-1);  limit (y4, 0, i.dimy()-1);

  float dx1 = distance (x1,y1, x2,y2);
  float dx2 = distance (x3,y3, x4,y4);
  float dy1 = distance (x1,y1, x3,y3);
  float dy2 = distance (x2,y2, x4,y4);
  int sizex = int(round((dx1+dx2)/2.0f));
  int sizey = int(round((dy1+dy2)/2.0f));

  if (issquare)
    if (sizex<sizey)
      sizey = sizex;
    else
      sizex = sizey;

  Image croppedimage(sizex,sizey,1,3);

  for (int y=0; y<sizey; y++)
    for (int x=0; x<sizex; x++)
    {
      float fx = float(x) / float (sizex-1); // goes from 0 to 1
      float fy = float(y) / float (sizey-1); // goes from 0 to 1

      float readx = (x1 * (1.0f - fx) + x2 * fx) * (1.0f - fy) + (x3 * (1.0f - fx) + x4 * fx) * fy ;
      float ready = (y1 * (1.0f - fy) + y3 * fy) * (1.0f - fx) + (y2 * (1.0f - fy) + y4 * fy) * fx ;
  
      limit (readx, 0.0f, float(sourcesizex-1));
      limit (ready, 0.0f, float(sourcesizey-1));

      // linear interpolation is computed
      int oxround = int(readx);
      int oyround = int(ready);

      //on interpole le tableau de correspondances
      // those values are between 0 and 1
      // they correspond to the position between the square of 4 values
      float subx = readx - float(oxround);
      float suby = ready - float(oyround);

      // gets all the 4 corner values of the square on the old map that we're pointing at
      // 00=upper left corner, 01=upper right corner, 10= lower left, 11= lower right
      Color x00=getPixel(i, oxround  , oyround  );

      Color x01(x00), x10(x00), x11(x00);

      if (oxround+1 < i.dimx())
        x01=getPixel(i, oxround+1, oyround  );

      if (oyround+1 < i.dimy())
        x10=getPixel(i, oxround  , oyround+1);

      if ( oxround+1 < i.dimx() && oyround+1 < i.dimy() )
        x11=getPixel(i, oxround+1, oyround+1);
      

      // Here are the interpolated values
      float r = (float(x00.r)*(1.0f-subx)+float(x01.r)*subx)*(1.0f-suby) + (float(x10.r)*(1.0f-subx)+float(x11.r)*subx)*suby;
      float g = (float(x00.g)*(1.0f-subx)+float(x01.g)*subx)*(1.0f-suby) + (float(x10.g)*(1.0f-subx)+float(x11.g)*subx)*suby;
      float b = (float(x00.b)*(1.0f-subx)+float(x01.b)*subx)*(1.0f-suby) + (float(x10.b)*(1.0f-subx)+float(x11.b)*subx)*suby;

      limit( r, 0.0f, 255.0f);      limit( g, 0.0f, 255.0f);      limit( b, 0.0f, 255.0f);
      setPixel(croppedimage, x,y, Color((unsigned char) r,(unsigned char) g, (unsigned char)b));
      //setPixel(croppedimage, x,y, getPixel(i, oxround  , oyround  ));
    }

  return croppedimage;
}
void Cropper::defineArea(float x1, float y1, float x2, float y2)
{
  x1_ = x1;  y1_ = y1;  x2_ = x2;  y2_ = y2;

  limit (x1_, 0.0f, 1.0f);  limit (y1_, 0.0f, 1.0f);
  limit (x2_, 0.0f, 1.0f);  limit (y2_, 0.0f, 1.0f);

  if (x1>x2) x1=x2;
  if (y1>y2) y1=y2;
}

int rnd(float x) 
{ 
  return int(round(x)); 
}

int reduce2N_(int size)
{
  if (size<=0) 
    return 0;
  if (size>=2097152)
    THROW_INTERNAL_ERROR( "Size is too big (in reduce2N())" );

  const int NB=22;
  int sizes[NB]={1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,
                 16384,32768,65536,131072,262144,524288,1048576,2097152};
  for (int i=0; i<NB-1;i++)
    if (size>=sizes[i] && size<sizes[i+1])
      return sizes[i];

  return -1;
}

Image Cropper::reduce2N(const Image& i)
{
  int sizex = reduce2N_(i.dimx());
  int sizey = reduce2N_(i.dimy());

  return i.get_resize(sizex, sizey);
}

IntMap Cropper::reduce2N(const IntMap& d)
{
  int sizex = reduce2N_(d.getSizeX());
  int sizey = reduce2N_(d.getSizeY());

  return d.getResizeAndMultiply(sizex, sizey);
}

IntMap Cropper::crop(const IntMap &m)
{
  if (isCrop())
  {
    int sx =  m.getSizeX();
    int sy =  m.getSizeY();

    int x1=rnd(x1_ * float(sx));  int y1=rnd(y1_ * float(sy));    
    int x2=rnd(x2_ * float(sx));  int y2=rnd(y2_ * float(sy));
    int x3=rnd(x3_ * float(sx));  int y3=rnd(y3_ * float(sy));    
    int x4=rnd(x4_ * float(sx));  int y4=rnd(y4_ * float(sy));

    limit (x1, 0, sx-1);  limit (y1, 0, sy-1);    
    limit (x2, 0, sx-1);  limit (y2, 0, sy-1);
    limit (x3, 0, sx-1);  limit (y3, 0, sy-1);    
    limit (x4, 0, sx-1);  limit (y4, 0, sy-1);

    if (isSquare_ && !isPerspective())
    {
      // Make the coordinates square
      int sizeSquarex = x2-x1+1  , sizeSquarey = y2-y1+1;
 
      int squaresize = sizeSquarex; if (sizeSquarey<squaresize) squaresize=sizeSquarey;
      
      x1=(x1+x2-squaresize)/2;
      x2=x1+squaresize-1;
      y1=(y1+y2-squaresize)/2;
      y2=y1+squaresize-1;
    }

    IntMap c;
    if (!isPerspective())
      c = crop_ (m, x1,y1,x2,y2 );
    else
      c = crop_ (m, x1,y1,x2,y2,x3,y3,x4,y4, isSquare_ );

    return c;
  }
  else
    THROW_INTERNAL_ERROR( "Crop Function Not Initialized" );
}

Image Cropper::crop(const Image &i)
{
  if (isCrop())
  {
    int sx =  i.dimx();
    int sy =  i.dimy();

    int x1=rnd(x1_ * float(sx));    int y1=rnd(y1_ * float(sy));
    int x2=rnd(x2_ * float(sx));    int y2=rnd(y2_ * float(sy));
    int x3=rnd(x3_ * float(sx));    int y3=rnd(y3_ * float(sy));
    int x4=rnd(x4_ * float(sx));    int y4=rnd(y4_ * float(sy));
    limit (x1, 0, sx-1);        limit (y1, 0, sy-1);    
    limit (x2, 0, sx-1);        limit (y2, 0, sy-1);
    limit (x3, 0, sx-1);        limit (y3, 0, sy-1);    
    limit (x4, 0, sx-1);        limit (y4, 0, sy-1);

    if (isSquare_ && !isPerspective())
    {
      // Make the coordinates square
      int sizeSquarex = x2-x1+1  , sizeSquarey = y2-y1+1;
 
      int squaresize = sizeSquarex; if (sizeSquarey<squaresize) squaresize=sizeSquarey;
      
      x1=(x1+x2-squaresize)/2;
      x2=x1+squaresize-1;
      y1=(y1+y2-squaresize)/2;
      y2=y1+squaresize-1;
    }

    Image c;

    if (!isPerspective())
      c = crop_ (i, x1,y1,x2,y2);
    else
      c = crop_ (i, x1,y1,x2,y2,x3,y3,x4,y4, isSquare_);

    return c;
  }
  else
    THROW_INTERNAL_ERROR( "Crop Function Not Initialized" );
}

bool Cropper::isSquare  () const
{
  return isSquare_;
}

void Cropper::setSquareRectify (bool set, int realSizeOfAreaX, int realSizeOfAreaY)
{
  isSquare_ = set;

  if (!isPerspective_ && isSquare_)
     makeSquare ( float(realSizeOfAreaX), float(realSizeOfAreaY), 0 );
}

bool Cropper::is2N  () const
{
  return isMode2n_;
}

void Cropper::set2N (bool set)
{
  isMode2n_ = set;
}

bool Cropper::isSeamless () const
{
  return isSeamless_;
}

void Cropper::setSeamless (bool set)
{
  isSeamless_ = set;
}

bool Cropper::isPerspective () const
{
  return isPerspective_;
}

void Cropper::setPerspective (bool set)
{
  DEBUG(setPer);
  isPerspective_ = set;
}

static void swap(float& a, float& b) { float c=a; a=b ; b=c; }
#ifndef OBFUSCATED_VARIABLES_ARE_CLEAR
void swap(ofloat& a, ofloat& b) { float c=a; a=b ; b=c; }
#endif

void Cropper::correct()
{
  if (isCrop() )
  {
    if (!isPerspective_)
    {
      if (x1_ > x2_) swap (x1_, x2_); // reorder the points
      if (y1_ > y2_) swap (y1_, y2_);
    }

    // limit all points to the square between 0,0  and 1,1
    limit (x1_, 0.0f, 1.0f);
    limit (y1_, 0.0f, 1.0f);
    limit (x2_, 0.0f, 1.0f);
    limit (y2_, 0.0f, 1.0f);
  }
}

void Cropper::makeSquare(float realSizeOfAreaX, float realSizeOfAreaY, int method)
{
  float sizex = x2_ - x1_ ;
  float sizey = y2_ - y1_ ;

  bool invertx = false ;
  bool inverty = false ;

  float sx = sizex * realSizeOfAreaX ;
  float sy = sizey * realSizeOfAreaY ;

  if (x1_ > x2_) invertx = true ;
  if (y1_ > y2_) inverty = true ;

  switch (method)
  {
    case HANDLE_CORNER_NOT_DETECTED:
      if (sx<sy)
      {
        float realsize = sx / realSizeOfAreaY;
        float medium = ( y1_ + y2_ ) / 2.0f ;
        y1_ = medium - realsize / 2.0f ;
        y2_ = medium + realsize / 2.0f ;
      }
      else if (sx>sy)
      {
        float realsize = sy / realSizeOfAreaX;
        float medium = ( x1_ + x2_ ) / 2.0f ;
        x1_ = medium - realsize / 2.0f ;
        x2_ = medium + realsize / 2.0f ;
      }
      break;

    case HANDLE_CORNER_TOP_LEFT:
      if (sx<sy)
      {
        float realsize = sx / realSizeOfAreaY;   
        y1_ = y2_ - realsize ;
      }
      else if (sx>sy)
      {
        float realsize = sy / realSizeOfAreaX;
        x1_ = x2_ - realsize ;
      }
      break;

    case HANDLE_CORNER_TOP_RIGHT:
      if (sx<sy)
      {
        float realsize = sx / realSizeOfAreaY;   
        y1_ = y2_ - realsize ;
      }
      else if (sx>sy)
      {
        float realsize = sy / realSizeOfAreaX;
        x2_ = x1_ + realsize ;
      }
      break;

    case HANDLE_CORNER_BOTTOM_LEFT:
      if (sx<sy)
      {
        float realsize = sx / realSizeOfAreaY;   
        y2_ = y1_ + realsize ;
      }
      else if (sx>sy)
      {
        float realsize = sy / realSizeOfAreaX;
        x1_ = x2_ - realsize ;
      }
      break;

    case HANDLE_CORNER_BOTTOM_RIGHT:
      if (sx<sy)
      {
        float realsize = sx / realSizeOfAreaY;
        if (!inverty)
          y2_ = y1_ + realsize ;
        else
          y1_ = y2_ - realsize ;
      }
      else if (sx>sy)
      {
        float realsize = sy / realSizeOfAreaX;
        if (!invertx)
          x2_ = x1_ + realsize ;
        else
          x1_ = x2_ - realsize ;
      }
    break;
  }
}

bool Cropper::isCrop() const
{
  if (x1_ <= 0.0f && y1_ <= 0.0f && x2_ >= 1.0f && y2_ >= 1.0f )
    return false;
  else
    return true;
}

void Cropper::setCropDimensions(float x1,  float y1,  float x2,  float y2  )
{
  x1_ = x1 ; y1_ = y1 ; x2_ = x2 ; y2_ = y2 ;
}

void Cropper::getCropDimensions(float& x1, float& y1, float& x2, float& y2 ) const
{
  x1 = x1_ ; y1 = y1_ ; x2 = x2_ ; y2 = y2_ ;
}

void Cropper::setCropDimensions(float x1,  float y1,  float x2,  float y2,
                                float x3,  float y3,  float x4,  float y4)
{
  DEBUG(setCropDi);
  x1_ = x1 ; y1_ = y1 ; x2_ = x2 ; y2_ = y2 ;
  x3_ = x3 ; y3_ = y3 ; x4_ = x4 ; y4_ = y4 ;
}

void Cropper::getCropDimensions(float& x1, float& y1, float& x2, float& y2,
                                float& x3, float& y3, float& x4, float& y4) const
{
  x1 = x1_ ; y1 = y1_ ; x2 = x2_ ; y2 = y2_ ;
  x3 = x3_ ; y3 = y3_ ; x4 = x4_ ; y4 = y4_ ;
}

void  Cropper::setFrameSize(float s)
{
  DEBUG(setFra);
  limit (s, 0.01f, 0.25f);
  frameSize_ = s;
}

float Cropper::getFrameSize() const
{
  return frameSize_;
}

float Cropper::getWidthRatio() const
{
  if (isCrop())
  {
    if (!isPerspective_)
      return distanceF (x2_,0.0f, x1_,0.0f) ;
    else
    {
      float dx1 = distanceF (x1_,y1_, x3_,y3_);
      float dx2 = distanceF (x4_,y4_, x2_,y2_);
      return (dx1 + dx2) / 2.0f;
    }
  }
  else
    return 1.0f;
}
