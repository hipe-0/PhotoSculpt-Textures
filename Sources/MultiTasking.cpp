/*
 * MultiTasking.cpp
 *
 *  Created on: 2011/01/10
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include "Windows.h"
#include <process.h>
#include "Mutex.h"
#include "MultiTasking.h"
#include "UserInterrupt.h"
#include "Trace.h"

static int NextLine;
static HANDLE eventleave;

static int  nbtask;
static bool isInterrupt;

static void (*TaskPointer)(int line, void * object);
Mutex MultiTasking::mutex_;
Mutex MultiTasking::launchMutex_;

void thread(PVOID object)
{
  MultiTasking::mutex_.stopThreadHere();

  nbtask++;
  HANDLE handle = GetCurrentThread();
  SetThreadPriority(handle, THREAD_PRIORITY_BELOW_NORMAL);

  while(NextLine >= 0 && !isInterrupt)
  {
    int line = NextLine;
    NextLine--;
   
    MultiTasking::mutex_.releaseThreadHere();

    try
    {
      TaskPointer(line, object);
    }
    catch (UserInterrupt)
    {
      isInterrupt = true;
    }

    MultiTasking::mutex_.stopThreadHere();
  }

  nbtask--;

  if (nbtask == 0) // last task?
    SetEvent(eventleave); // let's leave the "MultiTasking()" function

  MultiTasking::mutex_.releaseThreadHere();
}

void MultiTasking::launch(int nblines, void (*task)(int line, void * object), void * object)
{
  launchMutex_.stopThreadHere();

  DEBUG(MultiT);
  int nbtask = getNbCores();
  DEBUG_VALUE("  nbtask", nbtask);

  eventleave = CreateEvent(NULL, FALSE, FALSE, NULL) ;
  ResetEvent(eventleave);

  NextLine    = nblines - 1 ;
  TaskPointer = task        ;
  isInterrupt = false;

  for (int i = 0; i < nbtask; i++)
    _beginthread(thread, 0, object);

  WaitForSingleObject(eventleave, INFINITE);

  launchMutex_.releaseThreadHere();
  if (isInterrupt) // All the threads have been stoped by user, but the main thead is not stoped yet
    throw UserInterrupt(); // throw another interrupt on the main thread to kill the action
}

int MultiTasking::getNumberOfCoresInSystem()
{
  SYSTEM_INFO sysinfo;
  GetSystemInfo( &sysinfo );

  int numCPU = sysinfo.dwNumberOfProcessors;
  DEBUG_VALUE("  Number of Processors", numCPU);
  return numCPU;
}

int  MultiTasking::NbCores_    = -1;
bool MultiTasking::NbCoreAuto_ = true;

void MultiTasking::setNbCores(int nbcores)
{
  NbCoreAuto_ = false;
  NbCores_ = nbcores;
  DEBUG_VALUE("  Setting number of cores", nbcores);
}

void MultiTasking::setNbCoresAuto() 
{
  NbCoreAuto_ = true;
}

int  MultiTasking::getNbCores()
{
  if (NbCoreAuto_)
    return NbCores_ = getNumberOfCoresInSystem();
  else
    return NbCores_;
}

void Parallelable::task(int line) { }