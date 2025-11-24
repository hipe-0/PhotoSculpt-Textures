#pragma once

#include "Image.h"
#include "SmartPointer.h"

class Histogram
{
public:
  int pixelCount_[256];

  int   computeQuantityBetween(int value1, int value2);
  float computeMeanValue      ();
  int   computeQuantile       (int quantity);
  PImage computeImage          ();
};



typedef SmartPointer<Histogram, false> PHistogram;



class ExposureAnalyser
{
public:
  ExposureAnalyser(void);

  void init(const Image& left, const Image& right);
  static PHistogram computeHistogram(const Image& i);
  
  bool isTooDark           () ;
  bool isTooLight          () ;
  bool isTooMuchContrast   () ;
  bool isTooLittleContrast () ;
  bool isTooShiny          () ;
  bool isTooDifferent      () ;
  bool isLeftBlurred       () ;
  bool isRightBlurred      () ;

private:
  float difference_ , tooDark_ , tooLight_ , contrasted_ , tooMuchShines_,
    edgeleftright_;
};
