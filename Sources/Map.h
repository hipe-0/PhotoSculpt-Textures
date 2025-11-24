#pragma once

#include "Obfuscate.h"
#include "Includes.h"

#define TEMPLATE_K template <class K>

TEMPLATE_K
class Map
{
public:

  Map();
  Map(int newsizex, int newsizey);
  Map(const Map<K> &v);
  Map<K> & operator=(const Map<K> &v);
  ~Map();

  inline int getSizeX() const;
  inline int getSizeY() const;
  inline bool isEmpty() const;

  inline void set (int x,int y, const K & vect);
  inline K    get (int x,int y ) const ;

  inline K & operator()(int,int);       // Get element location with Write Access granted
  inline K & operator()(int,int) const; // Get element with read access only

  friend Map operator+ (const Map & a, const Map & b);
  friend Map operator+ (const Map & a, const K b     );
  friend Map operator- (const Map & a, const Map & b);
  friend Map operator- (const Map & a, const K b     );
    
  static void copyAt(Map& dest, const Map& src, int atX, int atY);
  static void repeatN(Map& map, int nb, int framesizex, int framesizey);

protected:
  K* data_;
  int sizex_,	sizey_;
};

#ifdef DISPLACEMENTMAP_DEBUG
  #define CHECK_WITHIN_RANGE(x,y, sizex_, sizey_)\
    if (x < 0 && x >= sizex_)\
      THROW_INTERNAL_ERROR( "Wrong Argument x out of range (in MAP_DEBUG )");\
    if (y < 0 && y >= sizey_)\
      THROW_INTERNAL_ERROR( "Wrong Argument y out of range  (in MAP_DEBUG )");
#else
  #define CHECK_WITHIN_RANGE(x,y, sizex_, sizey_)
#endif

TEMPLATE_K Map<K>::Map()
{
  sizex_ = sizey_ = 0 ;  data_ = NULL;
}

TEMPLATE_K Map<K>::Map(int newsizex, int newsizey)
{
  ASSERT (newsizex<=0 || newsizey<=0 ,
    "Incorrect parameters (Map())" );

  sizex_ = newsizex; sizey_ = newsizey;
  int size = sizex_ * sizey_ ;
  data_ = new K[size];

  if (data_ == NULL) 
    THROW_MEMORY_LIMIT_REACHED();
}

TEMPLATE_K Map<K>::Map(const Map<K> &v)
{
  sizex_ = 0;  sizey_ = 0;  data_ = NULL;

  if (v.sizex_>0 && v.sizey_>0 && v.data_ != NULL)
  {
    sizex_ = v.sizex_;
    sizey_ = v.sizey_;
    int size = sizex_ * sizey_;
    data_ = new K[size];
    
    if (data_==0) 
      THROW_MEMORY_LIMIT_REACHED();

    // copy the vdata into the object
    K * ptr= data_, * vptr= v.data_;
    for (int k=0; k<size; k++, ptr++,vptr++)
      *ptr = *vptr;
  }

}

TEMPLATE_K Map<K> & Map<K>::operator=(const Map<K> &v)
{
  // if not copying itself
  if (this != &v) 
  {
    if (data_ != NULL)
      delete[] data_;

    sizex_ = 0;    sizey_ = 0;   data_ = NULL;

    if (v.sizex_>0 && v.sizey_>0 && v.data_ != NULL)
    {
      sizex_ = v.sizex_;
      sizey_ = v.sizey_;

      int size = sizex_ * sizey_;
      data_ = new K[size];

      if (data_ == 0)
        THROW_MEMORY_LIMIT_REACHED();

      // copy the vdata into the object
      K * ptr = data_ , * vptr = v.data_;
      for (int k=0; k < size; k++, ptr++,vptr++)
        *ptr = *vptr;
    }
  }
  return *this;
}


TEMPLATE_K Map<K>::~Map()
{
  if (data_ != NULL)
    delete[] data_;

  sizex_ = 0;  sizey_ = 0;  data_ = NULL;
}

TEMPLATE_K inline int Map<K>::getSizeY() const
  {
    return sizey_;
  }

TEMPLATE_K inline int Map<K>::getSizeX() const
  {
    return sizex_;
  }


TEMPLATE_K inline K & Map<K>::operator()(int x, int y)
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  return data_[x + y * sizex_ ] ;
}

TEMPLATE_K inline K & Map<K>::operator()(int x, int y) const
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  return data_ [x + y * sizex_] ;
}


TEMPLATE_K inline void Map<K>::set (int x,int y, const K & vect)
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  data_[x + y * sizex_] = vect ;
}

TEMPLATE_K inline K Map<K>::get (int x,int y ) const 
{
  CHECK_WITHIN_RANGE(x,y, sizex_, sizey_);

  return data_[x + y * sizex_];
}

TEMPLATE_K inline bool Map<K>::isEmpty() const
{
  if ((sizex_<=1 && sizey_<=1) || data_ == NULL)
    return true;
  else
    return false;
}

TEMPLATE_K void Map<K>::copyAt(Map<K>& dest, const Map<K>& src, int destx, int desty)
{
  int srcsizex = src.getSizeX(), srcsizey = src.getSizeY();

  if (dest.getSizeX() - destx < srcsizex) srcsizex = dest.getSizeX() - destx;
  if (dest.getSizeY() - desty < srcsizey) srcsizey = dest.getSizeY() - desty;
  int sourcex = 0, sourcey = 0;
  if (destx < 0)
  {
    srcsizex += destx;
    sourcex  = -destx;
    destx    =  0    ;
  }
  if (desty < 0)
  {
    srcsizey += desty;
    sourcey  = -desty;
    desty    =  0    ;
  }

  if ( srcsizex<=0 || srcsizey<=0 ) return;

  for   (int y=0; y<srcsizey; y++)
    for (int x=0; x<srcsizex; x++)
      dest(destx+x,desty+y) = src(sourcex+x,sourcey+y);
}

TEMPLATE_K void Map<K>::repeatN(Map<K>& map, int nb, int framesizex, int framesizey)
{
  if (nb<=0)   return;

  Map<K> res (map.getSizeX()*nb+framesizex*2, 
           map.getSizeY()*nb+framesizey*2);

  int startx = 0, starty = 0;
  if (framesizex>0) startx = 1;
  if (framesizey>0) starty = 1;

  for   (int y=-starty; y<nb+starty; y++)
    for (int x=-startx; x<nb+startx; x++)
      Map<K>::copyAt(res, map, map.getSizeX()*x+framesizex, map.getSizeY()*y+framesizey);

  map = res ;
}
