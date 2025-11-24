/*
 * VectorMap.h
 *
 *  Created on: 29 janv. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#pragma once

// if any problem with heap corruption, this may help to debug, warning it's slow
//#define DISPLACEMENTMAP_DEBUG

#include "Vector.h"
#include "Obfuscate.h"
#include "SmartPointer.h"

#define MULTIPLY 100

/// Simple 2D array containing 2 int elements, refered as x and y
///
class VectorMap
{
public:
  // constructors and destructors
  // warning : data is not initialised, use clear() to zero data before using
  VectorMap();
  VectorMap(int,int);
  ~VectorMap();

  void clear();
  void fill(int i);
  void fill(Vector vector);

  /// Copy constructor (canonic method)
  VectorMap(const VectorMap &);

  /// affectation (canonic method)
  VectorMap & operator=   (const VectorMap &);

  // Setters
  inline void   set ( int x,int y, Vector & vect  ) ;
  inline void   set ( int x,int y, int vx, int vy ) ;
  inline void   setX( int x,int y, int val        ) ;
  inline void   setY( int x,int y, int val        ) ;

  // Getters
  inline Vector get ( int x,int y ) const ;
  inline int    x   ( int x,int y ) const ;
  inline int    y   ( int x,int y ) const ;

  int getSizeX() const;
  int getSizeY() const;

  // The table is empty
  bool isEmpty() const;

  VectorMap operator*(float value) const;
  VectorMap operator/(float value) const;
  VectorMap operator-() const ;

  VectorMap getMultiply(float valuex, float valuey) const;

  // Resize table and interpolate all values
  VectorMap getResize      (int newx, int newy) const;
  VectorMap computeDownsize(int newx, int newy) const;
  VectorMap computeUpsize  (int newx, int newy) const;

  // Resize table and increase all values by same amount
  VectorMap getResizeAndMultiply(int newx, int newy) const;

  VectorMap getCrop(int x1, int y1, int x2, int y2) const;

  int   getCoordX     (int x, int y) const;
  float getCoordXFloat(int x, int y) const;
  int   getCoordY     (int x, int y) const;
  float getCoordYFloat(int x, int y) const;

  friend VectorMap operator+ (const VectorMap & a, const VectorMap & b);
  friend VectorMap operator+ (const VectorMap & a, const Vector b     );
  friend VectorMap operator- (const VectorMap & a, const VectorMap & b);
  friend VectorMap operator- (const VectorMap & a, const Vector b     );

private:
  SmartPointer<Vector,true> data_;
  int sizex_,	sizey_;  // was oint sizex_,	sizey_;

};

VectorMap operator+ (const VectorMap & a, const VectorMap & b);
VectorMap operator+ (const VectorMap & a, const Vector b     );
VectorMap operator- (const VectorMap & a, const VectorMap & b);
VectorMap operator- (const VectorMap & a, const Vector b     );


typedef SmartPointer<VectorMap,false> PVectorMap;


#ifdef DISPLACEMENTMAP_DEBUG
  #define CHECK_WITHIN_RANGE(x,y, sizex_, sizey_)\
    if (x < 0 && x >= sizex_)\
      THROW_INTERNAL_ERROR( "Wrong Argument x out of range (in MAP_DEBUG )");\
    if (y < 0 && y >= sizey_)\
      THROW_INTERNAL_ERROR( "Wrong Argument y out of range  (in MAP_DEBUG )");
#else
  #define CHECK_WITHIN_RANGE(x,y, sizex_, sizey_)
#endif


inline void   VectorMap::set (int x,int y, Vector & vect)
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  Vector* ptr = &data_[x + y * sizex_];

  *ptr = Vector ( vect.x(),vect.y() ) ;
}

inline void   VectorMap::set (int x,int y, int vx, int vy)
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  Vector* ptr = &data_[x + y * sizex_];

  *ptr = Vector ( vx,vy ) ;
}

inline void   VectorMap::setX(int x,int y, int val ) 
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  ((data_.getPointer())[x + y * sizex_]).setX( val );
}

inline void   VectorMap::setY(int x,int y, int val ) 
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  data_[x + y * sizex_].setY(val);
}

inline Vector VectorMap::get (int x,int y ) const 
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  const Vector* ptr = &data_[x + y * sizex_];

  return Vector( ptr->x(), ptr->y() );
}

inline int    VectorMap::x(int x,int y ) const 
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  return data_[x + y * sizex_].x();
}

inline int    VectorMap::y(int x,int y ) const 
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  return data_[x + y * sizex_].y();
}

