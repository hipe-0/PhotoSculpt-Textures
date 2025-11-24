#include "SculptInProgress.h"

SculptInProgress::SculptInProgress()
{
  clear();
}

void SculptInProgress::clear()
{
  substep_         = step_ = lastStep_ = 0 ;
  currentLeft_     = new Image;
  currentRight_    = new Image;
  parameter_       = Parameter();
  currentMap_      = new VectorMap;
  alpha_           = new Image;
  tasktimer_.interruptRecording();
}

bool SculptInProgress::isValid() const
{
  // are all maps correctly setup?
  if (currentLeft_  -> is_empty()) return false;
  if (currentRight_ -> is_empty()) return false;
  if (currentMap_   -> isEmpty())  return false;

  // are all dimensions the same = correct?
  if (currentLeft_  -> dimx() != currentMap_ -> getSizeX() ) return false;
  if (currentRight_ -> dimx() != currentMap_ -> getSizeX() ) return false;
  if (currentLeft_  -> dimy() != currentMap_ -> getSizeY() ) return false;
  if (currentRight_ -> dimy() != currentMap_ -> getSizeY() ) return false;
  
  return true;
}

const Image&  SculptInProgress::getImageL() const
{
  return *currentLeft_;
}

const Image&  SculptInProgress::getImageR() const
{
  return *currentRight_;
}

void SculptInProgress::setImageL(PImage i)
{
  currentLeft_ = i;
}

void SculptInProgress::setImageR(PImage i)
{
  currentRight_ = i;
}

const PVectorMap & SculptInProgress::getMap() const
{
  return currentMap_;
}
void SculptInProgress::setMap(VectorMap m)
{
  *currentMap_ = m;
}

float SculptInProgress::estimateTimeToEnd()
{
  if (this->isValid())
    return tasktimer_.estimateSecondsNow();
  else 
    return -2.0f;
}

void SculptInProgress::setAlpha(PImage alpha)
{
  alpha_ = alpha;
}

POV SculptInProgress::getPOV() const
{
  return pointofview_;
}

