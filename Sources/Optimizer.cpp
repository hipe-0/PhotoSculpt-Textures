#include <math.h>
#include <stdlib.h>
#include <float.h>
#include "Optimizer.h"
#include "Trace.h"
#include "Round.h"
#include "Error.h"
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()

#define SEARCH_FADE 0.1f
#define SEARCH_RANGE_OVERSHOOT 2.0f  // search a little bit more than step value

float randminus1to1()
{
  return float(rand()) / float(RAND_MAX) * 2.0f  - 1.0f;
}

Optimizer::Optimizer()
{
  nbParams_  = 0;
  bestScore_ = FLT_MAX; // a very high value

  crossFade_       = SEARCH_FADE;
  searchOverShoot_ = SEARCH_RANGE_OVERSHOOT;
}

Optimizer::Optimizer(float crossfade, float searchovershoot)
{
  nbParams_  = 0;
  bestScore_ = FLT_MAX; // a very high value
  
  crossFade_       = crossfade;
  searchOverShoot_ = searchovershoot;
}

void Optimizer::declareParameter(float* parameter, float initialrange)
{
  ASSERT (nbParams_ >= NBPARAM, "Too many declarations, limit reached (in Optimizer2::declare)");
  
  bestParam_[nbParams_] = *parameter   ;
  bestStep_ [nbParams_] = initialrange ;
  params_   [nbParams_] = parameter    ;
  isFloat_  [nbParams_] = true         ;
  nbParams_ ++;
}

void Optimizer::declareParameter(int* parameter, int initialrange)
{
  ASSERT (nbParams_ >= NBPARAM,
    "Too many declarations, limit reached (in Optimizer2::declare)");
  
  bestParam_[nbParams_] = *(float*) parameter  ; // cast int* to float*
  bestStep_ [nbParams_] = float (initialrange) ;
  params_   [nbParams_] = (float*) parameter   ; // cast int* to float*
  isFloat_  [nbParams_] = false                ;
  nbParams_ ++;
}

bool Optimizer::shuffleToMinimizeScore(float score)
{
  bool isimproved = score < bestScore_;
  if (isimproved)
  {
    //DEBUG_WRITE("Best value found");
    for (int i=0; i<nbParams_; i++)
    {
      float newstep;
      if (isFloat_[i])
        newstep = abs ( *params_[i] - bestParam_[i] );
      else
        newstep = float( * ((int*) params_[i])) - bestParam_[i];

      bestStep_[i] = bestStep_[i] * (1.0f - crossFade_) + newstep * crossFade_ ;

      //DEBUG_VALUE(" >bestStep_", bestStep_[i]);
    }

    for (int i=0; i<nbParams_; i++)
    {
      if (isFloat_[i])
        bestParam_[i] = *params_[i];
      else
        bestParam_[i] = float (* ((int *)params_[i]));

      //DEBUG_VALUE(" >Parameter", *params_[i]);
    }

    bestScore_  = score;
    //DEBUG_VALUE(" >Score    ", bestScore_);
  }

  for (int i=0; i<nbParams_; i++)
    if (isFloat_[i])
      *params_[i] = bestParam_[i] + randminus1to1() * bestStep_[i] * searchOverShoot_; 
    else
      *(int *) params_[i] = round ( bestParam_[i] + randminus1to1() * bestStep_[i] * searchOverShoot_ );

  return isimproved;
}

void Optimizer::retrieveBestParameters()
{
  for (int i=0; i<nbParams_; i++)
    if (isFloat_[i])
      *params_[i] = bestParam_[i];
    else
      *(int *) params_[i] = round (bestParam_[i]);
}

float Optimizer::getBestScore()
{
  return bestScore_;
}

void testOptimizer()
{
  srand((unsigned int) time(0));  // Initialize random number generator.

  float crossfade = 0.02f, overshoot = 1.22f;
  Optimizer opt2;
  opt2.declareParameter(&crossfade, 0.1f);
  opt2.declareParameter(&overshoot, 0.1f);
 
  for (int k=0; k<1000 ; k++)
  {
    float finalscore = 0.0f;

    for (int l=0; l<100; l++)
    {
      float x=0,y=0;

      Optimizer optimizer(crossfade, overshoot);
      //Optimizer optimizer(0.2f, 2.0f); 
      optimizer.declareParameter(&x, 1.0f);
      optimizer.declareParameter(&y, 1.0f);

      for (int i=0; i<10000; i++)
      {
        float score = (x+20.0f) * (x+20.0f) + (y-50.0f) * (y-50.0f);
        optimizer.shuffleToMinimizeScore(score);
      }

      optimizer.retrieveBestParameters();
      //DEBUG_VALUE("best score", optimizer.getBestScore());
      finalscore += optimizer.getBestScore();
    }
    //DEBUG_VALUE("                    crossfade",  crossfade );
    //DEBUG_VALUE("                    overshoot",  overshoot );
    //DEBUG_VALUE("finalscore", finalscore);

    if (opt2.shuffleToMinimizeScore(finalscore))
    {
      DEBUG_VALUE("new best final score", finalscore);
    }
  }

  opt2.retrieveBestParameters();
  DEBUG_VALUE("best crossfade", crossfade);
  DEBUG_VALUE("best overshoot", overshoot);

  // one last trial
  float x=0,y=0;

  Optimizer o(crossfade, overshoot);
  o.declareParameter(&x, 1.0f);
  o.declareParameter(&y, 1.0f);

  for (int i=0; i<100; i++)
  {
    float score = (x+20.0f) * (x+20.0f) + (y-50.0f) * (y-50.0f);
    o.shuffleToMinimizeScore(score);
  }

  o.retrieveBestParameters();
  DEBUG_VALUE("x", x);
  DEBUG_VALUE("y", y);
}
