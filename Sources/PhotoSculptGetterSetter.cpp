/* PhotoSculptGetterSetter.cpp
 *
 *  Created on: 14 may 2009
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 *  80 char per line
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_
 */

#include "Photosculpt.h"
#include "Convert.h"
#include "Perspective.h"
#include "PhotoSculptTools.h"
#include "Options.h"

VectorMap PhotoSculpt::getMap() const
{
  return *currentMapStable_;
}

void PhotoSculpt::addLocalDetailToMap (PIntMap map) 
{
  MUTEX;
  PIntMap detailmap;
  
  // computes local detail
  int level      = computeLevel();
  Image left     = pyramid_.computeImageL(level);
  Image detailimage   = ImageGeneration::generateFakeDepth(left);

  left = Image(); // free memory;

  float strength = 0.005f * MULTIPLY ;

  detailmap = new IntMap (PhotoSculptTools::applyDisplace(detailimage, 
    *map, strength, 0.5f));
    
  PhotoSculptTools::mixByAlpha(map, detailmap, stableAlpha_);
  MUTEXRELEASE;
}

extern IntMap getMapX(const VectorMap& vectormap);

PIntMap PhotoSculpt::computeIntMap()
{
  // PIntMap workdepthmap = new IntMap(getMapX(*currentMapStable_)); // old method

  MUTEX;
  PIntMap workdepthmap;
  if (GlobalOptions.isPerspectiveCorrected_ && currentMapStable_->getSizeX()>20)
    workdepthmap = new IntMap(getPerspCorrectedMap(*currentMapStable_, pointofview_));
  else
    workdepthmap = new IntMap(getPerspCorrectedHMap(*currentMapStable_));

  if (GlobalOptions.isAutoLocalDetail_)
    addLocalDetailToMap(workdepthmap);

  MUTEXRELEASE;
  return workdepthmap ;
}


// warning : supposes that the given displacement map has a compatible size with
//           one of the images in the pyramid
void PhotoSculpt::setMap(VectorMap& map, bool issculpted)
{
  MUTEX;

  ASSERTMUTEX (map.getSizeX() < pyramid_.getSizeX(pyramid_.smallestLevel()) ||
      map.getSizeY() < pyramid_.getSizeY(pyramid_.smallestLevel()) ,
    "cannot accept this map, it's too small (setMap())" ) ;
  
  *currentMapStable_ = map;
  isSculpted_ = issculpted;

  MUTEXRELEASE;
}

#include "POV.h"

void PhotoSculpt::resetMap()
{
  MUTEX;
  ASSERTMUTEX (! pyramid_.isInitialised(), "Internal error : no pyramid was initialised (resetMap())" );

  int level = pyramid_.smallestLevel();

  int sizex = pyramid_.getSizeX(level);
  int sizey = pyramid_.getSizeY(level);

#ifdef INITIAL_SCULPT_IS_FLAT
  currentMapStable_ = new VectorMap( sizex, sizey );
  currentMapStable_->fill(0); // 0 no displacement so far
#else
  currentMapStable_ = new VectorMap( pointofview_.generateMapOfFlatObject(sizex,sizey) ) ;
#endif
  isSculpted_ = false;

  setSculptInProgress();

  MUTEXRELEASE;
}

void PhotoSculpt::retrieveSculptInProgress()
{
  MUTEX;
  *currentMapStable_ = *(sculptInProgress_.getMap()) ;
  MUTEXRELEASE;
}

void PhotoSculpt::setSculptInProgress()
{
  MUTEX;
  sculptInProgress_.setMap      ( *currentMapStable_ ) ;
  MUTEXRELEASE;
  prepareImages_();
}

/////////////////// MAP SIZE

int PhotoSculpt::getStableSizeX() const
{
  return currentMapStable_->getSizeX();
}

int PhotoSculpt::getStableSizeY() const
{
  return currentMapStable_->getSizeY();
}

Image PhotoSculpt::computeLeftImage()
{
  return computeLeftImage(computeLevel());
}

Image PhotoSculpt::computeRightImage()
{
  return computeRightImage(computeLevel());
}

Image PhotoSculpt::getTexture()
{
  return computeLeftImage(0);
}

Image PhotoSculpt::computeLeftImage(int level) 
{
  return pyramid_.computeImageL(level);
}

Image PhotoSculpt::computeRightImage(int level) 
{
  return pyramid_.computeImageR(level);
}

int PhotoSculpt::computeLevel() const
{
  const PVectorMap m = currentMapStable_;

  return computeLevelUsingSize(m->getSizeX(), m->getSizeY());
}

oint PhotoSculpt::getMaxLevel() const
{
  return pyramid_.smallestLevel();
}

oint PhotoSculpt::computeStep() const
{
  const PVectorMap map = currentMapStable_;

  int level = computeLevelUsingSize(map->getSizeX(), map->getSizeY());

  return getMaxLevel() - level;
}

bool PhotoSculpt::isFirstStep() const
{
  return computeLevel() == pyramid_.smallestLevel();
}


bool PhotoSculpt::isLastStep() const
{
  return (computeLevel()==0);
}


bool PhotoSculpt::hasDownSamples() const
{
  return pyramid_.isInitialised();
}

bool PhotoSculpt::isModel() const
{
  return pyramid_.isInitialised() && !currentMapStable_->isEmpty();
}

void  PhotoSculpt::setAlpha(PImage alpha)
{
  MUTEX;
  sculptInProgress_.setAlpha(alpha);
  stableAlpha_ = alpha ;
  MUTEXRELEASE;
}

POV PhotoSculpt::getPOV() const
{
  return pointofview_;
}

void PhotoSculpt::setPOV(const POV& pointofview)
{
  pointofview_ = pointofview;
}

void PhotoSculpt::setFocalLength(float focallength)
{
  if (focallength == 0.0f)
    focallength = 50.0f; // 50 mm default focal

  pointofview_.setFocalLength(focallength);
}

// dangerous stuff to be deleted ASAP

int PhotoSculpt::getCurrentMapLevelUNSTABLE() const
{
  const PVectorMap map = sculptInProgress_.getMap();

  ASSERT (map->isEmpty(), "Invalid sculpt in progress (in getCurrentMapLevelUNSTABLE())");

  return computeLevelUsingSize(map->getSizeX(), map->getSizeY());
}


int PhotoSculpt::getUNSTABLESizeX() const
{
  ASSERT(!sculptInProgress_.isValid(), 
    "Invalid sculpt in progress (in getUNSTABLESizeX())");
  return sculptInProgress_.getMap()->getSizeX();
}

int PhotoSculpt::getUNSTABLESizeY() const
{
  ASSERT(!sculptInProgress_.isValid(), 
    "Invalid sculpt in progress (in getUNSTABLESizeY())");
  return sculptInProgress_.getMap()->getSizeY();
}

oint PhotoSculpt::getStepNoUNSTABLE() const
{
  ASSERT(!sculptInProgress_.isValid(), "Invalid sculpt in progress (in getStepNoUNSTABLE())");
  const PVectorMap map = sculptInProgress_.getMap();

  int level = computeLevelUsingSize(map->getSizeX(), map->getSizeY());

  return getMaxLevel() - level;
}

const Image& PhotoSculpt::getLeftImageUNSTABLE() const
{
  ASSERT(!sculptInProgress_.isValid(), "Invalid sculpt in progress (in getLeftImageUNSTABLE())");
  return sculptInProgress_.getImageL();
}

const Image& PhotoSculpt::getRightImageUNSTABLE() const
{
  ASSERT(!sculptInProgress_.isValid(), "Invalid sculpt in progress (in getRightImageUNSTABLE())");
  return sculptInProgress_.getImageR();
}

bool PhotoSculpt::isSculpted()
{
  return isSculpted_;
}