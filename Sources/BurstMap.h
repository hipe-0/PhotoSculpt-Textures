/* BurstMap.h
 *
 *  Created on: 15 avr. 2009
 *  Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

/// Table of x y coordinates starting at 0,0 and growing around this set of coordinates
//
// note : this table is static, only one is in memory
// note : values are like for example
//          0,0 ;
//          0,1 ; 1,0 ; -1,0 ; 0,-1 ;
//          1,1 ; -1,1 ; -1,-1 ; 1,-1 ;
//          2,0 ; 0,2 ; ...
//
class BurstMap 
{
public:
  static void init(int size, int density);

  /// get coordinates and distance to this coordinate
  inline static int   x       (int index);
  inline static int   y       (int index);
  inline static float distance(int index);

  /// give the number of elements to be used in this table
  static int getMaxTableIndex(int scope);
  static int getMaxSize();
 
private:
  static int   size_ , density_ ;
  
  static int   x_[], y_[];

  static float dist_[];
  static int   reverseSearch_[];
};

inline int BurstMap::x(int index)
{
  return x_[index];
}

inline int BurstMap::y(int index)
{
  return y_[index];
}

inline float BurstMap::distance(int index)
{
  return dist_[index];
}

inline int BurstMap::getMaxSize()
{
  return size_;
}
