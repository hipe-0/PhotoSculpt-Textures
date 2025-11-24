#pragma once

#include "FloatVector.h"

class FunctionOptimizer
{
public:
  FunctionOptimizer();

  FloatVector minimize( float (*function)(FloatVector), FloatVector initialvalues, 
       int numberOfIteration, FloatVector mask);

  float getBestScore();

private:
  float       stepForVector_ ;
  FloatVector bestRange_     ;
  float       bestScore_     ;

};

class OptimizingFloat
{
public:
  OptimizingFloat(float initialvalue, char* filename);

  operator float() const;

  void optimizeWithLowScore(float score);

private:
  float currentTestValue_, bestValue_, bestScore_;
};