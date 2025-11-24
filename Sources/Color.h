/* Color.h
 *
 *  Created on: 29 janv. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "COMPILER_OPTIONS.h"
#include "Image.h"

#include <math.h>
#include "Round.h"
#include "SmartPointer.h"

class Color
{
public :
  unsigned char r,g,b;
  Color(unsigned char colorr,unsigned char colorg,unsigned char colorb);
  Color();
};

class ColorF
{
public:
  float r,g,b;

  ColorF(float colorr, float colorg, float colorb);
  ColorF();
};

inline Color getPixel(const Image& I, int x, int y)
{
  Color c;
  c.r=I(x,y,0,0);
  c.g=I(x,y,0,1);
  c.b=I(x,y,0,2);
  return c;
}

inline Color getPixel(const Image& image, float uvmapx, float uvmapy)
{
  Color c;

  int x = round(uvmapx * float (image.dimx()-1));
  int y = round(uvmapy * float (image.dimy()-1));

  c.r = image(x,y,0,0);
  c.g = image(x,y,0,1);
  c.b = image(x,y,0,2);
  return c;
}


inline void setPixel(Image& I, int x, int y, Color c)
{
  I(x,y,0,0)=c.r;
  I(x,y,0,1)=c.g;
  I(x,y,0,2)=c.b;
}

inline ColorF getPixelF(const Image& I, int x, int y)
{
  ColorF c;
  c.r=(float)I(x,y,0,0)/255.0f;
  c.g=(float)I(x,y,0,1)/255.0f;
  c.b=(float)I(x,y,0,2)/255.0f;
  return c;
}

inline void setPixelF(Image& I, int x, int y, ColorF c)
{
  I(x,y,0,0)=(unsigned char)round((c.r*255.0f));
  I(x,y,0,1)=(unsigned char)round((c.g*255.0f));
  I(x,y,0,2)=(unsigned char)round((c.b*255.0f));
}

inline Color::Color(unsigned char colorr, unsigned char colorg, unsigned char colorb):
  r(colorr),g(colorg),b(colorb)
  {};

inline Color::Color():
  r(0),g(0),b(0)
  {};

inline ColorF::ColorF(float colorr,float colorg,float colorb):
  r(colorr),g(colorg),b(colorb)
  {};

inline ColorF::ColorF():
  r(0.0),g(0.0),b(0.0)
  {};

