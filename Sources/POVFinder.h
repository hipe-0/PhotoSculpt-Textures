#pragma once

#include "POV.h"
#include "Image.h"
#include "VectorMap.h"
#include "FloatMap.h"

class POVFinder
{
public:
  POVFinder();

  void  init                 (const Image& l, const Image& r, const Image& alpha, 
                              const VectorMap& v, const POV& pointofview, 
                              int maxsize);

  POV   findBestView         ();
  POV   findBestView         (FloatVector mask, int nbsamplesperline);
  //void  setInitialPOV        (const POV& pov);
  POV   getCurrentBestPOV    ();
  float getCurrentBestScore  ();

  Image     generateImage             (const POV& pointofview) const;
  //VectorMap generateMapOfCoordinates  (const POV& pointofview);

  int   getMapSizeX() const ;
  int   getMapSizeY() const ;
  void  trace      () const ;
  void  updateDepths(const POV& pointofview);
  VectorMap generateMapOfCoordinates  () const;
  int   getNbSamplePerLine() const;

public:
  float computeScore_        (const POV& pointofview, int nbsamplesperline);

private:  
  Image computeScoreImage_   (const POV& pointofview, int nbsamplesperline) const;
  POV   findBestXYAngle_     (const POV& pointofview);
  POV   findBestZoomAndRoll_ (const POV& pointofview);

  Image     sourceL_, sourceR_, alpha_;
  VectorMap map_    ;
  FloatMap  depths_ ;
  POV       currentBestPov_  ;
  float     currentBestScore_;
  int       nbSamplePerLine_ ;
};
