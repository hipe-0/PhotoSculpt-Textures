/*
 * Vector.h
 *
 *  Created on: 22 april 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef VECTOR_H_
#define VECTOR_H_

#include <math.h>

class Vector
{
private:
  int vx,vy;

public:
  Vector(int valuex,int valuey):
    vx(valuex),
    vy(valuey)
    {};


  int x()
  {
    return vx;
  }

  int y()
  {
    return vy;
  }

  Vector& operator+=(int a)
  {
    vx+=a;
    vy+=a;
    return *this;
  }

  Vector& operator+=(Vector a)
  {
    vx+=a.vx;
    vy+=a.vy;
    return *this;
  }

  Vector& operator-=(int a)
  {
    vx-=a;
    vy-=a;
    return *this;
  }

  Vector& operator-=(Vector a)
  {
    vx-=a.vx;
    vy-=a.vy;
    return *this;
  }

  Vector& operator*=(int a)
  {
    vx*=a;
    vy*=a;
    return *this;
  }
  Vector& operator/=(int a)
  {
    vx/=a;
    vy/=a;
    return *this;
  }

};

inline Vector operator+(Vector a, Vector b)
{
  return Vector(a.x()+b.x(), a.y()+b.y());
}

inline Vector operator-(Vector a, Vector b)
{
  return Vector(a.x()-b.x(), a.y()-b.y());
}

inline Vector operator*(Vector a, int b)
{
  return Vector(a.x()*b, a.y()*b);
}

inline Vector operator*(int a, Vector b)
{
  return Vector(a*b.x(), a*b.y());
}

inline Vector operator/(Vector a, int b)
{
  return Vector(a.x()/b, a.y()/b);
}


#endif /* VECTOR_H_ */
