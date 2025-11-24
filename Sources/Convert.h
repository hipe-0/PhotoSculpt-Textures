/*
 * Convert.h
 *
 *  Created on: 21 dec. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

#include "IntMap.h"
#include "VectorMap.h"
#include "FloatMap.h"

inline IntMap convertToInt(const VectorMap& v)
{
  int sx = v.getSizeX();
  int sy = v.getSizeY();

  if (sx == 0 || sy == 0)
    THROW_INTERNAL_ERROR("Wrong null argument (in convertToInt())");

  IntMap u(sx, sy);

  for   (int i=0; i<sx; i++)
    for (int j=0; j<sy; j++)
      u.set(i,j,v.x(i,j));

  return u;
}


inline IntMap convertToInt(const FloatMap& fmap)
{
  int sx = fmap.getSizeX();
  int sy = fmap.getSizeY();

  if (sx == 0 || sy == 0)
    THROW_INTERNAL_ERROR("Wrong null argument (in convertToFloat())");

  IntMap intmap(sx, sy);

  for   (int i = 0; i < sx; i ++)
    for (int j = 0; j < sy; j ++)
      intmap(i,j) = round( fmap(i,j) );

  return intmap;
}

inline FloatMap convertToFloat(const IntMap& d)
{
  int sizex = d.getSizeX();
  int sizey = d.getSizeY();

  FloatMap m(sizex,sizey);

  // create picture using the dispmap information
  for   (int y=0;y<sizey;y++)
    for (int x=0;x<sizex;x++) 
      m(x,y)=float ( d(x,y) );
    
  return m;
}

inline VectorMap negativex(const VectorMap & vectormap)
{
  int sx = vectormap.getSizeX();
  int sy = vectormap.getSizeY();

  if (sx<=0 || sy<=0 ) 
    THROW_INTERNAL_ERROR( "Incorrect parameters (negativex())" );

  //Defines the new table
  VectorMap newtable(sx,sy);

  for   (int x=0; x < sx; x++)
    for (int y=0; y < sy; y++)
  {
    Vector tmp = vectormap.get(x,y);
    tmp.setX(-tmp.x());
    newtable.set(x,y, tmp);
  }

  return newtable;
}

inline VectorMap fade(const VectorMap &original, const VectorMap &modification, float fade)
{
  int sx = original.getSizeX();
  int sy = original.getSizeY();

  if (sx<=0 || sy<=0 ) 
    THROW_INTERNAL_ERROR( "Incorrect parameters (fade())" );

  if (sx != modification.getSizeX() || sy != modification.getSizeY() ) 
    THROW_INTERNAL_ERROR( "Incorrect parameters, maps have different sizes (fade())" );

  if (fade < 0.0f || fade > 1.0f ) 
    THROW_INTERNAL_ERROR( "Incorrect parameter 'fade' (fade())" );

  //Defines the new table
  VectorMap newtable(sx,sy);

  for   (int x=0; x < sx; x++)
    for (int y=0; y < sy; y++)
  {
    Vector a = original.get(x,y);
    Vector b = modification.get(x,y);
    int resx= round (float(a.x()) * (1-fade) + float (b.x()) * fade);
    int resy= round (float(a.y()) * (1-fade) + float (b.y()) * fade);

    newtable.set(x,y, resx,resy);
  }

  return newtable;
}

inline IntMap getMapX(const VectorMap& vectormap)
{
  int sx = vectormap.getSizeX();
  int sy = vectormap.getSizeY();

  IntMap map (sx, sy);

  for   (int x=0; x < sx; x++)
    for (int y=0; y < sy; y++)
      map(x,y) = vectormap.x(x,y);
  
  return map;
}

inline void setMapX(VectorMap& vectormap, IntMap intmap)
{
  int sx = vectormap.getSizeX();
  int sy = vectormap.getSizeY();

  IntMap map (sx, sy);

  for   (int x=0; x < sx; x++)
    for (int y=0; y < sy; y++)
      vectormap.setX(x,y, intmap(x,y));
}
