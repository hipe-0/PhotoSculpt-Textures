// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once
#include "Image.h"
#include "Obfuscate.h"

class Pyramid
{
public:
  Pyramid();

  void   initTables       (int sizex, int sizey);
  bool   downsampleImages (Image& l, Image& r);
  int    computeStepUsingSize (int sizex, int sizey)  const;
  Image& computeImageLAtStep(int step) ;
  Image& computeImageRAtStep(int step) ;
  void   clear            ();
  void   clearAtStep      (int step);
  int    getSizeXAtStep   (int step)  const;
  int    getSizeYAtStep   (int step)  const;
  bool   isInitialised    () const;
  int    nbStep           () const;

  // deprecated (to be deleted)
  int    computeLevelUsingSize(int sizex, int sizey) const;
  Image& computeImageL    (int level) ;
  Image& computeImageR    (int level) ;
  int    getSizeX         (int level) const;
  int    getSizeY         (int level) const;
  int    smallestLevel    () const;
  void   clear            (int level);

private:
  void   blurallimages_   (float value);

  int                     sizesX_[20], sizesY_[20] ;
  oint                    nbImages_                ;
  CImgList<unsigned char> ImagesL_, ImagesR_       ;
  Image                   *originalL_, *originalR_ ;

};
