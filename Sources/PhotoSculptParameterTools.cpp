#include "Limit.h"
#include "PhotoSculpt.h"
#include "Mix.h"

const int   RESET_PARAMETER_EVERY     = 15 ;
const int   CHANGE_PARAMETER_EVERY    = 5  ;

void variate(oint& i, int min, int max)
{
  // Changes parameters in a random fashion
  //
  if (rand() % CHANGE_PARAMETER_EVERY == 0)
    // compute random values -1 ... +1 around the current best value
    i+=rand()%7-3;

  if (rand() % RESET_PARAMETER_EVERY == 0)
    i = rand() % 80 + 1;  

  // limits the values, values should be 1 <= value <= 7
  limit(i, min, max);
}

#ifndef OBFUSCATED_VARIABLES_ARE_CLEAR
void variate(ofloat& i, float min, float max)
{
  if (rand() % CHANGE_PARAMETER_EVERY == 0)
    // compute random values within -10% ... +10% of the current best value
    i*=1.0f + 0.4f * float(rand()) / float(RAND_MAX) - 0.2f ;


  if (rand() % RESET_PARAMETER_EVERY == 0)
    i = float(rand()) / float(RAND_MAX) / 10.0f + .00001f;

  limit(i,min, max);
}
#endif

void variate(float& i, float min, float max)
{
  if (rand() % CHANGE_PARAMETER_EVERY == 0)
    // compute random values within -10% ... +10% of the current best value
    i*=1.0f + 0.4f * float(rand()) / float(RAND_MAX) - 0.2f ;


  if (rand() % RESET_PARAMETER_EVERY == 0)
    i = float(rand()) / float(RAND_MAX) / 10.0f + .00001f;

  limit(i,min, max);
}

void variate(Parameter& i)
{
  variate(i.qualityTarget           , 0.001f  , 1.0f);

  variate(i.displacementRange1      , 1       , 70   );
  variate(i.scope1                  , 1       , 150 );
  variate(i.skip1                   , 1       , 5   );

  variate(i.displacementRange2      , 0       , 70   );
  variate(i.scope2                  , 1       , 150  );
  variate(i.skip2                   , 1       , 5   );

  variate(i.displacementRange3      , 0       , 70   );
  variate(i.scope3                  , 1       , 150  );
  variate(i.skip3                   , 1       , 5   );

  variate(i.displacementFalloff     , 0.01f   , 1.0f);
  variate(i.pixelFalloff            , 0.01f   , 1.0f);

  variate(i.falloffDisplaceStepDown , 0.01f   , 1.0f);  
  variate(i.falloffRangeStepDown    , 0.01f   , 1.0f);

  variate(i.blur1                   , 0       , 150   );
  variate(i.blur2                   , 0       , 150   );
  variate(i.blur3                   , 0       , 150   );

  variate(i.smartBlur               , 1       , 150  );
  variate(i.smartThreshold          , 0.0001f , 0.1f);
  variate(i.rangeOfKeepBest         , 0       , 150   );
}

void mix(Parameter &result, const Parameter& a, const Parameter& b, float amount=0.5f)
{
  //result.qualityTarget      = mix(a.qualityTarget,      b.qualityTarget,      amount);

  result.displacementRange1 = mix(a.displacementRange1, b.displacementRange1, amount);
  result.scope1             = mix(a.scope1,             b.scope1,             amount);
  result.skip1              = mix(a.skip1,              b.skip1,              amount);

  result.displacementRange2 = mix(a.displacementRange2, b.displacementRange2, amount);
  result.scope2             = mix(a.scope2,             b.scope2,             amount);
  result.skip2              = mix(a.skip2,              b.skip2,              amount);

  result.displacementRange3 = mix(a.displacementRange3, b.displacementRange3, amount);
  result.scope3             = mix(a.scope3,             b.scope3,             amount);
  result.skip3              = mix(a.skip3,              b.skip3,              amount);

  //result.displacementFalloff= mix(a.displacementFalloff, b.displacementFalloff,amount);
  //result.pixelFalloff       = mix(a.pixelFalloff,        b.pixelFalloff,      amount);

  //result.falloffDisplaceStepDown = mix(a.falloffDisplaceStepDown, b.falloffDisplaceStepDown, amount);
  //result.falloffRangeStepDown    = mix(a.falloffRangeStepDown,    b.falloffRangeStepDown,    amount);

  result.blur1              = mix(a.blur1,               b.blur1,             amount);
  result.blur2              = mix(a.blur2,               b.blur2,             amount);
  result.blur3              = mix(a.blur3,               b.blur3,             amount);
  result.smartBlur          = mix(a.smartBlur,           b.smartBlur,         amount);
  //result.smartThreshold     = mix(a.smartThreshold,      b.smartThreshold,    amount);
  result.rangeOfKeepBest    = mix(a.rangeOfKeepBest,     b.rangeOfKeepBest,   amount);
}