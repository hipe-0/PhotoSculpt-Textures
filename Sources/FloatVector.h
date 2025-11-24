/*
 * FloatVector.h
 *
 *  Created on: 29 janv. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once

// if any problem with heap corruption, this may help to debug, warning it's slow
//#define DISPLACEMENTMAP_DEBUG

#include "Obfuscate.h"
#include "SmartPointer.h"

/// Simple 2D array containing 2 int elements, refered as x and y
///
class FloatVector
{
public:
  // constructors and destructors
  // warning : data is not initialised, use clear() to zero data before using
  FloatVector();
  FloatVector(int newsize);
  FloatVector(float f1);
  FloatVector(float f1, float f2);
  FloatVector(float f1, float f2, float f3);
  FloatVector(float f1, float f2, float f3, float f4);
  FloatVector(float f1, float f2, float f3, float f4, float f5);
  FloatVector(float f1, float f2, float f3, float f4, float f5, float f6);
  FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7);
  FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8);
  FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9);
  FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10);
  ~FloatVector();

  void clear();
  void fill(int i);
  void fill(float value);
  void randomizeFrom0To1();

  /// Copy constructor (canonic method)
  FloatVector               ( const FloatVector & );

  /// affectation (canonic method)
  FloatVector & operator=   ( const FloatVector & );

  // Setters
  inline void  set ( int x, float value  ) ;

  // Getters
  inline float get ( int x ) const ;

  int  getSize() const;

  // The table is empty
  bool isEmpty() const;

  FloatVector operator*  (float value) const;
  FloatVector operator/  (float value) const;
  FloatVector operator-  ()            const;
  FloatVector getAbsolute()            const;
  FloatVector getSum     ()            const;
  FloatVector getMean    ()            const;
  float       getLength  ()            const;
  float       operator[] (int x)       const;
  float&      operator[] (int x)            ;

  // Resize table and interpolate all values
  FloatVector getResize(int newx) const;

  FloatVector getCrop(int x1, int x2) const;

  void writeToString(char* text, int buffersize) const;

  friend FloatVector operator+  (const FloatVector & a, const FloatVector & b);
  friend FloatVector operator+  (const FloatVector & a, const float b         );
  friend FloatVector operator-  (const FloatVector & a, const FloatVector & b);
  friend FloatVector operator-  (const FloatVector & a, const float b         );
  friend FloatVector operator*  (const FloatVector & a, const FloatVector & b);
  friend FloatVector getMinimum (const FloatVector & a, const float b        );
  friend FloatVector getMaximum (const FloatVector & a, const float b        );
  friend FloatVector getMinimum (const FloatVector & a, const FloatVector & b);
  friend FloatVector getMaximum (const FloatVector & a, const FloatVector & b);

private:
  SmartPointer<float,true> data_;
  oint size_;
};

FloatVector operator+ (const FloatVector & a,  const FloatVector & b);
FloatVector operator+ (const FloatVector & a,  const float b        );
FloatVector operator- (const FloatVector & a,  const FloatVector & b);
FloatVector operator- (const FloatVector & a,  const float b        );
FloatVector operator* (const FloatVector & a,  const FloatVector & b);
FloatVector getMinimum (const FloatVector & a, const FloatVector & b);
FloatVector getMaximum (const FloatVector & a, const FloatVector & b);
FloatVector getMinimum (const FloatVector & a, const float b        );
FloatVector getMaximum (const FloatVector & a, const float b        );

#ifdef FLOATVECTOR_DEBUG
  #define FLOATVECTOR_CHECK_WITHIN_RANGE(x, size_)\
    if (x < 0 && x >= sizex_)\
      THROW_INTERNAL_ERROR( "Wrong Argument x out of range (in VECTOR1D_DEBUG )");
#else
  #define FLOATVECTOR_CHECK_WITHIN_RANGE(x, size_)
#endif

