/*
 * DisplacementMapClass.h
 *
 *  Created on: 29 janv. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

// if any problem with heap corruption, this may help to debug, warning it's slow
#include "Obfuscate.h"
#include "Includes.h"
#include "Map.h"
#include "SmartPointer.h"

/// simple 2D Array containing one float as element
class FloatMap: public Map<float>
{
public:
  FloatMap();
  FloatMap(int x,int y);
  FloatMap(const FloatMap&);

  inline float  getLinear(float, float) const;

  FloatMap      operator*(float value) const;
  FloatMap      operator/(float value) const;

  void          fill (float value);

  FloatMap getMultiply(float valuex, float valuey) const;

  // Resize table and interpolate all values
  FloatMap getResize(int newx, int newy) const;

  // Resize table and increase all values by same amount
  FloatMap getResizeAndMultiply(int newx, int newy) const;

  friend FloatMap operator+ (const FloatMap & a, const FloatMap & b);
  friend FloatMap operator+ (const FloatMap & a, const float b     );
  friend FloatMap operator- (const FloatMap & a, const FloatMap & b);
  friend FloatMap operator- (const FloatMap & a, const float b     );
};

typedef SmartPointer<FloatMap,false> PFloatMap;

inline float FloatMap::getLinear(float x, float y) const
{
  #ifdef VECTOR_DEBUG
    if ((sizex_<=0 && sizey_<=0) || data_ == NULL)
      THROW_INTERNAL_ERROR ("empty map (in getLinear())");
    if (x<0 || y<0 || x>sizex_-1 || y>sizey_-1)
      THROW_INTERNAL_ERROR ("wrong argument (in getLinear())");
  #endif

  // linear interpolation is computed
  int xround=int(x);
  int yround=int(y);

  // fx and fy values are between 0.0 and 1.0
  // they correspond to the position between the square of 4 values
  float fx=x-float(xround);
  float fy=y-float(yround);

  int xroundplus1 = xround+1;
  int yroundplus1 = yround+1;

  if (xroundplus1 >= sizex_)   xroundplus1 = sizex_-1;
  if (yroundplus1 >= sizey_)   yroundplus1 = sizey_-1;

  // gets all the 4 corner values of the square on the old map that we're pointing at
  // 00=upper left corner, 01=upper right corner, 10= lower left, 11= lower right
  float x00=(this->operator())( xround      , yround      );
  float x01=(this->operator())( xroundplus1 , yround      );
  float x10=(this->operator())( xround      , yroundplus1 );
  float x11=(this->operator())( xroundplus1 , yroundplus1 );

  // Here are the interpolated values
  return (x00 * (1.0f-fx) + x01 * fx) * (1.0f-fy) + (x10 * (1.0f-fx) + x11 * fx) * fy;
}
