// copied from http://www.game-lab.com/index.php?section=tutorials&section_id=1&p=tutorial&action=showtut&id=243
//
#pragma once

#define TEMPLATE_TA template <typename T,bool Array> 
#define SMARTPTR SmartPointer<T,Array>

template <typename T, bool Array = false>
class SmartPointer
{
public:
  inline SmartPointer();
  inline SmartPointer(const SmartPointer& SmartPtr);
  inline SmartPointer(T* Pointer);
  inline ~SmartPointer();

  inline T& operator*  ();
  inline T* operator-> ();
  inline T& operator[] (int x);

  inline const T& operator*  ()      const;
  inline const T* operator-> ()      const;
  inline const T& operator[] (int x) const;

  inline bool operator==(const SmartPointer& SmartPtr) const;
  inline bool operator!=(const SmartPointer& SmartPtr) const;

  inline SmartPointer& operator=(const SmartPointer& SmartPtr);
  inline T*       getPointer()      ;
  inline const T* getPointer() const;

  inline SmartPointer& operator=(T* Pointer);

private:
  inline void _Release();

  int* _RefCounter;
  T* _Pointer;
};

TEMPLATE_TA inline T& SMARTPTR::operator*()
{
  return *_Pointer;
}

TEMPLATE_TA inline T* SMARTPTR::operator->()
{
  return _Pointer;
}

TEMPLATE_TA inline const T& SMARTPTR::operator*() const
{
  return *_Pointer;
}

TEMPLATE_TA inline const T* SMARTPTR::operator->() const
{
  return _Pointer;
}

TEMPLATE_TA inline bool SMARTPTR::operator==(const SMARTPTR& SmartPtr) const
{
  return _Pointer == SmartPtr._Pointer;
}

TEMPLATE_TA inline bool SMARTPTR::operator!=(const SMARTPTR& SmartPtr) const
{
  return _Pointer != SmartPtr._Pointer;
}

TEMPLATE_TA inline SMARTPTR::SmartPointer() :
    _RefCounter(0), _Pointer(0)
{}


TEMPLATE_TA inline SMARTPTR::SmartPointer(T* Pointer) :
    _Pointer(Pointer)
{
  _RefCounter = new int(1);
}

TEMPLATE_TA inline SMARTPTR::SmartPointer(const SMARTPTR& SmartPtr) :
    _RefCounter(SmartPtr._RefCounter),
    _Pointer(SmartPtr._Pointer)
{
  if(_RefCounter != 0)
    ++(*_RefCounter);
}

TEMPLATE_TA inline SMARTPTR& SMARTPTR::operator=(T* Pointer)
{
  _Release();

  _Pointer = Pointer;
  _RefCounter = new int(1);

  return *this;
}

TEMPLATE_TA inline SMARTPTR& SMARTPTR::operator=(const SMARTPTR& SmartPtr)
{
  _Release();

  _Pointer = SmartPtr._Pointer;
  _RefCounter = SmartPtr._RefCounter;
  if(_RefCounter != 0)
    ++(*_RefCounter);

  return *this;
}

TEMPLATE_TA inline T* SMARTPTR::getPointer() 
{
  return _Pointer;
}
TEMPLATE_TA inline const T* SMARTPTR::getPointer() const
{
  return _Pointer;
}
TEMPLATE_TA inline SMARTPTR::~SmartPointer()
{
  _Release();
}

TEMPLATE_TA inline void SMARTPTR::_Release()
{
  if(_RefCounter != 0)
  {
    (*_RefCounter)--;
    if(*_RefCounter <= 0)
    {
      delete _RefCounter;
      if(_Pointer != 0)
      if(Array)
        delete[] _Pointer;
      else
        delete _Pointer;
    }
  }
}

TEMPLATE_TA inline const T& SMARTPTR::operator[] (int i) const
{
  return _Pointer[i];
}

TEMPLATE_TA inline T& SMARTPTR::operator[](int i)
{
  return _Pointer[i];
}
