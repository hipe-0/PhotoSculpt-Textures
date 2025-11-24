
#pragma once

#include <math.h>
#include "Round.h"

class Int3 
{
  public:
  Int3(int i)
  {
    set_(i);
  }  

  Int3()
  {
    val_[0] = val_[1] = val_[2] = 0;
  }

  Int3 (const Int3& a) 
  {
    int value=a.get_();
    set_(value );
  }
  operator int() const
  {
    return get_();
  }

  const Int3& operator =(const Int3& a) 
  {
    int value=a.get_();
    set_(value );
    return *this;
  }

  Int3 operator ++() // pré
  {
    int value=get_() + 1;
    set_(value );
    return *this;
  }
  Int3 operator ++(int) // post
  {
    int value=get_() + 1;
    set_(value);
    return *this;
  }  
  
  Int3 operator +=(int v)
  {
    int value=get_()+ v;
    set_( value );
    return *this;
  }  

  Int3 operator --() // pré
  {
    int value=get_()- 1;
    set_( value );
    return *this;
  }
  Int3 operator --(int) // post
  {
    int value=get_()- 1;
    set_( value );
    return *this;
  }  
  
  Int3 operator -=(int v)
  {
    int value=get_() - v;
    set_( value );
    return *this;
  }  
  Int3 operator *=(int v)
  {
    int value=get_() * v;
    set_( value );
    return *this;
  }  
  Int3 operator /=(int v)
  {
    int value = get_() / v;
    set_( value );
    return *this;
  }  

private:
  unsigned char val_[3];

  int get_() const
  {
    int r = val_[2] << 16 | val_[1] << 8 | val_[0];

    // is this a negative value?
    if (val_[2] & 0x80)
      r |= 0x0FF000000 ;

    return r;
  }
  void set_(int i)
  {
    val_[0] = i & 0xFF;
    val_[1] = (i>>8) & 0xFF;
    val_[2] = (i>>16) & 0xFF;
    unsigned char unused = (i>>24) & 0xFF;
  }
};