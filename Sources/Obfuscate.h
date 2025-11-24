// Obfustate.h
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once

#include "COMPILER_OPTIONS.h"
#include "stdio.h"
#include "stdlib.h" // for rand function

class Cypher
{
public:
  
  Cypher()
  {}

  Cypher(Cypher& a)
  {
    int v = a.get();
    set(v);
  }

  Cypher& operator=(Cypher& a)
  {
    int v = a.get();
    set(v);
    return *this;
  }

  static void printTable()
  {
    printf("= {");
    for ( int i=0; i<256; i++)
    {
      printf("%d", rand()%256);
      if (i<255) printf(", ");
      if (i%15 == 14) 
        printf("\n");
    }
    printf("};\n");
  }

  void set(int i)
  {  
    long long factor = (long long(&value_)) % (NB_VAL-11);
    int mix = (table_[factor+2]+(table_[factor+10]<<8)+(table_[factor+5]<<16)+(table_[factor+1]<<24));

    value_ = i ^ mix ;
  }


  int get() const
  {
    long long factor = (long long(&value_)) % (NB_VAL-11);
    int mix = (table_[factor+2]+(table_[factor+10]<<8)+(table_[factor+5]<<16)+(table_[factor+1]<<24));

    return value_ ^ mix;
  }

  void setf(float i)
  {
    int val = *( (int*) (&i) );
    set (val);
  }

  float getf() const
  {
    int vali=get();
    float valf = * ( (float * ) (&vali) );
    return valf;
  }

private:
  int value_;  

  static const int Cypher::NB_VAL = 256;
  static const unsigned char table_[];
};

#ifndef OBFUSCATED_VARIABLES_ARE_CLEAR

class obool
{
public:
  obool(bool i)
  {
    // I used the ? operator to remove a compiler warning
    cryptedValue_.set(i?1:0);
  }

  obool()
  {
  }

  obool (const obool& a) 
  {
    bool value = a.cryptedValue_.get()? true : false;
    cryptedValue_.set(value?1:0 );
  }
  operator bool() const
  {
    return cryptedValue_.get()? true : false;
  }

  const obool& operator =(const obool& a) 
  {
    bool value=a.cryptedValue_.get()? true : false;
    cryptedValue_.set(value?1:0 );
    return *this;
  }

private:
  Cypher cryptedValue_;
};

class oint
{
public:
  oint(int i)
  {
    cryptedValue_.set(i);
  }  

  oint()
  {
  }

  oint (const oint& a) 
  {
    int value=a.cryptedValue_.get();
    cryptedValue_.set(value );
  }
  operator int() const
  {
    return cryptedValue_.get();
  }

  const oint& operator =(const oint& a) 
  {
    int value=a.cryptedValue_.get();
    cryptedValue_.set(value );
    return *this;
  }

  oint operator ++() // pré
  {
    int value=cryptedValue_.get() + 1;
    cryptedValue_.set(value );
    return *this;
  }
  oint operator ++(int) // post
  {
    int value=cryptedValue_.get() + 1;
    cryptedValue_.set(value);
    return *this;
  }  
  
  oint operator +=(int v)
  {
    int value=cryptedValue_.get()+ v;
    cryptedValue_.set( value );
    return *this;
  }  

  oint operator --() // pré
  {
    int value=cryptedValue_.get()- 1;
    cryptedValue_.set( value );
    return *this;
  }
  oint operator --(int) // post
  {
    int value=cryptedValue_.get()- 1;
    cryptedValue_.set( value );
    return *this;
  }  
  
  oint operator -=(int v)
  {
    int value=cryptedValue_.get() - v;
    cryptedValue_.set( value );
    return *this;
  }  
  oint operator *=(int v)
  {
    int value=cryptedValue_.get() * v;
    cryptedValue_.set( value );
    return *this;
  }  
  oint operator /=(int v)
  {
    int value=cryptedValue_.get() / v;
    cryptedValue_.set( value );
    return *this;
  }  

private:
  Cypher cryptedValue_;
};

class ofloat
{
public:
  ofloat(float i)
  {
    cryptedValue_.setf(i);
  }  

  ofloat()
  {
  }

  ofloat (const ofloat& a) 
  {
    float value=a.cryptedValue_.getf();
    cryptedValue_.setf(value );
  }
  operator float() const
  {
    return cryptedValue_.getf();
  }

  const ofloat& operator =(const ofloat& a) 
  {
    float value=a.cryptedValue_.getf();
    cryptedValue_.setf(value );
    return *this;
  }

  ofloat operator ++() // pré
  {
    float value=cryptedValue_.getf() + 1;
    cryptedValue_.setf(value );
    return *this;
  }
  ofloat operator ++(int) // post
  {
    float value=cryptedValue_.getf() + 1.0f;
    cryptedValue_.setf(value);
    return *this;
  }  
  
  ofloat operator +=(float v)
  {
    float value=cryptedValue_.getf()+ v;
    cryptedValue_.setf( value );
    return *this;
  }  

  ofloat operator --() // pré
  {
    float value=cryptedValue_.getf()- 1.0f;
    cryptedValue_.setf( value );
    return *this;
  }
  ofloat operator --(int) // post
  {
    float value=cryptedValue_.getf()- 1.0f;
    cryptedValue_.setf( value );
    return *this;
  }  
  
  ofloat operator -=(float v)
  {
    float value=cryptedValue_.getf() - v;
    cryptedValue_.setf( value );
    return *this;
  }  
  ofloat operator *=(float v)
  {
    float value=cryptedValue_.getf() * v;
    cryptedValue_.setf( value );
    return *this;
  }    
  ofloat operator /=(float v)
  {
    float value=cryptedValue_.getf() / v;
    cryptedValue_.setf( value );
    return *this;
  }  

private:
  Cypher cryptedValue_;
};

#else
  typedef float ofloat;
  typedef int   oint  ;
  typedef bool  obool ;
#endif
  


