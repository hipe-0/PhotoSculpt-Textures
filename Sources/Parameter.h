// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once
#include "Obfuscate.h"

class Parameter
{
public:
  ofloat qualityTarget          ;

  oint   displacementRange1     ;
  oint   scope1                 ;
  oint   skip1                  ;

  oint   displacementRange2     ;
  oint   scope2                 ;
  oint   skip2                  ;

  oint   displacementRange3     ;
  oint   scope3                 ;
  oint   skip3                  ;

  float  displacementFalloff    ;
  float  pixelFalloff           ;
  float  falloffDisplaceStepDown;
  float  falloffRangeStepDown   ;

  oint   blur1                  ;
  oint   blur2                  ;
  oint   blur3                  ;
  oint   smartBlur              ;
  oint   edgeBlur               ;
  ofloat smartThreshold         ;
  oint   rangeOfKeepBest        ;
  ofloat fakeDepth              ;

  void print();
};