/*
 * ProgressionBar.h
 *
 *  Created on: 15 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#ifndef PROGRESSIONBAR_H_
#define PROGRESSIONBAR_H_

#include "Image.h"
#include "Obfuscate.h"

/// This object is used to display an horizontal bar that shows the progression of
///	the calculation, from 0% to 100%
///
class ProgressionBar
{
public:
  ProgressionBar();

  /// tells the ProgressionBar class where to output the display.
  ///	Warning : do not forget this important step before displaying anything
  ///
  void setDisplay(Image& displayimage);

  /// set the total number of steps currently in the progression
  /// this should be done once at the beginning
  void setStep(int nbstep, int step, int nbsubstep, int substep);

  void setCurrentProgress(float progress);
  void setCurrentProgressWithinStep(float subprogress);

  void draw();

  void setLineColor(int r,int g,int b);

  /// describes the shape of the horizontal bar
  void setGeometry(int xLeft, int yUp, int xRight, int yDown);

  void setPower(float power);

  /// tell if the 'setdisplay()' method has not been forgotten
  bool isValid() const;

private:
  void   drawSteps_();

  // image where to output the display
  Image* image_;

  oint   step_,subStep_,nbStep_,nbSubStep_;

  unsigned char linecolor_[3];

  oint   x1_,y1_,x2_,y2_;

  ofloat power_;
  ofloat lastProgress_;
};

#endif /* PROGRESSIONBAR_H_ */
