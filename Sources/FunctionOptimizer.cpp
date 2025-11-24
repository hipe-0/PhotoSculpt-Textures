#include "FunctionOptimizer.h"
#include <stdlib.h>
#include "UserInterrupt.h"
#include "Limit.h"
#include "Trace.h"
#include "Image.h"

FloatVector bestRange_;

void writeToImageFile(FloatVector range, char * filename)
{
    Image image(150,120,1,3);
    image.fill(255);
    for (int i = 0; i < range.getSize() ; i ++ )
    {
      unsigned char color[3] = {0,255,0};
      char txt[100]="";
      sprintf(txt, "%d : %.5f ", i, range[i]);
      image.draw_text(txt, 0, i*10, color);
    }
    DEBUG_SAVE_IMAGE(image , filename   );
}

FunctionOptimizer::FunctionOptimizer()
{
  stepForVector_ = 0.1f;
}

FloatVector FunctionOptimizer::minimize( float (*function)(FloatVector),
       FloatVector initialvalues, 
       int numberOfIteration, const FloatVector mask)
{
  int         size      = initialvalues.getSize();

  FloatVector bestparam = initialvalues;
  FloatVector range(size);

  if (bestRange_.isEmpty())
  {
    range.fill(0.1f);
    range = range * mask;
    bestRange_ = range;
  }
  else
    range = bestRange_ ;

  float initscore = function (initialvalues);
  bestScore_ = initscore;

  for (int i = 0 ; i < numberOfIteration ; i++)
  {
    // randomizes the range
    FloatVector random(size);
    random.randomizeFrom0To1();
    range = bestRange_ * (random + 0.5f);

    UserInterrupt::stopIfOrdered();

    // Phase 1: Random base jump
    float       bestscore2 = initscore * 4.0f ;
    FloatVector bestparam2 = bestparam ;
    bool isimproved = false;
    for (int j = 0 ; j<100 ; j++)
    {
      UserInterrupt::stopIfOrdered();
      FloatVector random(size);
      random.randomizeFrom0To1();

      float bonus = 1.0f;
      if (::rand() % 20 == 0 ) bonus = 10.0f;
      if (::rand() % 20 == 0 ) bonus = 0.1f;

      FloatVector newtestparam = bestparam + (random * 2.0f - 1.0f) * range / 2.0f * bonus;

      float newscore = function(newtestparam);
      if (newscore < bestscore2)
      {
        bestscore2 = newscore     ;
        bestparam2 = newtestparam ;
        isimproved = true;
      }
    }

    if (isimproved)
    {
      // Phase 2: Hill climbing
      float dx = 0.00001f;
      FloatVector derivatedvalue(size);
    
      // computes the derivative of the function "function"
      for (int k = 0 ; k < size ; k++ )
      {
        UserInterrupt::stopIfOrdered();
        FloatVector testparamR =  bestparam2, testparamL =  bestparam2;
        testparamR[k] += dx ;
        testparamL[k] -= dx ;

        if (mask[k] != 0.0f)
          derivatedvalue[k]     =  (function(testparamL) - function(testparamR) ) / 2.0f / dx * mask[k];
        else
          derivatedvalue[k]     = 0.0f;
      }

      float length = derivatedvalue.getLength();

      FloatVector vector = range;
      if (length > 0.0f)
        vector = derivatedvalue / length;

      char derivatedrangetext[200] = "  vector   ";
      vector.writeToString(derivatedrangetext, 200);

      bool isexit = false;
      int count = 0;
      while (isexit == false)
      {
        UserInterrupt::stopIfOrdered();

        FloatVector newtestparam = bestparam2 + vector * stepForVector_;

        float newscore = function(newtestparam);
        if (newscore < bestscore2)
        {
          bestparam2         = newtestparam  ;
          bestscore2         = newscore ;

          // computes the derivative of the function "function"
          for (int k = 0 ; k < size ; k++ )
          {
            UserInterrupt::stopIfOrdered();
            FloatVector testparamR =  bestparam2, testparamL =  bestparam2;
            testparamR[k]          += dx ;
            testparamL[k]          -= dx ;

            if (mask[k] != 0.0f)
              derivatedvalue[k]     =  (function(testparamL) - function(testparamR) ) / 2.0f / dx * mask[k];
            else
              derivatedvalue[k]     = 0.0f;
          }

          float length = derivatedvalue.getLength();

          if (length > 0.0f)
            vector = derivatedvalue / length;

          char vectortext[200] = "  vector   ";
          vector.writeToString(vectortext, 200);

          stepForVector_ *= 2.0f ;

          isexit = false;
        }
        else
          stepForVector_ /=2.0f;
        

        if (stepForVector_ <= 0.00001f)
          isexit = true;

        if (count++ > 20 )
          isexit = true;
      }

      // Phase 3: record if best solution so far
      if (bestscore2 < bestScore_)
      {
        bestScore_ = bestscore2;
        bestparam  = bestparam2;
        bestRange_ = (bestRange_ * 0.9f) + (range * 0.1f);
      }
    }
  }

  return bestparam;
}

float FunctionOptimizer::getBestScore()
{
  return bestScore_;
}