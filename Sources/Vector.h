/*
 * Vector.h
 *
 *  Created on: 22 april 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>
#include "Round.h"
#include "Int3.h"

template <class K, class L>
class VectorKL
{
public:
  VectorKL() {}; 
  VectorKL(K x,L y):  x_(x),  y_(y)  {};

  K x() const
  {
    return x_;
  }

  L y() const
  {
    return y_;
  }

  VectorKL& operator+=(K a)
  { 
    x_ += a ;
    y_ += a ;
    return *this;
  }
  /*
  VectorKL& operator-()
  { 
    return VectorKL(-x_, -y_);
  }*/

  VectorKL operator-() const
  { 
    return VectorKL(-x_, -y_);
  }

  VectorKL& operator+=(VectorKL a)
  {
    x_ += a.x_ ;
    y_ += a.y_ ;
    return *this;
  }

  VectorKL& operator-=(K a)
  {
    x_ -= a;
    y_ -= a;
    return *this;
  }

  VectorKL& operator-=(VectorKL a)
  {
    x_ -= a.x_ ;
    y_ -= a.y_ ;
    return *this;
  }

  VectorKL& operator*=(K a)
  {
    x_ *= a;
    y_ *= a;
    return *this;
  }

  VectorKL& operator/=(K a)
  {
    x_ /= a;
    y_ /= a;
    return *this;
  }

  void setX(K a)
  {
    x_ = a;
  }

  void setY(L a)
  {
    y_ = a;
  }


protected:
  K x_;
  L y_;
};

//typedef VectorKL<Int3, Int3> VectorS; // <-- slower (43s instead of 32s) but 11% less memory 
typedef VectorKL<int, int> Vector;

//typedef VectorS Vector; // <-- slower (43s instead of 32s) but 11% less memory 
class VectorS: public VectorKL<Int3, Int3>
{
public:
  VectorS()                 : VectorKL<Int3, Int3>()             { }
  VectorS(int x,int y)      : VectorKL<Int3, Int3>(x,y)          { }
  VectorS(const VectorS& v) : VectorKL<Int3, Int3>(v)            { }
  VectorS(const Vector& v)  : VectorKL<Int3, Int3>(v.x() , v.y()){ }
};

class VectorF: public VectorKL<float,float>
{
public:
  VectorF()                 : VectorKL<float,float>()     { }
  VectorF(float x,float y)  : VectorKL<float,float>(x,y)  { }
  VectorF(const VectorF& v) : VectorKL<float,float>(v)    { }
  inline float getLength () const ;
  inline float getAzimuth() const;
};

inline float VectorF::getLength() const
{
  return sqrtf( x_ * x_ + y_ * y_ );
}

inline float VectorF::getAzimuth() const
{
  return atan2f(y_, x_) * (180.0f / 3.1415926535897932384626433832795f); // convert radians to degrees
}

inline float angle(VectorF a, VectorF b)
{
  return b.getAzimuth() - a.getAzimuth();
}

inline Vector operator+(Vector a, Vector b)
{
  return Vector(a.x()+b.x(), a.y()+b.y());
}

inline Vector operator-(Vector a, Vector b)
{
  return Vector(a.x()-b.x(), a.y()-b.y());
}

inline Vector operator*(int a, Vector b)
{
  return Vector(a*b.x(), a*b.y());
}

inline Vector operator*(Vector a, int b)
{
  return Vector(a.x()*b, a.y()*b);
}

inline Vector operator/(Vector a, int b)
{
  return Vector(a.x()/b, a.y()/b);
}

inline Vector operator/(Vector a, float b)
{
  return Vector( round( float(a.x()) / b ), 
	             round( float(a.y()) / b ) );
}



inline VectorF operator+(VectorF a, VectorF b)
{
  return VectorF(a.x()+b.x(), a.y()+b.y());
}

inline VectorF operator-(VectorF a, VectorF b)
{
  return VectorF(a.x()-b.x(), a.y()-b.y());
}

inline VectorF operator*(VectorF a, int b)
{
  return VectorF(a.x()*b, a.y()*b);
}

inline VectorF operator*(VectorF a, float b)
{
  return VectorF(a.x()*b, a.y()*b);
}

inline VectorF operator*(int a, VectorF b)
{
  return VectorF(a*b.x(), a*b.y());
}

inline VectorF operator/(VectorF a, int b)
{
  return VectorF(a.x()/b, a.y()/b);
}

inline VectorF operator/(VectorF a, float b)
{
  return VectorF(a.x()/b, a.y()/b);
}


#endif /* VECTOR_H_ */
