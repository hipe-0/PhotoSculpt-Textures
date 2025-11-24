/*
 * SculptOptimize.cpp
 *
 *  Created on: 22 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
 
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#include "PhotoSculpt.h"
#include "Timer.h"
#include "Limit.h"
#include "ImageGeneration.h"
#include "View3d.h"
#include "PhotoSculptTools.h"
#include "UserInterrupt.h"
#include "Options.h"

void SculptInProgress::changeStep(int substep)
{
  tasktimer_.stopRecordTime();
  substep_=substep; 
  DEBUG_VALUE(  "substep", substep_);
  tasktimer_.startRecordTime(step_,substep_);
}

void SculptInProgress::optimizeThisLevel(Parameter parameter, 
                                         const EditCoordinates& editcoorditate, 
                                         int step, int laststep, POV pointofview)
{
  DEBUG(ptimzhisLve);
  NEVER_EXECUTE_ON_WINDOWS_THREAD();

  step_ = step; lastStep_ = laststep;
  substep_ = 1;
  tasktimer_.setNbTasks     ( lastStep_ + 1   );
  tasktimer_.startRecordTime( step_, substep_ );

  //************************** BEGIN OF OPTIMISATION
  parameter_  = parameter;
  pointofview_= pointofview;

  stepOpt1 ();                                changeStep(2);
  stepBlur1(); stepApplyEdit(editcoorditate); changeStep(3);
  stepOpt2 ();                                changeStep(4);
  stepBlur2(); stepApplyEdit(editcoorditate); changeStep(5);
  stepOpt3 ();                                changeStep(6);

  stepApplyEdit(editcoorditate);

  stepSmartBlur();                            changeStep(7);
  stepBlurExceptEdges();                      changeStep(8);
  stepBlur3();                                changeStep(9);
  stepBlurY();                                changeStep(10);

  stepPushAlpha();
  step3DAnalysis();

  changeStep(11);  tasktimer_.interruptRecording();
  substep_ = 0;
  
  if (DUMP_TIMES_TO_FILE)
    tasktimer_.dumpTimesToDisk("TaskTimer.txt");
}

inline int ratio(int blur) 
{ 
  return round(float (blur) * GlobalOptions.blur_ ); 
}

void SculptInProgress::stepBlur0()
{
  DEBUG(stpBur);
  *currentMap_ = PhotoSculptTools::blurMap(*currentMap_, ratio(INITIAL_BLUR_RANGE));
}

void SculptInProgress::stepBlur1()
{
  DEBUG(seplur);
  *currentMap_ = PhotoSculptTools::blurMap(*currentMap_, ratio(parameter_.blur1));
}

void SculptInProgress::stepBlur2()
{
  DEBUG(stpBur);
  *currentMap_ = PhotoSculptTools::blurMap(*currentMap_, ratio(parameter_.blur2));
}

void SculptInProgress::stepBlur3()
{
  DEBUG(sepBu3);
  *currentMap_ = PhotoSculptTools::blurMap(*currentMap_, ratio(parameter_.blur3));
}

void SculptInProgress::stepOpt1()
{
  DEBUG(seppt);
  optimiseMapKeepBestOnly( parameter_.displacementRange1, parameter_.scope1, 
                           parameter_.skip1, parameter_.qualityTarget );
}
void SculptInProgress::stepOpt2()
{
  DEBUG(steOp);
  optimiseMapKeepBestOnly( parameter_.displacementRange2, parameter_.scope2, 
                           parameter_.skip2, parameter_.qualityTarget );
}

void SculptInProgress::stepOpt3()
{
  DEBUG(teOt3);
  optimiseMapKeepBestOnly( parameter_.displacementRange3, parameter_.scope3, 
                           parameter_.skip3, parameter_.qualityTarget );
}
void SculptInProgress::stepSmartBlur()
{
  DEBUG(stpSartlur);
  currentMap_ = PhotoSculptTools::smartBlur(currentMap_, currentLeft_, 
                          parameter_.smartThreshold,ratio(parameter_.smartBlur));
}

void SculptInProgress::stepBlurExceptEdges()
{
  DEBUG(sepBurExeptges);
  PFloatMap tempMask  = PhotoSculptTools::computeEdgeDetection(*currentLeft_, 
                                                          ratio(RANGE_FOR_EDGE_DETECTION));

  currentMap_ = new VectorMap(PhotoSculptTools::blurMaskedMap(*currentMap_, *tempMask, 1, 
                              ratio(parameter_.edgeBlur), EDGE_MASK_THRESHOLD));
}

void SculptInProgress::stepBlurY()
{
  DEBUG(splrY);
  if (step_ < 5)
    currentMap_ = PhotoSculptTools::blurMapYOnly(currentMap_,ratio(Y_BLUR_VALUE));
}

#include "POVFinder.h"
#include "Convert.h"
#include "Perspective.h"
#include "Optimizer.h"

#define NB_POV_ANALYSIS_DURING_SCULPT 50

void SculptInProgress::step3DAnalysis()
{
  DEBUG("stp3Aayss");

  Optimizer optimizer;

  // second optim
  int NB_SAMPLES_PER_LINE[15] = { 1,  1,  19,   14, 17,   36, 17, 17,  12,   20, 20, 20, 20, 20, 20 };
  int MAX_SIZES          [15] = { 20, 12, 114,  71, 101, 115, 98, 188, 203, 300, 300, 300, 300, 300, 300 };
  int NB_ANALYSIS        [15] = { 5,  39, 64,   43, 41,   63, 54, 49,  33,   50, 50, 50, 50, 50, 50 };
  POV temppointofview = pointofview_;
  POV bestpointofview;

  optimizer.declareParameter(&NB_SAMPLES_PER_LINE [step_], 1);
  optimizer.declareParameter(&MAX_SIZES           [step_], 1);
  optimizer.declareParameter(&NB_ANALYSIS         [step_], 1);

  FloatVector parameterranges (9);
  parameterranges[0] = 0.001f;  // pos x
  parameterranges[1] = 0.001f;  // pos y
  parameterranges[2] = 0.01f;   // pos z
  parameterranges[3] = 0.0f;    // focal
  parameterranges[4] = 1.0f;    // azimuth angle
  parameterranges[5] = 1.0f;    // zenith angle
  parameterranges[6] = 0.1f;    // roll angle
  parameterranges[7] = 0.0f;    // zscale
  parameterranges[8] = 0.0f;    // distort

  if (step_ >= 6)
  {
    parameterranges[7] = 1.0f;      // zscale
    parameterranges[3] = 0.001f;    // focal
    parameterranges[8] = 0.001f;    // distort
  }

  //for (int k=0;k<1000;k++)
  {
    Timer timer;
    POVFinder finder;

    finder.init(*currentLeft_, *currentRight_, *alpha_, *currentMap_, pointofview_, MAX_SIZES[step_]);
    
    for (int i = 0 ; i<NB_ANALYSIS[step_] ; i++)
      temppointofview = finder.findBestView(parameterranges, NB_SAMPLES_PER_LINE[step_]);

    DEBUG_VALUE(" step", step_);
    DEBUG_VALUE(" time spent for step3DAnalysis(s)", timer.getSeconds());
    DEBUG_VALUE(" score obtained", finder.getCurrentBestScore());

    float score = min(timer.getSeconds(), 0.1f) + finder.getCurrentBestScore() + 
                  (float(MAX_SIZES[step_])+float(NB_ANALYSIS[step_])+float(NB_SAMPLES_PER_LINE[step_]))/100.0f;

    bool isimproved = optimizer.shuffleToMinimizeScore(score);
    if (isimproved)
      bestpointofview = temppointofview;

    if (NB_SAMPLES_PER_LINE[step_]<1) NB_SAMPLES_PER_LINE[step_]=1;
    if (MAX_SIZES[step_]<3)           MAX_SIZES[step_]=3;
    if (NB_ANALYSIS[step_]<1)         NB_ANALYSIS[step_]=1;
  }

  float bestscore = optimizer.getBestScore();
  
  optimizer.retrieveBestParameters();

  DEBUG_VALUE("  NB_SAMPLES_PER_LINE", NB_SAMPLES_PER_LINE[step_]);
  DEBUG_VALUE("  MAX_SIZES          ", MAX_SIZES[step_]);
  DEBUG_VALUE("  NB_ANALYSIS        ", NB_ANALYSIS[step_]);
  DEBUG_VALUE("  Best score         ", bestscore);

  pointofview_ = bestpointofview;
  /*
  if (step_>=1 && step_<=4)
  {
    FloatMap depths = convertToFloat ( getPerspCorrectedMap(*currentMap_, pointofview_) ) / MULTIPLY;
    // copies the result into the current map of coordinates
    *currentMap_ = pointofview_.generateMapOfCoordinates(depths);
  }*/
}

int SculptInProgress::getSubStep() const
{
  return substep_;
}

void SculptInProgress::stepPushAlpha()
{
  DEBUG(stpPuhApa);
  int sizex = currentMap_->getSizeX(), sizey = currentMap_->getSizeY();

  if ( ! alpha_ -> is_empty() )
  {
    int asx = alpha_ -> dimx(), asy = alpha_ -> dimy();

    for   (int y = 0; y < sizey; y ++)
      for (int x = 0; x < sizex; x ++)
      {
        UserInterrupt::stopIfOrdered();

        // read the alpha map
        int ax = round (float(x) * float(asx-1) / float(sizex-1));
        int ay = round (float(y) * float(asy-1) / float(sizey-1));

        if (alpha_->operator()(ax,ay,0,0) < 127) // black = background = we push
          currentMap_ -> setX(x,y, currentMap_ -> x(x,y) + 10);
      }
  }
}
