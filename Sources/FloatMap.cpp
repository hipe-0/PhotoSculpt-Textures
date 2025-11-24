/*
 * FloatMap.cpp
 *
 *  Created on: 22 april 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "FloatMap.h"
#include "Includes.h"
#include "Bicubic.h"

FloatMap::FloatMap():Map<float>()
{
}

FloatMap::FloatMap(int newsizex, int newsizey) : Map<float>(newsizex, newsizey)
{
}

FloatMap::FloatMap(const FloatMap& v):Map<float>(v)
{
}

FloatMap FloatMap::operator/(float value) const
{	
  return (*this) * (1 / value); 
}

FloatMap FloatMap::operator*(float value) const
{
  //Defines the new table
  FloatMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in operator*())");

  long int size = (long int) sizex_ * sizey_;
  float * ptr  = data_         ;
  float * nptr = newtable.data_;

  for (long int k = 0 ; k < size ; k++)
  {
    *nptr = (*ptr) * value ;
    ptr++ ;
    nptr++;
  }

  return newtable;
}

FloatMap FloatMap::getMultiply(float valuex, float valuey) const
{
  //Defines the new table
  FloatMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getMultiply())");

  int size   = sizex_*sizey_;
  float * ptr  = data_;
  float * nptr = newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr = (*ptr) * valuex;
    ptr++;
    nptr++;
  }

  return newtable;
}


FloatMap FloatMap::getResize(int nsizex, int nsizey) const
{
  ASSERT (nsizex<=0 || nsizey<=0, "Incorrect parameters (getResize())" );
  ASSERT (sizex_==1 || sizey_==1, "incorrect map of size 1 (in getResize())");
  ASSERT (nsizex<=0 || nsizey<=0 ,"Incorrect parameters (getResize())" );

  //Defines the new table
  FloatMap newtable(nsizex,nsizey);

  int osizex=getSizeX();
  int osizey=getSizeY();

  float ratiox=float(nsizex-1)/float(osizex-1);
  float ratioy=float(nsizey-1)/float(osizey-1);

  // upsizing/downsizing the table
  for (int y=0;y<nsizey;y++)
    for (int x=0;x<nsizex;x++) {

      float ox=float(x)/ratiox*.9999f;
      float oy=float(y)/ratioy*.9999f;

      // linear interpolation is computed
      int oxround=int(ox);
      int oyround=int(oy);

      //on interpole le tableau de correspondances
      // those values are between 0 and 1
      // they correspond to the position between the square of 4 values
      float fx=ox-float(oxround);
      float fy=oy-float(oyround);

      // gets all the 4 corner values of the square on the old map that we're pointing at
      // 00=upper left corner, 01=upper right corner, 10= lower left, 11= lower right
      float x00=(this->operator())(oxround  ,oyround  );
      float x01=(this->operator())(oxround+1,oyround  );
      float x10=(this->operator())(oxround  ,oyround+1);
      float x11=(this->operator())(oxround+1,oyround+1);

      // Here are the interpolated values
      float dispx= (x00*(1.0f-fx)+x01*fx)*(1.0f-fy) + (x10*(1.0f-fx)+x11*fx)*fy;

      // we store the interpolated values in the resulting new displacement map
      newtable(x,y)=dispx;
    }

  return newtable;
}

FloatMap FloatMap::getResizeAndMultiply(int nsizex, int nsizey) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getResizeAndMultiply())");
  ASSERT (nsizex<=0 || nsizey<=0, "Incorrect parameters (getResizeAndMultiply())" );

  int osizex=getSizeX();
  int osizey=getSizeY();

  // ratio >1.0 when size get bigger
  float ratiox=float(nsizex-1)/float(osizex-1);
  float ratioy=float(nsizey-1)/float(osizey-1);

  if (ratiox>1.0f)
    return getMultiply(ratiox, ratioy).getResize(nsizex, nsizey);
  else
    return getResize(nsizex, nsizey).getMultiply(ratiox, ratioy);
}

void FloatMap::fill (float value)
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in fill())");

  int size = (int) sizex_ * sizey_;

  float * ptr = data_;
  for (int k=0; k < size; k++, ptr++)
    *ptr = value; 
}


FloatMap operator+(const FloatMap & a, const FloatMap & b)
{
  ASSERT (a.sizex_ != b.sizex_ || a.sizey_ != b.sizey_, 
          "2 Maps have different sizes and cannot be added (in FloatMap operator+ () )" );

  //Defines the new table
  FloatMap newtable(a.sizex_,a.sizey_);

  int         size  = a.sizex_ * a.sizey_;
  const float *ptra = a.data_ ;
  const float *ptrb = b.data_ ;
  float       *nptr = newtable.data_ ;

  for (int k=0; k<size; k++)
  {
    *nptr = *ptra + *ptrb;

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

FloatMap operator+(const FloatMap & a, const float b)
{
  //Defines the new table
  FloatMap newtable(a.sizex_,a.sizey_);

  int         size  = a.sizex_ * a.sizey_ ;
  const float *ptra = a.data_             ;
  float       *nptr = newtable.data_      ;

  for (int k=0; k<size; k++)
  {
    *nptr = *ptra + b;

    ptra++;
    nptr++;
  }

  return newtable;
}

FloatMap operator-(const FloatMap & a, const FloatMap & b)
{
  ASSERT (a.sizex_ != b.sizex_ || a.sizey_ != b.sizey_,
           "2 Maps have different sizes and cannot be substracted (in FloatMap operator- () )" );

  //Defines the new table
  FloatMap newtable(a.sizex_,a.sizey_);

  int         size  = a.sizex_ * a.sizey_ ;
  const float *ptra = a.data_ ;
  const float *ptrb = b.data_ ;
  float       *nptr = newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr = *ptra - *ptrb;

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

FloatMap operator-(const FloatMap & a, float b)
{
  //Defines the new table
  FloatMap newtable(a.sizex_,a.sizey_);

  int         size  = a.sizex_ * a.sizey_ ;
  const float *ptra = a.data_ ;
  float       *nptr = newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr = *ptra - b;

    ptra++;    nptr++;
  }

  return newtable;
}
