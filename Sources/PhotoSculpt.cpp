/*
 * PhotoSculpt.cpp
 *
 *  Created on: 3 févr. 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "PhotoSculpt.h"
#include "Includes.h"
#include "UserInterrupt.h"
#include "Options.h"

PhotoSculpt::PhotoSculpt()
{ 
  clear();
}

void PhotoSculpt::init(Image& leftimage, Image& rightimage)
{
  DEBUG(PhotoSc);
  currentMapStable_ = new VectorMap;

  // warning : this operation can take a few seconds
  bool isResultOk=pyramid_.downsampleImages(leftimage, rightimage);
  
  if (!isResultOk)
    THROW_INTERNAL_ERROR( "Initialisation error due to user images not ok (PhotoSculpt())");

  resetMap();

  DEBUG_VALUE("  Init Photosculpt with Image sizex", leftimage.dimx() );
  DEBUG_VALUE("  Init Photosculpt with Image sizey", leftimage.dimy() );
}

void PhotoSculpt::prepareImages_()
{
  int level = getCurrentMapLevelUNSTABLE();

  sculptInProgress_.setImageL( new Image(pyramid_.computeImageL (level)) );
  sculptInProgress_.setImageR( new Image(pyramid_.computeImageR (level)) );

  pyramid_.clear(level);

  // Change left image color and exposure so that it matches the right image
  if (getMaxLevel()-level > 3)
  {
    adjustExposureGlobal_();
    adjustExposureLocal_();
  }

  //DEBUG_SAVE_IMAGE(sculptInProgress_.getImageL(), "prepareImages left.jpg");
  //DEBUG_SAVE_IMAGE(sculptInProgress_.getImageR(), "prepareImages right.jpg");
}

/// Destroys the Sculpt object entirely
void  PhotoSculpt::clear()
{
  pyramid_.clear();
  sculptInProgress_.clear();
  currentMapStable_ = new VectorMap ;
  stableAlpha_      = new Image     ;
  isSculpted_       = false;
}

int PhotoSculpt::computeLevelUsingSize(int sizex, int sizey) const
{
  return pyramid_.computeLevelUsingSize(sizex, sizey);
}

void PhotoSculpt::setEditCoordinates(const EditCoordinates& e)
{
  editCoordinates_ = e;
}

EditCoordinates PhotoSculpt::getEditCoordinates() const
{
  return editCoordinates_;
}

void PhotoSculpt::clearCurrentSculptInProgress()
{
  sculptInProgress_.clear();
}

float PhotoSculpt::estimateTimeToEnd()
{
  return sculptInProgress_.estimateTimeToEnd();
}

int PhotoSculpt::getSubStep()
{
  return sculptInProgress_.getSubStep();
}

// this function double the size of the disp map in both x and y size and in
// value
//
bool PhotoSculpt::interpolateToNextLevel()
{
  DEBUG(intnextl);
  MUTEX;
  if (!isLastStep())
  {
    int level = computeLevel()-1;

    int newsizex = pyramid_.getSizeX(level);
    int newsizey = pyramid_.getSizeY(level);

    sculptInProgress_.setMap( currentMapStable_->getResizeAndMultiply(newsizex, newsizey) );
    //isSculpted_ = false;

    prepareImages_();

    MUTEXRELEASE;
    return true;
  }
  else
  {
    MUTEXRELEASE;
    return false;
  }
}

bool PhotoSculpt::downsampleToPreviousLevel()
{
  MUTEX;
  if (!isFirstStep())
  {
    int level = computeLevel()+1;

    int newsizex=pyramid_.getSizeX(level);
    int newsizey=pyramid_.getSizeY(level);

    *currentMapStable_ = currentMapStable_->getResizeAndMultiply(newsizex, newsizey) ;

    setSculptInProgress();
    isSculpted_ = true;

    MUTEXRELEASE;
    return true;
  }
  else
  {
    MUTEXRELEASE;
    return false;
  }
}

bool PhotoSculpt::growAndOptimizeThisLevel(Parameter parameters)
{
  MUTEX;
  int step = computeStep();

  if (isSculpted_)
  {
    bool islast = ! interpolateToNextLevel();
    if (islast) return false;

    step++;
  }
  else
  {
    sculptInProgress_.setMap( *currentMapStable_ ) ;
    prepareImages_();
  }
  
  ASSERTMUTEX (!sculptInProgress_.isValid(),
               "Optimisation was launched without valid sculptinprogress object (in optimizeThisLevel())");

  sculptInProgress_.optimizeThisLevel(parameters, editCoordinates_, step, 
    getMaxLevel(), pointofview_); // very slow processing

  *currentMapStable_ = *(sculptInProgress_.getMap()) ;
  pointofview_ = sculptInProgress_.getPOV();
  
  isSculpted_ = true;

  sculptInProgress_.clear(); // frees 160 Mb

  MUTEXRELEASE;
  if (isLastStep())  return false;
  else               return true;
}

bool PhotoSculpt::growAndOptimizeThisLevel()
{
  int stepofnextmap = computeStep();

  if (isSculpted_) stepofnextmap ++;

  Parameter parameters = RECIPE[stepofnextmap];

  return growAndOptimizeThisLevel(parameters);
}


