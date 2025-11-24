#include "Parameter.h"
#include "stdio.h"
#include "Trace.h"

void Parameter::print()
{
  char text[1000]="";
  sprintf_s(text, 1000,
    "{%.4ff,%d,%d,%d, %d,%d,%d, %d,%d,%d, %.2ff, %.2ff, %.2ff, %.2ff, %d,%d,%d, %d, %d, %.3ff, %d,%.3ff}, ", 
    float(qualityTarget), 
    int(displacementRange1),   int(scope1),   int(skip1),   
    int(displacementRange2),   int(scope2),   int(skip2),   
    int(displacementRange3),   int(scope3),   int(skip3), 
    float(displacementFalloff), float(pixelFalloff), 
    float(falloffDisplaceStepDown), float(falloffRangeStepDown), 
    int(blur1), int(blur2), int(blur3),  
    int(smartBlur),  int(edgeBlur), 
    float(smartThreshold), int(rangeOfKeepBest), float(fakeDepth) );

  DEBUG_WRITE(text);
}
