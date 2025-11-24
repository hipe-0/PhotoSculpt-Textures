#pragma once

float randminusnton(float n);

#define NBPARAM 8

class Optimizer
{
public:
  Optimizer();
  Optimizer(float crossfade, float searchovershoot);
  void  declareParameter(float* parameter, float initialrange);
  void  declareParameter(int*   parameter, int   initialrange);
  bool  shuffleToMinimizeScore(float score);
  void  retrieveBestParameters();
  float getBestScore();

protected:
  int   nbParams_;
  float* params_[NBPARAM];
  float bestParam_[NBPARAM]; // stores the best parameter value ever seen
  float bestStep_ [NBPARAM]; // stores the step value that was done to get the best parameter
  bool  isFloat_  [NBPARAM];
  float bestScore_; // stores the best score that's associated with the best parameter
  float crossFade_, searchOverShoot_;
};

void testOptimizer();