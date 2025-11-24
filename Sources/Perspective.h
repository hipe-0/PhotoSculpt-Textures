/*
 * Convert.h
 *
 *  Created on: 21 dec. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include "IntMap.h"
#include "VectorMap.h"

// converts a value to a -1 ... +1 range, given it's middle value and scope
inline float convertToMinus1Plus1Range(float x, float middle, int scope)
{
  return (float(x)-float(middle)) / float(scope) * 2.0f ;
}

inline float convertFromMinus1Plus1Range(float x, float middle, int scope)
{
  return x / 2.0f * float(scope) + float(middle) ;
}

// mixes a bit of A0 and a bit of A1 according to x (from 0 to 1)
inline float mix (float x, float A0, float A1)
{
  return A0 + x * (A1 - A0);
}

inline float mix (float x, float y, float A00, float A10, float A01, float A11)
{
  float u = mix (x, A00, A10);
  float d = mix (x, A01, A11);
  return u + y * (d - u);
}

// distance of the point at coordinates x and y to the origin (0,0)
inline float dist(float x, float y)
{
  return sqrtf ( x * x + y * y );
}

inline void getInvertedZs(const VectorMap& map, float& invertz_00, float& invertz_10, 
                          float& invertz_01, float& invertz_11)
{
  int sx = map.getSizeX();
  int sy = map.getSizeY();

  if (sx == 0 || sy == 0)
    THROW_INTERNAL_ERROR("Wrong null argument (in correctPerspective())");

  float ratio = 0.5f;
  int L = sx/2 - round(float(sx)*ratio / 2.0f);
  int R = sx/2 + round(float(sx)*ratio / 2.0f);
  int U = sy/2 - round(float(sy)*ratio / 2.0f); 
  int D = sy/2 + round(float(sy)*ratio / 2.0f);
 
  // 4 points are used for estimating the grid deformation
  // L = left, R = Right, U = up , D = down
  float X00map = float( map.x(L,U) ) / MULTIPLY + L ;
  float X10map = float( map.x(R,U) ) / MULTIPLY + R ;
  float X01map = float( map.x(L,D) ) / MULTIPLY + L ;
  float X11map = float( map.x(R,D) ) / MULTIPLY + R ;
  
  float Y00map = float( map.y(L,U) ) / MULTIPLY + U ;
  float Y10map = float( map.y(R,U) ) / MULTIPLY + U ;
  float Y01map = float( map.y(L,D) ) / MULTIPLY + D ;
  float Y11map = float( map.y(R,D) ) / MULTIPLY + D ;
  
  // all those values are converted to -1...+1 values
  float X00 = convertToMinus1Plus1Range( X00map , float((sx)/2), sx-1);
  float X10 = convertToMinus1Plus1Range( X10map , float((sx)/2), sx-1);
  float X01 = convertToMinus1Plus1Range( X01map , float((sx)/2), sx-1);
  float X11 = convertToMinus1Plus1Range( X11map , float((sx)/2), sx-1);
  
  float Y00 = convertToMinus1Plus1Range( Y00map , float((sy)/2), sx-1);
  float Y10 = convertToMinus1Plus1Range( Y10map , float((sy)/2), sx-1);
  float Y01 = convertToMinus1Plus1Range( Y01map , float((sy)/2), sx-1);
  float Y11 = convertToMinus1Plus1Range( Y11map , float((sy)/2), sx-1);

  float heightl = (Y01 - Y00) / 2.0f ; // left
  float heightr = (Y11 - Y10) / 2.0f ; // right

  float Xcenter = ((X00 + X01) / heightl + (X10  + X11) / heightr) /
                  2.0f / (1.0f / heightl + 1.0f / heightr);
  float Ycenter = ((Y00 + Y01) / heightl + (Y10  + Y11) / heightr) / 
                  2.0f / (1.0f / heightl + 1.0f / heightr);

  float s = dist(1.0f , float(sy) / float(sx) ) * ratio;

  // method of distance to center
  float z00_from_d = abs( s / dist(X00-Xcenter, Y00-Ycenter) ) ;
  float z10_from_d = abs( s / dist(X10-Xcenter, Y10-Ycenter) ) ;
  float z01_from_d = abs( s / dist(X01-Xcenter, Y01-Ycenter) ) ;
  float z11_from_d = abs( s / dist(X11-Xcenter, Y11-Ycenter) ) ;

  invertz_00 = 1.0f / z00_from_d;
  invertz_10 = 1.0f / z10_from_d;
  invertz_01 = 1.0f / z01_from_d;
  invertz_11 = 1.0f / z11_from_d;
}


inline IntMap getPerspCorrectedHMap(const VectorMap& map)
{
  int sx = map.getSizeX();
  int sy = map.getSizeY();

  if (sx == 0 || sy == 0)
    THROW_INTERNAL_ERROR("Wrong null argument (in correctPerspective())");

  IntMap newmap(sx, sy);

  // The method corrects the deformation implied by perspective.
  // The deformation is estimated with a method close to trilinear filtering
  // 1/Z value is estimated for each x,y, based on 4 1/Z values for left right up and down
  
  // estimates 1/Z values for left right up and down
  float invertz_00, invertz_10, invertz_01, invertz_11;
  getInvertedZs(map, invertz_00, invertz_10, invertz_01, invertz_11);
  
  for   (int u = 0; u < sx; u ++)
    for (int v = 0; v < sy; v ++)
    {
      float fu = float (u) / float(sx-1);   // fu goes from 0.0 to 1.0
      float fv = float (v) / float(sy-1);   // fv goes from 0.0 to 1.0
      float X  = convertToMinus1Plus1Range(float(u),float((sx)/2), sx-1); // X goes from -1.0 to 1.0

      // 1/Z value is computed
      float invertz = mix(fu,fv, invertz_00, invertz_10,invertz_01,invertz_11);

      float x = X / invertz ;

      float xshouldhavebeen = convertFromMinus1Plus1Range(x, float((sx)/2), sx-1) * float(MULTIPLY);
      float xwas            = float(u)* float(MULTIPLY) ;
      float xcorrector      = xshouldhavebeen - xwas ;

      newmap(u,v) = map.x(u,v) + round(xcorrector);
    }

  return newmap;
}

extern IntMap getMapX(const VectorMap& vectormap);

#include "POV.h"
inline IntMap getPerspCorrectedMap(const VectorMap& map, const POV& pointofview)
{
  int sx   = map.getSizeX(), sy = map.getSizeY();
  IntMap flatxcoords = getMapX( pointofview.generateMapOfFlatObject(sx,sy) );

  IntMap correctedmap = getMapX(map) - flatxcoords;

  int altitudeatcenter = correctedmap(sx/2, sy/2);
  return correctedmap + IntMap(sx,sy).fill(-altitudeatcenter); // rezero the map using center
}