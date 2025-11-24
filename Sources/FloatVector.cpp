/*
 * FloatVector.cpp
 *
 *  Created on: 12/12/2010
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "FloatVector.h"
#include "Round.h"
#include "Includes.h"


FloatVector::FloatVector()
{
  size_ = 0 ;
  data_ = NULL;
}

FloatVector::FloatVector(int newsize)
{
  ASSERT (newsize<=0, "Incorrect parameter (FloatVector())" );

  size_ = newsize;   data_ = new float[size_];  if (data_ == NULL)  THROW_MEMORY_LIMIT_REACHED();
}

FloatVector::FloatVector(float f1)
{
  size_ = 1; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1;
}

FloatVector::FloatVector(float f1, float f2)
{
  size_ = 2; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2;
}

FloatVector::FloatVector(float f1, float f2, float f3)
{
  size_ = 3; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4)
{
  size_ = 4; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4, float f5)
{
  size_ = 5; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4; data_[4] = f5;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4, float f5, float f6)
{
  size_ = 6; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4; data_[4] = f5; data_[5] = f6;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7)
{
  size_ = 7; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4; data_[4] = f5; data_[5] = f6; data_[6] = f7;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8)
{
  size_ = 8; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4; data_[4] = f5; data_[5] = f6; data_[6] = f7; data_[7] = f8;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9)
{
  size_ = 9; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4; data_[4] = f5; data_[5] = f6; data_[6] = f7; data_[7] = f8; data_[8] = f9;
}
FloatVector::FloatVector(float f1, float f2, float f3, float f4, float f5, float f6, float f7, float f8, float f9, float f10)
{
  size_ = 10; data_ = new float[size_]; if (data_ == NULL) THROW_MEMORY_LIMIT_REACHED();
  data_[0] = f1; data_[1] = f2; data_[2] = f3; data_[3] = f4; data_[4] = f5; data_[5] = f6; data_[6] = f7; data_[7] = f8; data_[8] = f9; data_[9] = f10;
}


FloatVector::FloatVector(const FloatVector &v)
{
  size_ = 0;    data_ = NULL;

  if ((v.size_>0 ) && v.data_ != NULL)
  {
    size_ = v.size_;
    
    data_ = new float[size_];
    
    if (data_==0) 
      THROW_MEMORY_LIMIT_REACHED();

    // copy the vdata into the object
    float * ptr = data_.getPointer();
    const float * vptr= v.data_.getPointer();
    for (int k=0; k<size_; k++, ptr++,vptr++)
      *ptr = *vptr;
  }
}


FloatVector & FloatVector::operator=(const FloatVector &v)
{
  ASSERT ( this == NULL, "FloatVector not created with new() in (operator=())");

  // if not copying itself
  if (this != &v) 
  {
    //if (data_ != NULL)
      //delete[] data_;

    size_ = 0;    
    data_ = NULL;

    if ((v.size_>0 ) && v.data_ != NULL)
    {
      size_ = v.size_;

      data_ = new float[size_];

      if (data_ == 0)
        THROW_MEMORY_LIMIT_REACHED();

      // copy the vdata into the object
      float * ptr = data_.getPointer();
      const float * vptr = v.data_.getPointer();
      for (int k=0; k < size_; k++, ptr++,vptr++)
        *ptr = *vptr;
    }
  }
  return *this;
}

FloatVector::~FloatVector()
{
  size_ = 0; 
  data_ = NULL;
}

void FloatVector::set (int x, float value)
{
  FLOATVECTOR_CHECK_WITHIN_RANGE(x, size_);

  data_[x ] = value ;
}

float FloatVector::get (int x) const 
{
  FLOATVECTOR_CHECK_WITHIN_RANGE(x, size_);

  return data_[x] ;
}

float FloatVector::operator[] (int x)       const
{
  FLOATVECTOR_CHECK_WITHIN_RANGE(x, size_);

  return data_[x] ;
}

float& FloatVector::operator[] (int x)
{
  FLOATVECTOR_CHECK_WITHIN_RANGE(x, size_);

  return data_[x] ;
}

void FloatVector::fill(int value)
{
  ASSERT ((size_<=0 ) || data_ == NULL, "empty map (in fill())");

  float * ptr = data_.getPointer();
  for (int k=0; k<size_; k++, ptr++)
    *ptr = float(value);
}


void FloatVector::fill(float value)
{
  ASSERT ((size_<=0) || data_ == NULL, "empty map (in fill())");

  float * ptr = data_.getPointer();
  for (int k=0; k<size_; k++, ptr++)
    *ptr = value;
}


#include <stdlib.h>

void FloatVector::randomizeFrom0To1()
{
  ASSERT ((size_<=0) || data_ == NULL, "empty map (in fill())");

  float * ptr = data_.getPointer();
  for (int k=0; k<size_; k++, ptr++)
    *ptr = float(rand())/float(RAND_MAX);
}

void FloatVector::clear()
{
  size_ = 0;
  data_ = NULL;
}

int FloatVector::getSize() const
{
  return size_;
}

bool FloatVector::isEmpty() const
{
  if ((size_<=0) || data_ == NULL)
    return true;
  else
    return false;
}

FloatVector FloatVector::operator/(float value) const
{	
  return (*this) * ( 1.0f / value ); 
}

FloatVector FloatVector::operator*(float value) const
{
  FloatVector newtable(size_);

  ASSERT ((size_<=0 ) || data_ == NULL, "empty map (in operator*())");

  const float * ptr  = data_         .getPointer();
  float       * nptr = newtable.data_.getPointer();

  for (int k=0; k<size_; k++)
  {
    *nptr = *ptr * value;
    ptr++;
    nptr++;
  }

  return newtable;
}

FloatVector FloatVector::getCrop(int x1, int x2) const
{
  ASSERT (size_<=0  || data_ == NULL, "empty map (in getCrop())");

  int nsize=x2-x1+1;

  FloatVector newtable(nsize);

    for (int x=0; x<nsize; x++) 
      newtable.set(x, get (x1+x));
  
  return newtable;
}



FloatVector operator+(const FloatVector & a, const FloatVector & b)
{
  ASSERT (a.size_ != b.size_,
    "2 Maps have different sizes and cannot be added (in FloatVector operator+ () )" );

  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  const float *ptrb = b.data_.getPointer();
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    *nptr = *ptra + *ptrb;

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}


FloatVector operator+(const FloatVector & a, const float b)
{
  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    *nptr = *ptra + b;

    ptra++;
    nptr++;
  }

  return newtable;
}


FloatVector operator-(const FloatVector & a, const FloatVector & b)
{
  ASSERT (a.size_ != b.size_,
    "2 Maps have different sizes and cannot be substracted (in FloatVector operator- () )" );

  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  const float *ptrb = b.data_.getPointer();
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    *nptr = *ptra - *ptrb;

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}


FloatVector operator-(const FloatVector & a, float b)
{
  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    *nptr = *ptra - b;

    ptra++;    
    nptr++;
  }

  return newtable;
}


FloatVector operator*(const FloatVector & a, const FloatVector & b)
{
  ASSERT (a.size_ != b.size_,
    "2 Maps have different sizes and cannot be added (in FloatVector operator+ () )" );

  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  const float *ptrb = b.data_.getPointer();
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    *nptr = (*ptra) * (*ptrb);

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

FloatVector getMinimum(const FloatVector & a, float b)
{
  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    if ((*ptra) < b)
      *nptr = (*ptra) ;
    else
      *nptr = b ;

    ptra++;    
    nptr++;
  }

  return newtable;
}

FloatVector getMaximum(const FloatVector & a, float b)
{
  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    if ((*ptra) > b)
      *nptr = (*ptra) ;
    else
      *nptr = b ;

    ptra++;    
    nptr++;
  }

  return newtable;
}

FloatVector getMinimum(const FloatVector & a, const FloatVector & b)
{
  ASSERT (a.size_ != b.size_,
    "2 Maps have different sizes and cannot be added (in FloatVector operator+ () )" );

  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  const float *ptrb = b.data_.getPointer();
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    if ((*ptra) < (*ptrb))
      *nptr = (*ptra) ;
    else
      *nptr = (*ptrb) ;

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

FloatVector getMaximum(const FloatVector & a, const FloatVector & b)
{
  ASSERT (a.size_ != b.size_,
    "2 Maps have different sizes and cannot be added (in FloatVector operator+ () )" );

  FloatVector newtable(a.size_);

  const float *ptra = a.data_.getPointer() ;
  const float *ptrb = b.data_.getPointer();
  float *nptr = newtable.data_.getPointer();

  for (int k=0; k<a.size_; k++)
  {
    if ((*ptra) > (*ptrb))
      *nptr = (*ptra) ;
    else
      *nptr = (*ptrb) ;

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

FloatVector FloatVector::operator-() const
{
  FloatVector newtable(size_);

  const float * source = data_         .getPointer();
  float * destination  = newtable.data_.getPointer();

  for (int k=0; k<size_; k++)
  {
    *destination =  - *source ;

    source++; 
    destination++;
  }

  return newtable;
}

FloatVector FloatVector::getAbsolute() const
{
  FloatVector newtable(size_);

  const float * source = data_         .getPointer();
  float * destination  = newtable.data_.getPointer();

  for (int k=0; k<size_; k++)
  {
    if (*source >= 0.0f)
      *destination = *source ;
    else
      *destination = - *source ;

    source++; 
    destination++;
  }

  return newtable;
}

FloatVector FloatVector::getSum() const
{
  float sum = 0.0f ;

  const float * source = data_.getPointer();

  for (int k=0; k<size_; k++)
  {
    sum += *source ;

    source++; 
  }

  return sum;
}

FloatVector FloatVector::getMean() const
{
  if (size_ == 0) 
    return 0.0f;

  float sum = 0.0f ;

  const float * source = data_.getPointer();

  for (int k=0; k<size_; k++)
  {
    sum += *source ;

    source++; 
  }

  return sum / size_ ;
}

float FloatVector::getLength() const
{
  float sum = 0.0f ;

  const float * source = data_.getPointer();

  for (int k=0; k<size_; k++)
  {
    sum += (*source) * (*source) ;

    source++; 
  }

  return sqrtf(sum);
}

FloatVector FloatVector::getResize(int nsize) const
{
  ASSERT ((size_<=0 ) || data_ == NULL, "empty map (in getResize())");
  ASSERT (size_==1,                     "incorrect map of size 1 (in getResize())");
  ASSERT (nsize<=0,                     "Incorrect parameters (getResize())" );

  FloatVector newtable(nsize);

  int         osize  = getSize();
  float       ratiox = float(nsize-1)/float(osize-1);

  // upsizing/downsizing the table
  for (int x = 0 ; x < nsize ; x++) {

    float ox=float(x)/ratiox*.9999f;

    // linear interpolation is computed
    int oxround=int(ox);

    //on interpole le tableau de correspondances
    // those values are between 0 and 1
    // they correspond to the position between the square of 4 values
    float fx=ox-float(oxround);

    // gets all the 4 corner values of the square on the old map that we're pointing at
    // 00=upper left corner, 01=upper right corner, 10= lower left, 11= lower right
    float x00 = float( this->get(oxround  ) );
    float x01 = float( this->get(oxround+1) );

    // Here are the interpolated values
    float dispx= x00 * ( 1.0f - fx ) + x01 * fx;

    // we store the interpolated values in the resulting new displacement map
    newtable.set(x, dispx );
  }

  return newtable;
}

void FloatVector::writeToString(char* text, int buffersize) const
{
  const float * source = data_.getPointer();
  
  strcat_s(text, buffersize, "{");

  char textval[20]="";
  for (int k = 0; k < size_; k++)
  {
    sprintf_s(textval, 20, "%f", *source) ;
    strcat_s (text, buffersize, textval);
    source++;

    if (k<size_-1) strcat_s(text, buffersize, " , ");
  }

  strcat_s(text, buffersize, "}");
}
