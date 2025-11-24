/*
 * PhotoSculptTest.cpp
 *
 *  Created on: 22 April 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "PhotoSculpt.h"

void PhotoSculpt::searchForBestParametersForThisLevel_()
{
  const int NB_PARAMETER=9;

  // initial parameters to try
  int best[NB_PARAMETER]={5,1,6, 3,7,4, 3,1,1};

  float bestParameterQuality=0.005f;

  // initial score
  float bestQuality=1.0f; // starts with a totally wrong value of score

  // backups the map now.
  VectorMap backupCurrentMap = *(sculptInProgress_.getMap());


  for (unsigned long timerloop = GetTickCount();GetTickCount()-timerloop<180000;)
  {// loop for 3 minutes

    int test[NB_PARAMETER];

    // Changes parameters in a random fashion
    //
    for (int i=0; i<NB_PARAMETER ; i++)
    {
      // compute random values -2 ... +2 around the current best value
      test[i]=best[i]+rand()%5-2;

      // limits the values, values should be 1 < value < 7
      if (test[i]<1)
        test[i]=1;

      if (test[i]>7)
        test[i]=7;
    }

    float testParameterQuality=bestParameterQuality*(1.0f + float(rand()) / float(RAND_MAX) - 0.5f);

    if (testParameterQuality<0.0001f)
      testParameterQuality=0.0001f;

    if (testParameterQuality>1.0f)
      testParameterQuality=1.0f;


    // reguralily resets parameters to brand new random values, to try extreme values
    //
    for (int i=0; i<NB_PARAMETER ; i++)
      if (rand()%10==0)
        test[i] = rand()%4+1;


    if (rand()%10==0)
      testParameterQuality = float(rand())/float(RAND_MAX)/10.0f+.00001f;

    // Recovers the Map just before optimisation so we always start working on the same map
    sculptInProgress_.setMap( backupCurrentMap );



    // optimises 3 times using all parameters
    // 1
    sculptInProgress_.optimiseMapKeepBestOnly(test[0],test[1],test[2],testParameterQuality);
    // 2
    sculptInProgress_.optimiseMapKeepBestOnly(test[3],test[4],test[5],testParameterQuality);
    // 3
    sculptInProgress_.optimiseMapKeepBestOnly(test[6],test[7],test[8],testParameterQuality);

    float testQuality=getScore();
    float testPercent=getGoodPixelPercent(0.01f);


    // Tests if we found a new interresting value (tested score is smaller than last best score found)
    //
    if (testQuality<bestQuality)
    {
      // we store the state of all parameters
      for (int i=0; i<NB_PARAMETER ; i++)
        best[i]=test[i];

      bestParameterQuality=testParameterQuality;

      // specifies that the new best score is this test score
      bestQuality=testQuality;

      printf(">> score = %.5f (%.3f %% of image) trying q%.4f %d %d %d  >> %d %d %d >>  %d %d %d \n",
          testQuality, testPercent*100.0f, testParameterQuality,
          test[0],test[1],test[2],test[3],test[4],test[5],test[6],test[7],test[8]);
    }
  }

  // Recovers the Map just after optimisation so we continue with the correct map
  sculptInProgress_.setMap( backupCurrentMap );
}
