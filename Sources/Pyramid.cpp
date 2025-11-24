#include "Pyramid.h"
#include "Includes.h"
#include "UserInterrupt.h"


Pyramid::Pyramid()
{
  for (int i = 0 ; i < NB_MAX_LEVELS ; i++)
  {
    sizesX_[i] = 0;
    sizesY_[i] = 0;
  }

  nbImages_ = 0;
}

void Pyramid::initTables(int sizex, int sizey)
{
  DEBUG(PyrmiiTab);
  //NANOBEGIN; // crashes here
  int level = 0;

  sizesX_[0]=sizex;
  sizesY_[0]=sizey;

  while (sizesX_[level] > MIN_IMAGE_SIZE &&
         sizesX_[level] > MIN_IMAGE_SIZE)
  {
    sizesX_[level+1] = sizesX_[level]/2;
    sizesY_[level+1] = sizesY_[level]/2;

    level++ ;
  }

  nbImages_ = level + 1;
  //NANOEND;
}

bool Pyramid::downsampleImages( Image& l, Image& r)
{
  DEBUG(PyrmiddonsaeIma);
  // NANOBEGIN; // crash
  // we clear all images previously created into this class before (previous run)
  ImagesL_.clear();
  ImagesR_.clear();

  if (l.is_empty() && r.is_empty())
    THROW_MESSAGE_TO_USER(IMAGE_EMPTY1);

  if (l.is_empty())
    THROW_MESSAGE_TO_USER(IMAGE_EMPTY2);

  if (r.is_empty())
    THROW_MESSAGE_TO_USER(IMAGE_EMPTY3);

  if (l.dimx()!=r.dimx() || l.dimy()!=r.dimy())
    THROW_MESSAGE_TO_USER(IMAGE_SIZES);

  if (l.dimx()<100 || l.dimy()<100)
    THROW_MESSAGE_TO_USER(IMAGE_SMALL);

  if (l.dimx()>10000 || l.dimy()>10000)
    THROW_MESSAGE_TO_USER (IMAGE_BIG);


  initTables(l.dimx(), l.dimy());

  originalL_ = &l;
  originalR_ = &r;

  int level = 0;
  ImagesL_ << Image(1,1,3,1); 
  ImagesR_ << Image(1,1,3,1); 

  level = 1;
  ImagesL_ << Image(1,1,3,1);
  ImagesR_ << Image(1,1,3,1);

  level = 2;
  UserInterrupt::stopIfOrdered();

  ImagesL_ << originalL_ -> get_resize(sizesX_[level], sizesY_[level],ANY,ANY,CIMG_INTERPOLATION_BICUBIC);
  UserInterrupt::stopIfOrdered();
  ImagesR_ << originalR_ -> get_resize(sizesX_[level], sizesY_[level],ANY,ANY,CIMG_INTERPOLATION_BICUBIC);

  for (level = 2 ; level < nbImages_ ; level ++)
  {
    UserInterrupt::stopIfOrdered();
    ImagesL_  << ImagesL_[level].get_resize_halfXY();
    UserInterrupt::stopIfOrdered();
    ImagesR_ << ImagesR_[level].get_resize_halfXY();
  }

  UserInterrupt::stopIfOrdered();

  blurallimages_(BLUR_IMAGES_VALUE);
  //NANOEND;
  return true; // says the function has worked properly
}

void Pyramid::blurallimages_(float value)
{
  for (int i=0; i < nbImages_; i++)
  {
    ImagesL_[i].blur(value);
    ImagesR_[i].blur(value);
  }
}

int Pyramid::computeLevelUsingSize(int sizex, int sizey) const
{
  if ( nbImages_==0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in computeLevelUsingSize())");

  for (int i=0; i < nbImages_ ; i++)
    if (sizesX_[i] <= sizex && sizesY_[i] <= sizey)
      // Returns the first level that matches the criteria (smaller or equal
      //    than specified size)
      return i;

  // no level is smaller than the specified size
  return -1;
}

int Pyramid::computeStepUsingSize(int sizex, int sizey) const
{
  if ( nbImages_==0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in computeLevelUsingSize())");

  for (int level=0; level < nbImages_ ; level++)
    if (sizesX_[level] <= sizex && sizesY_[level] <= sizey)
      // Returns the first step that matches the criteria (smaller or equal
      //    than specified size)
      return nbImages_-1-level;

  // no level is smaller than the specified size
  return -1;
}

Image& Pyramid::computeImageLAtStep (int step)
{
  int level = nbImages_-1-step;
  return computeImageL(level);
}

Image& Pyramid::computeImageRAtStep (int step)
{
  int level = nbImages_-1-step;
  return computeImageR(level);
}

void Pyramid::clear()
{
  ImagesL_.clear();
  ImagesR_.clear();
}

void Pyramid::clear(int level)
{
  if ( nbImages_ ==0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in clear())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in clear())");

  ImagesL_[level] = Image(1,1,3,1);
  ImagesR_[level] = Image(1,1,3,1);
}

void Pyramid::clearAtStep(int step)
{
  int level = nbImages_-1-step;
  if ( nbImages_ ==0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in clear())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in clear())");

  ImagesL_[level] = Image(1,1,3,1);
  ImagesR_[level] = Image(1,1,3,1);
}

int Pyramid::getSizeX(int level) const
{
  if ( nbImages_ == 0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in getSizeX())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in GetSizeX())");

  return sizesX_[level];
}

int Pyramid::getSizeY(int level) const
{
  if ( nbImages_ == 0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in getSizeY())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in GetSizeY())");

  return sizesY_[level];
}

int Pyramid::getSizeXAtStep(int step) const
{
  int level = nbImages_-1-step;

  if ( nbImages_ == 0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in getSizeX())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in GetSizeX())");

  return sizesX_[level];
}

int Pyramid::getSizeYAtStep(int step) const
{
  int level = nbImages_-1-step;

  if ( nbImages_ == 0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in getSizeY())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in GetSizeY())");

  return sizesY_[level];
}

bool Pyramid::isInitialised() const
{
  return !( nbImages_ == 0 );
}

int Pyramid::smallestLevel() const
{
  if ( nbImages_ == 0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in lastLevel())");

  return nbImages_ - 1 ;
}

int Pyramid::nbStep() const
{
  if ( nbImages_ == 0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in lastLevel())");

  return nbImages_ ;
}

Image& Pyramid::computeImageL (int level)
{
  if ( nbImages_==0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in getImageL())");
  if (level<0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in GetSizeX())");

  if (level > 1)
  {
    // was the requested image destroyed using the clear(level) function?
    if ( ImagesL_ [level].dimx() <= 1 ||  ImagesL_ [level].dimy()  <= 1 )
      // then quickly rebuild it.
    {
      if (level == 2)
        ImagesL_ [level] = originalL_ -> get_resize(sizesX_[level], sizesY_[level]);
      else
        downsampleImages(*originalL_, *originalR_);
    }
    return ImagesL_ [level];
  }
  else if (level == 1)
    return ImagesL_ [1] = originalL_ -> get_resize_halfXY();
  else
    return *originalL_;
}

Image& Pyramid::computeImageR (int level)
{
  if ( nbImages_ ==0 )
    THROW_INTERNAL_ERROR("Pyramid not initialised (in getImageR())");
  if ( level < 0 || level >= nbImages_ )
    THROW_INTERNAL_ERROR("Wrong argument (in GetSizeX())");

  if (level > 1)
  {
    // was the requested image destroyed using the clear(level) function?
    if ( ImagesR_ [level].dimx() <= 1 ||  ImagesR_ [level].dimy()  <= 1 )
      // then quickly rebuild it.
      if (level == 2)
        ImagesR_ [level] = originalR_ -> get_resize(sizesX_[level], sizesY_[level]);
      else
        downsampleImages(*originalL_, *originalR_);
    return ImagesR_ [level];
  }
  else if (level == 1)
    return ImagesR_ [1] = originalR_ -> get_resize_halfXY();
  else
    return *originalR_;
}
