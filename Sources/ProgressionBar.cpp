/*
 * ProgressionBar.cpp
 *
 *  Created on: 22 avr. 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "ProgressionBar.h"
#include "Limit.h"
#include "Round.h"

ProgressionBar::ProgressionBar()
{
  image_        = 0 ;

  nbStep_       = 1 ;
  nbSubStep_    = 1 ;
  step_         = 0 ;
  subStep_      = 0 ;

  lastProgress_ = 0 ;

  power_        = 1.0f ;

  linecolor_[0] = 255 ;
  linecolor_[1] = 255 ;
  linecolor_[2] = 255 ;
}

void ProgressionBar::setDisplay(Image& displayimage)
{
  image_ = &displayimage;
}

void ProgressionBar::setStep(int nbstep, int step, int nbsubstep, int substep)
{
  nbStep_=nbstep;
  if (nbStep_<1)      nbStep_=1;

  nbSubStep_ = nbsubstep;
  if (nbSubStep_ < 1) nbSubStep_ = 1;

  step_=step;
  limit(step_,0, nbStep_);

  subStep_=substep;
  limit(subStep_,0, nbSubStep_);

  setCurrentProgressWithinStep(0.0f);
}

void ProgressionBar::setLineColor(int r,int g,int b)
{
  limit(r, 0, 255);  limit(g, 0, 255);  limit(b, 0, 255);

  linecolor_[0] = unsigned char(r);
  linecolor_[1] = unsigned char(g);
  linecolor_[2] = unsigned char(b);
}

void ProgressionBar::setGeometry(int xLeft, int yUp, int xRight, int yDown)
{
  x1_ = xLeft;
  y1_ = yUp;
  x2_ = xRight;
  y2_ = yDown;

  if (x1_>x2_) x1_ = x2_;
  if (y1_>y2_) y1_ = y2_;
}

void ProgressionBar::setPower(float power)
{
  limit(power, 0.01f, 100.0f);
  power_=power;
}

bool ProgressionBar::isValid() const
{
  return (image_ != NULL);
}

void ProgressionBar::setCurrentProgress(float progress)
{
  limit(progress, 0.0f, 1.0f);

  lastProgress_=progress;
}

void ProgressionBar::setCurrentProgressWithinStep(float subprogress)
{
  limit(subprogress, 0.0f, 1.0f);

  float valueWithinStep = (float(subStep_) + subprogress     ) / float(nbSubStep_);
  float valueAbsolute   = (float(step_)    + valueWithinStep ) / float(nbStep_   );

  setCurrentProgress(valueAbsolute);
}


void ProgressionBar::drawSteps_()
{
  // check isValid to avoid a display crash
  //
  if (isValid())
  {
    const unsigned char SUB_DIVISION_COLOR[] = { 0, 0, 0};

    // draw a horizontal line to support the divisions
    image_->draw_rectangle(x1_,y2_+4, x2_, y2_+4, SUB_DIVISION_COLOR);

    const float limit_high=pow(1.0f,power_);

    for (int i=0; i <= nbStep_; i++)
    {
      float value0to1 = float(i) / float(nbStep_);
      int locationAlongBar = int( round ( float(x2_ - x1_) * pow(value0to1,
                               power_) / limit_high) );

      image_->draw_rectangle(x1_+locationAlongBar,   y1_-2,
                             x1_+locationAlongBar, y2_+3,
                             SUB_DIVISION_COLOR);
    }
  }
}

void ProgressionBar::draw()
{
  if (isValid())  // to avoid a display crash
  {
    drawSteps_();

    const float limit_high=pow(1.0f,power_);

    const int sizeOfBar=int( round ( (x2_ - x1_) * pow(lastProgress_,power_) / limit_high) );

    image_->draw_rectangle(x1_,y1_,x1_+sizeOfBar, y2_,linecolor_);
  }
}
