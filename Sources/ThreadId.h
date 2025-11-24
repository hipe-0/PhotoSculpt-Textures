#pragma once

#include "windows.h"

class ThreadId
{
public:
  inline ThreadId      ();
  inline void reset    ();
  inline bool isCurrent();

private:
  DWORD threadId_;
};

extern ThreadId GlobalWindowsThreadID ;

#ifdef THREADID_PROTECTION

  #define ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD()  { if (!GlobalWindowsThreadID.isCurrent()) THROW_INTERNAL_ERROR("this function has to run on main thread"); }
  #define NEVER_EXECUTE_ON_WINDOWS_THREAD() \
     { if (GlobalWindowsThreadID.isCurrent())    \
         THROW_INTERNAL_ERROR("this function cannot run on main thread (will block windows loop)"); \
     }

#else
  #define ALWAYS_EXECUTE_ON_WINDOWS_LOOP_THREAD()
  #define NEVER_EXECUTE_ON_WINDOWS_THREAD()

#endif

inline ThreadId::ThreadId()
{
  threadId_ = GetCurrentThreadId();
}

inline bool ThreadId::isCurrent()
{
  return threadId_ == GetCurrentThreadId();
}

inline void ThreadId::reset()
{
  threadId_ = GetCurrentThreadId();
}
