/*
 * Vector3.h
 *
 *  Created on: 16 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef VECTOR3_H_
#define VECTOR3_H_

class  Vector3
{
public:
  float x, y, z;

  // Default constructor fills with zeros.
  Vector3();

  // standard constructor
  Vector3(float  set_x, float  set_y, float  set_z);
  Vector3(double set_x, double set_y, double set_z);
  Vector3(int    set_x, int    set_y, int    set_z);
  Vector3(long   set_x, long   set_y, long   set_z);

  float getLength();

  // Will set the vector lenght to 1.0 without modifying the vector orientation
  // Warning, will crash if length == 0
  void setNormalize();

  void setZero();
  void setValue(int   set_x, int   set_y, int   set_z);
  void setValue(float set_x, float set_y, float set_z);

  void setValueXYZ(int set);

  void setValueXYZ(float set);
  void setNegative();

  bool isNull();
};



// 3D rotation along axis
Vector3 rotateX( const Vector3& v,   float angle);
Vector3 rotateY( const Vector3& v,   float angle);
Vector3 rotateZ( const Vector3& v,   float angle);

// Dot product
float operator*( const Vector3& a,   const Vector3& b);

float length   (Vector3 const &v);

/// normalize this vector v = set length to 1.0 ; keeps vector direction the same
// Warning:  will crash if length == 0
//
Vector3 normalize(Vector3 const & v);

// cross product
Vector3 operator^(const Vector3& v,  const Vector3& w);
Vector3 operator+(const Vector3& v,  const Vector3& w);
Vector3 operator-(const Vector3& v,  const Vector3& w);

Vector3 operator*(const Vector3& v,  float f);
Vector3 operator*(float f,           const Vector3& v);

Vector3 operator/(const Vector3& v,  float f);
Vector3 operator/(float f,           const Vector3& v);

Vector3 operator-(const Vector3& v);


#endif /* VECTOR3_H_ */
