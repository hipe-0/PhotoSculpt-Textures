/*
 * MultiTaskingTrial.cpp
 *
 *  Created on: 2011/01/10
  *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include "MultiTasking.h"
#include "Image.h"
#include "Trace.h"
#include "Error.h"

#include "MultitaskingTrial.h"

// 0 red, 1 green, 2 blue, 3 yellow
static int colors[4][3] = { { 255 , 0, 0 } , { 0, 255, 0 } , { 0, 0, 255 } , { 255, 255, 0 } } ;

void MultiTaskingTrial::task(int line)
  {
    for (int i=0; i < image_.dimx(); i++)
    {
      unsigned char currentcolor = image_(i, line, 0, 0);
      if (currentcolor != 128)
      {
        MessageBox(0, "A line was painted twice during this process", "Multitasking error", MB_OK);
        break;
      }
      image_(i, line, 0, 0) = colors[0][0] ;
      image_(i, line, 0, 1) = colors[0][1] ;
      image_(i, line, 0, 2) = colors[0][2] ;
    }
  }

#define MULTITASKING_TRIAL_NB_TESTS 100

void MultiTaskingTrial::launch()
  {
    DEBUG(MultiTa);

    for (int n=0; n<MULTITASKING_TRIAL_NB_TESTS ; n++)
    {
      DEBUG_VALUE("n", n );
   
      image_ = Image(2000,1000,1,3);
      image_.fill(128); // fill with gray

      launchTask(image_.dimy());

      for (int i=0; i < image_.dimy(); i++)
        if (image_(0, i, 0, 0) == 128)
          MessageBox(0, "A line was forgotten during this process", "Multitasking error", MB_OK);

      if (n == MULTITASKING_TRIAL_NB_TESTS-1) // save last image for record
        SAVE_IMAGE(image_, "MultiTaskingTest.bmp");

      image_ = Image();
    }
  
    MESSAGE("Multitasking test 100% completed with success");
  }

