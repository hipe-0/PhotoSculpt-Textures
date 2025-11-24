/*
 * IntMap.h
 *
 *  Created on: 21 dec. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

// if any problem with heap corruption, this may help to debug, warning it's slow
//#define DISPLACEMENTMAP_DEBUG

#include "Map.h"
#include "Obfuscate.h"
#include "SmartPointer.h"

/// Simple 2D array containing 2 int elements, refered as x and y
///
class IntMap : public Map<int>
{
public:
 
  IntMap();                           // Default constructor
  IntMap(int,int);                    // constructor
  IntMap(const IntMap &);             // Copy constructor (canonic method)

  IntMap& fill (int value);

  IntMap operator*(float value) const;
  IntMap operator/(float value) const;
  IntMap operator+(float value) const;
  IntMap operator-(float value) const;
  IntMap operator-() const ;

  IntMap getMultiply(float value) const;

  // Resize table and interpolate all values
  IntMap getResize(int newx, int newy) const;

  // Resize table and increase all values by same amount
  IntMap getResizeAndMultiply(int newx, int newy) const;

  IntMap getCrop(int x1, int y1, int x2, int y2) const;

  friend IntMap operator+(const IntMap & a, const IntMap & b);
  friend IntMap operator-(const IntMap & a, const IntMap & b);

  //static void copyAt (IntMap& dest,   const IntMap& src, int atX, int atY);
  //static void repeatN(IntMap& intmap, int nb, int framesizex, int framesizey);
};


typedef SmartPointer<IntMap,false> PIntMap;

