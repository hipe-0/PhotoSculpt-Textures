/*
 * MonoScreen.cpp
 *
 *  Created on: 06 fev. 2011
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */
#include "MonoScreen.h"
#include "Timer.h"
#include "Round.h"

MonoScreen::MonoScreen()
{
  screenPosX_ = screenPosY_ = screenSizeX_ = screenSizeY_ = -1;
}

bool MonoScreen::isCursorInImage(int x, int y, float& coordx, float& coordy)
{
  bool isInside = (x > screenPosX_ && x < screenPosX_ + screenSizeX_ - 1) && (y > screenPosY_ && y < screenPosY_ + screenSizeY_ - 1);

  coordx = float ( x - screenPosX_ ) / float (screenSizeX_) ;
  coordy = float ( y - screenPosY_ ) / float (screenSizeY_) ;

  return  isInside;
}

void MonoScreen::convertCoordToImage(float coordx, float coordy, int& x, int& y)
{
  x = screenPosX_ + round( coordx * float(screenSizeX_) );
  y = screenPosY_ + round( coordy * float(screenSizeY_) );
}

void MonoScreen::set(int xl, int yl, int sxl, int syl)
{
  screenPosX_  = xl;    screenPosY_  = yl;
  screenSizeX_ = sxl;   screenSizeY_ = syl;
}

int MonoScreen::getSizeX()
{
  return screenSizeX_ ;
}

int MonoScreen::getSizeY()
{
  return screenSizeY_ ;
}

int MonoScreen::getPosX()
{
  return screenPosX_ ;
}

int MonoScreen::getPosY()
{
  return screenPosY_ ;
}

void MonoScreen::drawPoint(float coordx, float coordy, Image& i, unsigned char color[3])
{
  int x,y;
  convertCoordToImage(coordx,coordy, x,y);
  
  const int r=2;
  i.draw_rectangle(x-r,y-r,x+r,y+r,color);
}



void MonoScreen::drawLine  (float coordx1, float coordy1, 
                            float coordx2, float coordy2, 
                            Image& i, const unsigned char color[3], float opacity)
{
  unsigned char c[3];
  c[0] = color[0];
  c[1] = color[1];
  c[2] = color[2];
  static Timer time;
  const int SEGMENT_LENGTH = 5 ;
  const int TICK_N_TIMES_PER_SECONDS = 4;
  int x1,y1,x2,y2;
  convertCoordToImage(coordx1,coordy1, x1,y1);
  convertCoordToImage(coordx2,coordy2, x2,y2);
  
  float length = sqrtf( float((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))) ;

  if (length>0.0f)
  {
    float x=float(x1) , y=float(y1);
    float dx = ( x2 - x1 ) / length ;
    float dy = ( y2 - y1 ) / length ;

    float firstsegment = fmod (time.getSeconds()*float(TICK_N_TIMES_PER_SECONDS) , 
                         float(SEGMENT_LENGTH)) ;

    bool firstcolor = bool ( fmod (time.getSeconds()*float(TICK_N_TIMES_PER_SECONDS)/float(SEGMENT_LENGTH)/2.0f, 2.0f));
    if (firstcolor)
    { c[0]=255-c[0]; c[1]=255-c[1]; c[2]=255-c[2]; }

    float xn = x + dx * firstsegment , yn = y + dy * firstsegment ;

    i.draw_line(x1,y1,int(round(xn)),int(round(yn)),c, opacity);
    x=xn; y=yn;
    c[0]=255-c[0]; c[1]=255-c[1]; c[2]=255-c[2];

    if (length>SEGMENT_LENGTH+firstsegment)
      for (float d = firstsegment ; d < length-float(SEGMENT_LENGTH) ; 
           d += float(SEGMENT_LENGTH))
      {
        float xn = x + dx * SEGMENT_LENGTH , yn = y + dy * SEGMENT_LENGTH ;
        i.draw_line(int(round(x)),int(round(y)),int(round(xn)),int(round(yn)),c, opacity);
        x=xn; y=yn;
        c[0]=255-c[0]; c[1]=255-c[1]; c[2]=255-c[2];
      }

    i.draw_line(int(round(x)),int(round(y)),x2,y2,c, opacity);
  }
}


void MonoScreen::drawFrame (float coordx1, float coordy1, 
                            float coordx2, float coordy2, 
                            Image& i, unsigned char color[3], float opacity)
{
  drawLine(coordx1,coordy1, coordx1, coordy2, i, color, opacity);
  drawLine(coordx1,coordy1, coordx2, coordy1, i, color, opacity);
  drawLine(coordx2,coordy1, coordx2, coordy2, i, color, opacity);
  drawLine(coordx1,coordy2, coordx2, coordy2, i, color, opacity);
}

void MonoScreen::drawFrame (float coordx1, float coordy1, 
                            float coordx2, float coordy2, 
                            float coordx3, float coordy3, 
                            float coordx4, float coordy4,
                            Image& i, unsigned char color[3], float opacity)
{
  drawLine(coordx1,coordy1, coordx2, coordy2, i, color, opacity);
  drawLine(coordx2,coordy2, coordx4, coordy4, i, color, opacity);
  drawLine(coordx3,coordy3, coordx4, coordy4, i, color, opacity);
  drawLine(coordx1,coordy1, coordx3, coordy3, i, color, opacity);
}

void MonoScreen::drawDot (float coordx1, float coordy1,  
                          Image& i, unsigned char color[3], float opacity)
{
  int x1,y1;
  convertCoordToImage(coordx1,coordy1, x1,y1);

  const int DOT_SIZE = 5;
  int halfDot=(DOT_SIZE-1)/2;

  i.draw_rectangle(x1-halfDot,y1-halfDot,x1-halfDot+DOT_SIZE,y1-halfDot+DOT_SIZE,color, opacity);
}

int MonoScreen::getDistance(float coordx1, float coordy1, float coordx2, float coordy2)
{
  int x1,y1;
  convertCoordToImage(coordx1,coordy1, x1,y1);
  int x2,y2;
  convertCoordToImage(coordx2,coordy2, x2,y2);

  int distance = int(round(sqrtf(float((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)))));
  return distance;
}

int MonoScreen::FtoI_( float valuefloat , int size) 
{
  return int ( round (float ( size - 1 ) * valuefloat ) );
}

float MonoScreen::ItoF_( int valueint , int size)
{
  return float (valueint) / float ( size - 1 ) ;
}
