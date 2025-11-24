/* VectorMap.cpp
 *
 *  Created on: 29 janv. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "VectorMap.h"
#include "Round.h"
#include "Includes.h"

VectorMap::VectorMap()
{
  sizex_ = sizey_ = 0 ;
  data_ = NULL;
}

VectorMap::VectorMap(int newsizex, int newsizey)
{
  ASSERT (newsizex<=0 || newsizey<=0, "Incorrect parameters (VectorMap())" );

  sizex_ = newsizex; sizey_ = newsizey;
  int size = sizex_ * sizey_ ;
  data_ = new Vector[size];

  if (data_ == NULL) 
    THROW_MEMORY_LIMIT_REACHED();
}

VectorMap::VectorMap(const VectorMap &v)
{
  sizex_ = 0;  sizey_ = 0;  data_ = NULL;

  if ((v.sizex_>0 || v.sizey_>0) && v.data_ != NULL)
  {
    sizex_ = v.sizex_;
    sizey_ = v.sizey_;
    int size = sizex_ * sizey_;
    data_ = new Vector[size];
    
    if (data_==0) 
      THROW_MEMORY_LIMIT_REACHED();

    // copy the vdata into the object
    Vector * ptr = data_.getPointer();
    const Vector * vptr= v.data_.getPointer();
    for (int k=0; k<size; k++, ptr++,vptr++)
      *ptr = *vptr;
  }
}

VectorMap & VectorMap::operator=(const VectorMap &v)
{
  ASSERT( this == NULL, "VectorMap not created with new() in (operator=())");

  // if not copying itself
  if (this != &v) 
  {
    sizex_ = 0;    
    sizey_ = 0;   
    data_ = NULL;

    if ((v.sizex_>0 || v.sizey_>0) && v.data_ != NULL)
    {
      sizex_ = v.sizex_;
      sizey_ = v.sizey_;

      int size = sizex_ * sizey_;
      data_ = new Vector[size];

      if (data_ == 0)
        THROW_MEMORY_LIMIT_REACHED();

      // copy the vdata into the object
      Vector * ptr = data_.getPointer();
      const Vector * vptr = v.data_.getPointer();
      for (int k=0; k < size; k++, ptr++,vptr++)
        *ptr = *vptr;
    }
  }
  return *this;
}


VectorMap::~VectorMap()
{
  sizex_ = 0;  sizey_ = 0;  data_ = NULL;
}

void VectorMap::fill(int i)
{
  int size = sizex_ * sizey_ ;

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in fill())");

  Vector * ptr = data_.getPointer();
  for (int k=0; k<size; k++, ptr++)
    *ptr = Vector(i,i);
}

void VectorMap::fill(Vector vector)
{
  int size = sizex_ * sizey_ ;

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in fill())");

  Vector * ptr = data_.getPointer();
  for (int k=0; k<size; k++, ptr++)
    *ptr = vector;
}

void VectorMap::clear()
{
  sizex_ = 0;  sizey_ = 0;  data_ = NULL;
}

int VectorMap::getSizeX() const
{
  return sizex_;
}

int VectorMap::getSizeY() const
{
  return sizey_;
}

bool VectorMap::isEmpty() const
{
  if ((sizex_<=1 && sizey_<=1) || data_ == NULL)
    return true;
  else
    return false;
}

VectorMap VectorMap::operator/(float value) const
{	
  return (*this) * ( 1 / value ); 
}

VectorMap VectorMap::operator*(float value) const
{
  //Defines the new table
  VectorMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in operator*())");

  int size  = sizex_*sizey_;

  const Vector * ptr = data_.getPointer();
  Vector * nptr= newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    int i=round(float(ptr->x()) * value);
    int j=round(float(ptr->y()) * value);
    *nptr = Vector(i,j);
    ptr++;
    nptr++;
  }

  return newtable;
}

VectorMap VectorMap::getMultiply(float valuex, float valuey) const
{
  //Defines the new table
  VectorMap newtable(sizex_,sizey_);

  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getMultiply())");

  int size   = sizex_*sizey_;

  const Vector * ptr  = data_.getPointer();
  Vector * nptr = newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    int i = round(float(ptr->x()) * valuex);
    int j = round(float(ptr->y()) * valuey);
    *nptr     = Vector(i,j);
    ptr++;
    nptr++;
  }

  return newtable;
}

VectorMap VectorMap::getCrop(int x1, int y1, int x2, int y2) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getCrop())");

  int nsizex=x2-x1+1;
  int nsizey=y2-y1+1;

  //Defines the new table
  VectorMap newtable(nsizex,nsizey);

  for (int y=0; y<nsizey; y++)
    for (int x=0; x<nsizex; x++) 
      newtable.set(x,y, get (x1+x,y1+y));
  
  return newtable;
}


VectorMap operator+(const VectorMap & a, const VectorMap & b)
{
  ASSERT (a.sizex_ != b.sizex_ || a.sizey_ != b.sizey_,
    "2 Maps have different sizes and cannot be added (in VectorMap operator+ () )" );

  //Defines the new table
  VectorMap newtable(a.sizex_,a.sizey_);

  int size = a.sizex_ * a.sizey_;
  const Vector *ptra = a.data_.getPointer() ;
  const Vector *ptrb = b.data_.getPointer();
  Vector *nptr = newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    int i = (ptra->x()) + (ptrb->x());
    int j = (ptra->y()) + (ptrb->y());
    *nptr = Vector (i,j);

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

VectorMap operator+(const VectorMap & a, const Vector b)
{
  //Defines the new table
  VectorMap newtable(a.sizex_,a.sizey_);

  int size = a.sizex_ * a.sizey_;
  const Vector *ptra = a.data_.getPointer() ;
  Vector *nptr = newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    int i = ptra->x() + b.x();
    int j = ptra->y() + b.y();
    *nptr = Vector (i,j);

    ptra++;
    nptr++;
  }

  return newtable;
}

VectorMap operator-(const VectorMap & a, const VectorMap & b)
{
  ASSERT (a.sizex_ != b.sizex_ || a.sizey_ != b.sizey_,
    "2 Maps have different sizes and cannot be substracted (in VectorMap operator- () )" );

  //Defines the new table
  VectorMap newtable(a.sizex_,a.sizey_);

  int size = a.sizex_ * a.sizey_ ;
  const Vector *ptra = a.data_.getPointer() ;
  const Vector *ptrb = b.data_.getPointer();
  Vector *nptr = newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    int i = (ptra->x()) - (ptrb->x());
    int j = (ptra->y()) - (ptrb->y());
    *nptr = Vector (i,j);

    ptra++;
    ptrb++;
    nptr++;
  }

  return newtable;
}

VectorMap operator-(const VectorMap & a, Vector b)
{
  //Defines the new table
  VectorMap newtable(a.sizex_,a.sizey_);

  int size = a.sizex_ * a.sizey_ ;
  const Vector *ptra = a.data_.getPointer() ;
  Vector *nptr = newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    int i = ptra->x() - b.x();
    int j = ptra->y() - b.y();
    *nptr = Vector (i,j);

    ptra++;    nptr++;
  }

  return newtable;
}

VectorMap VectorMap::operator-() const
{
  //Defines the new table
  VectorMap newtable(sizex_,sizey_);

  int size = sizex_*sizey_ ;

  const Vector * source = data_         .getPointer();
  Vector * destination  = newtable.data_.getPointer();

  for (int k=0; k<size; k++)
  {
    *destination =  - *source ;

    source++; destination++;
  }

  return newtable;
}


VectorMap VectorMap::getResize(int nsizex, int nsizey) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in getResize())");
  ASSERT (sizex_==1 || sizey_==1, "incorrect map of size 1 (in getResize())");
  ASSERT (nsizex<=0 || nsizey<=0, "Incorrect parameters (getResize())" );

  if (nsizex == sizex_ && nsizey == sizey_)
    return *this;
  else if (nsizex > sizex_) // get bigger
    return computeUpsize(nsizex,nsizey);
  else
    return computeDownsize(nsizex,nsizey);
}

VectorMap VectorMap::computeUpsize(int nsizex, int nsizey) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in computeUpsize())");
  ASSERT ((sizex_==1 || sizey_==1), "incorrect map of size 1 (in computeUpsize())");
  ASSERT (nsizex<=0 || nsizey<=0,   "Incorrect parameters (computeUpsize())" );
  ASSERT (nsizex<=sizex_ && nsizey<=sizey_, "Bigger dimensions are expected (in computeUpsize())");

  //Defines the new table
  VectorMap newtable(nsizex,nsizey);

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
      float x00 = float( this->x(oxround  ,oyround  ) );
      float x01 = float( this->x(oxround+1,oyround  ) );
      float x10 = float( this->x(oxround  ,oyround+1) );
      float x11 = float( this->x(oxround+1,oyround+1) );

      // same for y interpolation
      float y00 = float( this->y(oxround  ,oyround  ) );
      float y01 = float( this->y(oxround+1,oyround  ) );
      float y10 = float( this->y(oxround  ,oyround+1) );
      float y11 = float( this->y(oxround+1,oyround+1) );

      // Here are the interpolated values
      float dispx= (x00*(1.0f-fx)+x01*fx)*(1.0f-fy) + (x10*(1.0f-fx)+x11*fx)*fy;
      float dispy= (y00*(1.0f-fy)+y10*fy)*(1.0f-fx) + (y01*(1.0f-fy)+y11*fy)*fx;

      // we store the interpolated values in the resulting new displacement map
      newtable.set(x,y, int(round(dispx)),int(round(dispy)) );
    }

  return newtable;
}

inline float mix (float x, float A0, float A1)
{
  return A0 + x * (A1 - A0);
}

inline float mix (float x, float y, float A00, float A10, float A01, float A11)
{
  float u = mix (x, A00, A10);
  float d = mix (x, A01, A11);
  return u + y * (d - u);
}

#include "IntMap.h"

VectorMap VectorMap::computeDownsize(int nsizex, int nsizey) const
{
  ASSERT ((sizex_<=0 && sizey_<=0) || data_ == NULL, "empty map (in computeDownsize())");
  ASSERT (sizex_==1 || sizey_==1,      "incorrect map of size 1 (in computeDownsize())");
  ASSERT (nsizex<=0 || nsizey<=0,      "Incorrect parameters (computeDownsize())"      );
  ASSERT (nsizex>=sizex_ && nsizey>=sizey_, "Smaller dimensions are expected (in computeDownsize())");

  //Defines the new table
  VectorMap newtable(nsizex,nsizey);
  newtable.fill(Vector(0,0));

  IntMap numberofvalues(nsizex,nsizey);
  numberofvalues.fill(0);

  int osizex = this->sizex_;
  int osizey = this->sizey_;

  float ratiox = float(nsizex-1) / float(osizex-1); // ratio should be < 1.0
  float ratioy = float(nsizey-1) / float(osizey-1);

  // upsizing/downsizing the table
  for   (int oy = 0 ; oy < osizey ; oy ++)
    for (int ox = 0 ; ox < osizex ; ox ++) 
    {
      int valuex = this->x( ox ,oy );
      int valuey = this->y( ox ,oy );

      int nx = round (float(ox) * ratiox );
      int ny = round (float(oy) * ratioy );

      numberofvalues(nx,ny) ++ ;
      Vector previousvalue = newtable.get(nx,ny);
      newtable.set(nx,ny, previousvalue+Vector(valuex, valuey));
    }

  for   (int ny = 0 ; ny < nsizey ; ny ++)
    for (int nx = 0 ; nx < nsizex ; nx ++) 
    {
      Vector value = newtable.get(nx,ny);
      int number = numberofvalues(nx,ny);
      newtable.set(nx,ny, value / number);
    }

  numberofvalues = IntMap(); // free memory

  return newtable;
}

VectorMap VectorMap::getResizeAndMultiply(int nsizex, int nsizey) const
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

int VectorMap::getCoordX(int x, int y) const
{
  return this->x(x,y) / MULTIPLY + x ;
}

float VectorMap::getCoordXFloat(int x, int y) const
{
  return float(this->x(x,y)) / float(MULTIPLY) + float(x) ;
}

int VectorMap::getCoordY(int x, int y) const
{
  return this->y(x,y) / MULTIPLY + y ;
}

float VectorMap::getCoordYFloat(int x, int y) const
{
  return float(this->y(x,y)) / float(MULTIPLY) + float(y) ;
}
