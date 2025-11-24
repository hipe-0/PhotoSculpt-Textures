/*
 * SourceScreens.cpp
 *
 *  Created on: 06 fev. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include "SourceScreens.h"
#include "Error.h"

SourceScreens::SourceScreens()
{
  xl_ = yl_ = xr_ = yr_ = sxl_ = syl_ = sxr_ = syr_ = -1;
}

bool SourceScreens::isCursorInImageL(int x, int y, float& coordx, float& coordy)
{
  bool isInside = (x > xl_ && x < xl_ + sxl_ - 1) && (y > yl_ && y < yl_ + syl_ - 1);

  if (isInside)
  {
    coordx = float ( x - xl_ ) / float (sxl_) ;
    coordy = float ( y - yl_ ) / float (syl_) ;
  }
  else
    coordx = coordy = -1.0f;

  return  isInside;
}
bool SourceScreens::isCursorInImageR(int x, int y, float& coordx, float& coordy)
{
  bool isInside = (x > xr_ && x < xr_ + sxr_ - 1) && (y > yr_ && y < yr_ + syr_ - 1);

  if (isInside)
  {
    coordx = float ( x - xr_ ) / float (sxr_) ;
    coordy = float ( y - yr_ ) / float (syr_) ;
  }
  else
    coordx = coordy = -1.0f;

  return  isInside;
}
void SourceScreens::convertCoordToImageL(float coordx, float coordy, int& x, int& y)
{
  x = xl_ + int( round( coordx * float(sxl_) ) );
  y = yl_ + int( round( coordy * float(syl_) ) );
}
void SourceScreens::convertCoordToImageR(float coordx, float coordy, int& x, int& y)
{
  x = xr_ + int( round( coordx * float(sxr_) ) );
  y = yr_ + int( round( coordy * float(syr_) ) );
}
void SourceScreens::setL(int xl, int yl, int sxl, int syl)
{
  xl_  = xl;    yl_  = yl;
  sxl_ = sxl;   syl_ = syl;
}
void SourceScreens::setR(int xr, int yr, int sxr, int syr)
{
  xr_  = xr;    yr_  = yr;
  sxr_ = sxr;   syr_ = syr;
}

void SourceScreens::drawPointL(float coordx, float coordy, Image& i, unsigned char color[3])
{
  int x,y;
  convertCoordToImageL(coordx,coordy, x,y);
  
  const int r=2;
  i.draw_rectangle(x-r,y-r,x+r,y+r,color);
}

void SourceScreens::drawPointR(float coordx, float coordy, Image& i, unsigned char color[3])
{
  int x,y;
  convertCoordToImageR(coordx,coordy, x,y);
  
  const int r=2;
  i.draw_rectangle(x-r,y-r,x+r,y+r,color);
}

void SourceScreens::drawLine  (float coordlx, float coordly, 
                               float coordrx, float coordry, Image& i, 
                               unsigned char color[3], float opacity)
{
  int lx,ly, rx,ry;
  convertCoordToImageL(coordlx,coordly, lx,ly);
  convertCoordToImageR(coordrx,coordry, rx,ry);

  i.draw_line(lx,ly,rx,ry,color,opacity);
}


int SourceScreens::FtoI_( float valuefloat , int size) 
{
  return int ( round (float ( size - 1) * valuefloat ) );
}

float SourceScreens::ItoF_( int valueint , int size)
{
  return float (valueint) / float (size - 1) ;
}

void SourceScreens::correlateLtoR(float  coordlx, float  coordly, 
                                  float& coordrx, float& coordry, const VectorMap& m)
{
  if (m.isEmpty())
    THROW_INTERNAL_ERROR( "Map is empty (in correlateLtoR())" );

  if (coordlx<0.0f || coordlx>1.0f || coordly<0.0f || coordly>1.0f)
    THROW_INTERNAL_ERROR( "Wrong arguments (in correlateLtoR())" );

  int intcursorlx = FtoI_ ( coordlx, m.getSizeX() );
  int intcursorly = FtoI_ ( coordly, m.getSizeY() );

  Vector v = m.get(intcursorlx, intcursorly);
  v= Vector(intcursorlx, intcursorly) + ( v / MULTIPLY );
  
  coordrx = ItoF_ ( v.x(), m.getSizeX() ) ;
  coordry = ItoF_ ( v.y(), m.getSizeY() ) ;
}
