#pragma once

#include "VectorMap.h"
#include "Vector.h"

class PerspectiveAnalyser
{
public:
  PerspectiveAnalyser(void);

  void init(const VectorMap& map);

  bool isTooOffCenter      () ;
  bool isTooZoomed         () ;
  bool isTooStrongAngle    () ;
  bool isLeftRightInverted () ;
  bool isRotate90          () ;
  bool isCameraNotLevel    () ;
  bool isNotEnoughAngle    () ;
  bool isTooMuchAngle      () ;

private:
  float offcenter_ , zoomedout_, zoomedin_, horizonangle_, 
        pinchedleft_ , pinchedright_, pinchedtop_ , pinchedbottom_,
        direction_, pinch_;
};
