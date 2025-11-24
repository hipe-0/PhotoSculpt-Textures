/*
 * MultiTasking.h
 *
 *  Created on: 2011/01/10
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#pragma once
#include "Mutex.h"
#include "Error.h"

class MultiTasking
{
public:
  static void  launch(int nbIndex, void (*task)(int Index, void * object), void * object = 0);

  // manage number of cores
  static int   getNumberOfCoresInSystem();
  static void  setNbCores(int nbcores);
  static void  setNbCoresAuto();
  static int   getNbCores();

public:
  static Mutex mutex_      ; // public as the thread should be able to access it
  static Mutex launchMutex_;

private:
  static int   NbCores_    ;
  static bool  NbCoreAuto_ ;
};

class Parallelable
{
public:
  virtual void task(int line);

protected:
  static void taskstatic(int line, void* object)
  {
    static Mutex mutex;
    mutex.stopThreadHere();

    if (object == NULL)
      THROW_INTERNAL_ERROR("Object is null in Parallelable::taskstatic()");
      
    Parallelable* this_ = reinterpret_cast<Parallelable*>(object);

    if (this_ == NULL)
      THROW_INTERNAL_ERROR("this_ is null in Parallelable::taskstatic()");
 
    this_->task(line);
    mutex.releaseThreadHere();
  }

public:
  void launchTask(int nbindex)
  {
    MultiTasking::launch(nbindex, taskstatic, (void *) this);
  }
};
