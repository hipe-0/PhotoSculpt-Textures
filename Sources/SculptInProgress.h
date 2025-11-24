/*
 * SculptInProgress.h
 *
 *  Created on: 5 oct. 2009
 *      Author: Hippo
 */

// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#ifndef SCULPTINPROGRESS_H_
#define SCULPTINPROGRESS_H_

#include "VectorMap.h"
#include "ImageGeneration.h"
#include "ProgressionBar.h"
#include "BurstMap.h"
#include "Parameter.h"
#include "EditCoordinates.h"
#include "Obfuscate.h"
#include "TaskTimer.h"
#include "SmartPointer.h"
#include "POV.h"

class SculptInProgress
{
public:  
  SculptInProgress();

  void optimizeThisLevel(Parameter p, const EditCoordinates& e, int step, int laststep, POV pov);

  void stepBlur0();

  void stepBlur1          ();
  void stepBlur2          ();
  void stepBlur3          ();
  void stepOpt1           ();
  void stepOpt2           ();
  void stepOpt3           ();
  void stepAreaFill       ();
  void stepSmartBlur      ();
  void stepBlurExceptEdges();
  void stepFakeDepth      ();
  void stepBlurY          ();
  void stepApplyEdit      (const EditCoordinates& e);
  void step3DAnalysis     ();
  void stepPushAlpha      ();

  void applyVector_       (int lx, int ly, Vector v, int radius, VectorMap &m);

  void  test_fall_off() const;

  float fallOffValue(float distance, float max,
	                   float falloffFirstStepdown, float falloffAtEnd) const;


  PFloatMap  computeValueMap    (int radius=3, float maxv=0.01f);
  float      comparePixelPatch  (int xL, int yL, int xR, int yR, int Scope,
                                 float maxv) const;
  void       isImprovedValueMap (PFloatMap& mask, int pixRange, float maxv);

  float      findBestNeibour    (int xL,int yL, int& retx, int& rety,
                                 int DRange, int Scope, float MaxColorMatch);
  void       improveMap(PVectorMap& map,
                                  PFloatMap& oldmask, 
                                  int pixRange, float maxv);


  /// optimises this level by matching patches of pixels from left and right
  //  image
  void  optimiseMap           (int DisplacementRange,int Scope,int Skip,
                               float MaxColorMatch);

  /// more complex : keeps only the best bits from an optimisation step
  void  optimiseMapKeepBestOnly(int a,int b,int c,float maxv);

  static void       MixDispMap(PVectorMap DispMapOld,
                               const PVectorMap DispMapNew,
                               const PFloatMap  MaskNew);

  void  compareFloatMap       (PFloatMap Maskbase, const PFloatMap MaskNew);

  int               getSubStep() const      ;

  const Image&      getImageL () const      ;
  const Image&      getImageR () const      ;
  void              setImageL (PImage i)    ;
  void              setImageR (PImage i)    ;

  const PVectorMap & getMap   () const      ;
  void              setMap    (VectorMap m) ;

  void  clear                 ()            ;
  void  changeStep            (int substep) ;
  void  resetTimer            ()            ;
  void  resetTimer            (int newstep) ;
  float estimateTimeToEnd     ()            ;
  void  setAlpha              (PImage alpha);
  bool  isValid               () const      ;
  float getScore              ()            ;
  float getGoodPixelPercent   (float qualityThreshold);
  POV   getPOV                () const      ;
  
private:

  // Current displacement map under work
  PVectorMap   currentMap_                   ;
  POV          pointofview_                  ;

  PImage       currentLeft_, currentRight_   ;
  PImage       alpha_                        ;

  Parameter    parameter_                    ;

  int          step_, substep_, lastStep_    ;
  TaskTimer    tasktimer_                    ;
};


#endif