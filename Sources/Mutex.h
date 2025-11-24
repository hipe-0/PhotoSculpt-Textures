#pragma once
#include <windows.h>

#define MUTEX        mutex_.stopThreadHere();
#define MUTEXRELEASE mutex_.releaseThreadHere();

//#define MUTEX        MutexDoor mutexdoor(&mutex_);
//#define MUTEXRELEASE ;

class Mutex
{
public:
  Mutex(void);
  void stopThreadHere();
  void releaseThreadHere();

private:
  HANDLE mutexHandle_;
  bool   islocked_;
};

// For debug purposes, this mutex object has same members as above but does nothing 
//   at all
class FakeMutex
{
public:
  FakeMutex(void) {}
  void stopThreadHere() {}
  void releaseThreadHere() {}
};

inline Mutex::Mutex(void)
{
  mutexHandle_ = CreateMutex(0,FALSE,0);
  islocked_    = false;
}

inline void Mutex::stopThreadHere()
{
  islocked_ = true;
  WaitForSingleObject(mutexHandle_,INFINITE);
}

inline void Mutex::releaseThreadHere()
{
  ReleaseMutex(mutexHandle_);
  islocked_ = false;
}
/*
class MutexDoor
{
public:
  MutexDoor(Mutex* mutex)               { mutex_ = mutex; mutex_ -> stopThreadHere(); }
  MutexDoor(FakeMutex* mutex):mutex_(0) { }
  ~MutexDoor()                          { if (mutex_ != 0) mutex_ -> stopThreadHere(); }
 
private:
  Mutex* mutex_;
};
*/