/*
 * IntMap.cpp
 *
 *  Created on: 21 dec. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "IntMap.h"
#include "Round.h"
#include "Includes.h"

IntMap::IntMap():Map<int>() {}

IntMap::IntMap(int newsizex, int newsizey) : Map<int>(newsizex, newsizey) {}

IntMap::IntMap(const IntMap &v):Map<int>(v) {}

IntMap IntMap::operator/(float value) const
{	
  return (*this) * ( 1 / value ); 
}

IntMap IntMap::operator*(float value) const
{
  //Defines the new table
  IntMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in operator*())");

  int size  = sizex_ * sizey_;
  int * ptr = data_;
  int * nptr= newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr = round(float(*ptr) * value);
    ptr++;
    nptr++;
  }

  return newtable;
}

IntMap IntMap::operator+(float value) const
{
  //Defines the new table
  IntMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in operator*())");

  int size  = sizex_ * sizey_;
  int * ptr = data_;
  int * nptr= newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr = round(float(*ptr) + value);
    ptr++;
    nptr++;
  }

  return newtable;
}

IntMap IntMap::operator-(float value) const
{
  return operator+(-value);
}


IntMap IntMap::getMultiply(float value) const
{
  //Defines the new table
  IntMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getMultiply())");

  int size   = sizex_*sizey_;
  int * ptr  = data_;
  int * nptr = newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr     = round(float(*ptr) * value);
    ptr++;
    nptr++;
  }

  return newtable;
}

IntMap IntMap::getCrop(int x1, int y1, int x2, int y2) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL,
    "empty map (in getCrop())");

  int nsizex=x2-x1+1;
  int nsizey=y2-y1+1;

  //Defines the new table
  IntMap newtable(nsizex,nsizey);

  for (int y=0; y<nsizey; y++)
    for (int x=0; x<nsizex; x++) 
    {
      int value = operator() (x1+x,y1+y);
      newtable(x,y) = value;
    }
  
  return newtable;
}


IntMap operator+(const IntMap & a, const IntMap & b)
{
  ASSERT (a.sizex_ != b.sizex_ || a.sizey_ != b.sizey_,
    "2 Maps have different sizes and cannot be added (in VectorMap operator+ () )" );

  //Defines the new table
  IntMap newtable(a.sizex_,a.sizey_);

  int size = a.sizex_ * a.sizey_;
  int * ptra = a.data_;
  int * ptrb = b.data_;
  int * nptr = newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr=(*ptra) + (*ptrb);

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

IntMap operator-(const IntMap & a, const IntMap & b)
{
  ASSERT (a.sizex_ != b.sizex_ || a.sizey_ != b.sizey_,
    "2 Maps have different sizes and cannot be substracted (in IntMap operator- () )" );

  //Defines the new table
  IntMap newtable(a.sizex_,a.sizey_);

  int size= a.sizex_ * a.sizey_ ;
  int * ptra = a.data_;
  int * ptrb = b.data_;
  int * nptr = newtable.data_;

  for (int k=0; k<size; k++)
  {
    *nptr = (*ptra) - (*ptrb);

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

IntMap IntMap::operator-() const
{
  //Defines the new table
  IntMap newtable(sizex_,sizey_);

  int size = sizex_*sizey_ ;

  int * nptr   = newtable.data_;
  int * srcptr = data_ ;

  for (int k=0; k<size; k++)
  {
    *nptr =  - *srcptr ;

    nptr++; srcptr++;
  }

  return newtable;
}


IntMap IntMap::getResize(int nsizex, int nsizey) const
{
  ASSERT (nsizex<=0 || nsizey<=0, "Incorrect parameters (getResize())"       );
  ASSERT (sizex_==1 || sizey_==1, "incorrect map of size 1 (in getResize())" );
  ASSERT (nsizex<=0 || nsizey<=0, "Incorrect parameters (getResize())"       );

  //Defines the new table
  IntMap newtable(nsizex,nsizey);

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
      float x00=float((this->operator())(oxround  ,oyround  ));
      float x01=float((this->operator())(oxround+1,oyround  ));
      float x10=float((this->operator())(oxround  ,oyround+1));
      float x11=float((this->operator())(oxround+1,oyround+1));

      // Here are the interpolated values
      float dispx= (x00*(1.0f-fx)+x01*fx)*(1.0f-fy) + (x10*(1.0f-fx)+x11*fx)*fy;

      // we store the interpolated values in the resulting new displacement map
      newtable(x,y) = int(round(dispx));
    }

  return newtable;
}

IntMap IntMap::getResizeAndMultiply(int nsizex, int nsizey) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getResizeAndMultiply())");
  ASSERT (nsizex<=0 || nsizey<=0,           "Incorrect parameters (getResizeAndMultiply())" );

  int osizex=getSizeX();
  int osizey=getSizeY();

  // ratio >1.0 when size get bigger
  float ratiox=float(nsizex-1)/float(osizex-1);

  if (ratiox>1.0f)
    return getMultiply(ratiox).getResize(nsizex, nsizey);
  else
    return getResize(nsizex, nsizey).getMultiply(ratiox);
}

IntMap& IntMap::fill (int value)
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in fill())");

  int size = sizex_ * sizey_;

  int * ptr = data_;
  for (int k=0; k < size; k++, ptr++)
    *ptr = value; 

  return *this;
}
