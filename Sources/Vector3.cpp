#include "Vector3.h"
#include <math.h>

// Default constructor fills with zeros.
Vector3::Vector3(): x(0.0), y(0.0), z(0.0) {}

// standard constructor
Vector3::Vector3(float set_x, float set_y, float set_z):
  x(set_x), 
  y(set_y), 
  z(set_z) {}

Vector3::Vector3(double set_x, double set_y, double set_z):
  x(float(set_x)), 
  y(float(set_y)), 
  z(float(set_z)) {}

Vector3::Vector3(int set_x, int set_y, int set_z):
  x(float(set_x)), 
  y(float(set_y)), 
  z(float(set_z)) {}

Vector3::Vector3(long set_x, long set_y, long set_z):
  x(float(set_x)), 
  y(float(set_y)), 
  z(float(set_z)) {}

float Vector3::getLength()
{
  return (sqrt(x*x + y*y + z*z));
}

// Will set the vector lenght to 1.0 without modifying the vector orientation
// Warning, will crash if length == 0
void Vector3::setNormalize()
{
  float len = getLength();
  x /= len;
  y /= len;
  z /= len;
}

void Vector3::setZero()
{
  x=0; y=0 ; z=0;
}

void Vector3::setValue(int set_x, int set_y, int set_z)
{
  x=float(set_x); 
  y=float(set_y); 
  z=float(set_z);
}

void Vector3::setValue(float set_x, float set_y, float set_z)
{
  x=set_x; y=set_y ; z=set_z;
}

void Vector3::setValueXYZ(int set)
{
  x=float(set);
  y=float(set); 
  z=float(set);
}

void Vector3::setValueXYZ(float set)
{
  x=set; y=set ; z=set;
}

void Vector3::setNegative()
{
  x=-x; y=-y ; z=-z;
}

bool Vector3::isNull()
{
  return (x==0) && (y==0) && (z==0);
}


// 3D rotation along axis
Vector3 rotateX(Vector3 const &v, float angle)
{
  return Vector3(
      v.x,
      v.y * float(cos(angle)) - v.z * float(sin(angle)),
      v.z * float(cos(angle)) + v.y * float(sin(angle)));
}

Vector3 rotateY(Vector3 const &v, float angle)
{
  return Vector3(
      v.x * float(cos(angle)) + v.z * float(sin(angle)),
      v.y,
      v.z * float(cos(angle)) - v.x * float(sin(angle))
  );
}

Vector3 rotateZ(Vector3 const &v, float angle)
{
  return Vector3(
      v.x * float(cos(angle)) - v.y * float(sin(angle)),
      v.y * float(cos(angle)) + v.x * float(sin(angle)),
      v.z);
}

// Dot product
float operator*(Vector3 const &a, Vector3 const &b)
{
  return a.x*b.x+a.y*b.y+a.z*b.z;
}

float length(Vector3 const &v)
{
  return (sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
}

// Warning, will crash if length == 0
Vector3 normalize(Vector3 const & v)
{
  float len = length(v);
  return Vector3(
      v.x / len,
      v.y / len,
      v.z / len);
}

// cross product
Vector3 operator^(Vector3 const &v, Vector3 const &w)
{
  return Vector3(
      v.y*w.z - v.z*w.y,
      v.z*w.x - v.x*w.z,
      v.x*w.y - v.y*w.x );
}

Vector3 operator+(Vector3 const &v, Vector3 const &w)
{
  return Vector3( v.x + w.x, v.y + w.y, v.z + w.z );
}

Vector3 operator-(Vector3 const &v, Vector3 const &w)
{
  return Vector3( v.x - w.x, v.y - w.y, v.z - w.z );
}

Vector3 operator*(Vector3 const &v, float f)
{
  return Vector3( v.x * f, v.y * f, v.z * f);
}

Vector3 operator*(float f, Vector3 const &v)
{
  return Vector3( v.x * f, v.y * f, v.z * f);
}

Vector3 operator/(Vector3 const &v, float f)
{
  return Vector3( v.x / f, v.y / f, v.z / f);
}

Vector3 operator/(float f, Vector3 const &v)
{
  return Vector3( v.x / f, v.y / f, v.z / f);
}

Vector3 operator-(Vector3 const &v)
{
  return Vector3( -v.x , -v.y , -v.z );
}

