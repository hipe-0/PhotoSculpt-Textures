
#include "PhotoSculpt.h"
#include "Timer.h"
#include "Round.h"
#include "Perspective.h"
#include "PhotoSculptTools.h"
#include "Trace.h"

#include <iostream>
#include <fstream>

using namespace std;

// sets how many images are to be tested in the self test
const int   NB_MODELS = 15 ; // all = 15
const int   NB_STEPS  = 5  ; // 7 is maximum due to file sizes

// set 1.0f --> 1 s of run costs 1 percent of score
// set 0.1f --> 1 s =0.1 percent of score 
// set 0.0f --> seconds not to affect percent of score at all
const float RATIO_PERCENT_TO_SECONDS = 0.01f;

const float GOOD_PIXEL_THRESHOLD     = 0.01f;

void  variate (int& i, int min, int max        );
void  variate (float& i, float min, float max  );
void  variate (Parameter& i                    );
float mix     ( float a, float b, float amount );
int   mix     ( int a, int b, float amount     );
void  mix     (Parameter &result, const Parameter& a, const Parameter& b, float amount=0.5f);

void PhotoSculpt::searchForAllParameters()
{
  float refPercent ;
  float refTime    ;

  PhotoSculpt sculpt[NB_MODELS];

  printf("Reading %d couples of images for testing, please wait ...\n",
           NB_MODELS);

  // stores all original images in a table
  Image originalleftimages [NB_MODELS];
  Image originalrightimages[NB_MODELS];

  for (int i=0; i< NB_MODELS ; i++)
  {
    char nameLeft [50] = "";
    char nameRight[50] = "";
    sprintf(nameLeft,  "Optional SelfTests/Test%dL.jpg",i+1);
    sprintf(nameRight, "Optional SelfTests/Test%dR.jpg",i+1);

    originalleftimages [i].load( nameLeft  );
    originalrightimages[i].load( nameRight );
    sculpt[i].clear();
    sculpt[i].init(originalleftimages [i], originalrightimages[i]);
  }

  DEBUG_WRITE("Loading completed, testing starting, please wait...\n\n");

  Parameter bestparameters[NB_STEPS];

  for (int i=0; i<NB_STEPS ; i++)
    bestparameters[i]=RECIPE[i];
  
  float bestMASKED_BLUR_THRESHOLD    = MASKED_BLUR_THRESHOLD    ;
  float bestMASKED_BLUR_THRESHOLD2   = MASKED_BLUR_THRESHOLD2   ;
  int   bestMASKED_BLUR_RANGE        = MASKED_BLUR_RANGE        ;
  int   bestRANGE_FOR_EDGE_DETECTION = RANGE_FOR_EDGE_DETECTION ;
  float bestEDGE_MASK_THRESHOLD      = EDGE_MASK_THRESHOLD      ;
  int   bestRANGE_FOR_EDGE_BLUR      = RANGE_FOR_EDGE_BLUR      ;
  float bestTHRESHOLD_EDGE_BLUR      = THRESHOLD_EDGE_BLUR      ;

  // initial score
  float bestQuality=0.0f ; // starts with a totally wrong value of percent score

  long int count=0;

  while (1) // endless optimisation
  {
    Parameter testparameters[NB_STEPS];

    for (int i=0; i<NB_STEPS ; i++)
      testparameters[i]=bestparameters[i];

    float testMASKED_BLUR_THRESHOLD    = bestMASKED_BLUR_THRESHOLD    ;
    float testMASKED_BLUR_THRESHOLD2   = bestMASKED_BLUR_THRESHOLD2   ;
    int   testMASKED_BLUR_RANGE        = bestMASKED_BLUR_RANGE        ;
    int   testRANGE_FOR_EDGE_DETECTION = bestRANGE_FOR_EDGE_DETECTION ;
    float testEDGE_MASK_THRESHOLD      = bestEDGE_MASK_THRESHOLD      ;
    int   testRANGE_FOR_EDGE_BLUR      = bestRANGE_FOR_EDGE_BLUR      ;
    float testTHRESHOLD_EDGE_BLUR      = bestTHRESHOLD_EDGE_BLUR      ;

    // forces the low level skip value to be small to avoid skipping detail
    //testparameters[0].skip1=1;
    testparameters[0].skip2=1;
    //testparameters[0].skip3=1;
  	
    if (count>0)
    {
      for (int i=0; i<NB_STEPS ; i++)
	     variate(testparameters[i]);

      const int FADE_TO_STEP_NO = 4;

      for (int i=1; i<FADE_TO_STEP_NO ; i++)
      {
	      float amount= float( FADE_TO_STEP_NO+1-i ) / float( FADE_TO_STEP_NO+1 );

	      mix(testparameters[i], testparameters[0], testparameters[FADE_TO_STEP_NO], amount );
      }

      //variate    (testMASKED_BLUR_THRESHOLD   , 0.0001f   , 1.0f  ) ;
      //variate    (testMASKED_BLUR_THRESHOLD2  , 0.0001f   , 1.0f  ) ;
      //variate    (testMASKED_BLUR_RANGE       , 1         , 80    ) ;
      //variate    (testDEFAULT_THRESHOLD       , 0.0001f   , 1.0f  ) ;
      //variate    (testRANGE_FOR_EDGE_DETECTION, 10        , 30    ) ;
      //variate    (testEDGE_MASK_THRESHOLD     , 0.0001f   , 3.0f  ) ;
      //variate    (testRANGE_FOR_EDGE_BLUR     , 0         , 150   ) ;
      //variate    (bestTHRESHOLD_EDGE_BLUR     , 0.001f    , 3.0f  ) ;
    }

    // inits the global score for all levels and all images
    //float totalQuality     = 0.0f;
    float totalGoodPercent = 0.0f;

    Timer time;

    // Testing all images
    for (int modelnumber=0; modelnumber<NB_MODELS ; modelnumber++)
    {
      int lastStep=sculpt[modelnumber].getMaxLevel();

      if (lastStep>NB_STEPS-1)
        lastStep=NB_STEPS-1;

      MASKED_BLUR_THRESHOLD   = testMASKED_BLUR_THRESHOLD    ;
      MASKED_BLUR_THRESHOLD2  = testMASKED_BLUR_THRESHOLD2   ;
      MASKED_BLUR_RANGE       = testMASKED_BLUR_RANGE        ;
      RANGE_FOR_EDGE_DETECTION= testRANGE_FOR_EDGE_DETECTION ;
      EDGE_MASK_THRESHOLD     = testEDGE_MASK_THRESHOLD      ;
      RANGE_FOR_EDGE_BLUR     = testRANGE_FOR_EDGE_BLUR      ;
      THRESHOLD_EDGE_BLUR     = testTHRESHOLD_EDGE_BLUR      ;

      // Testing a number of steps for each image
      for (int step=0; step<=lastStep; step++)
      {
	      if (step==lastStep)
        {
	        testparameters[step].blur3          = 50;
	        testparameters[step].smartBlur      = 70;
	        testparameters[step].smartThreshold = 0.080f;
	      }

	      sculpt[modelnumber].growAndOptimizeThisLevel(testparameters[step]);
      } // end for step

      float testGoodPercent = sculpt[modelnumber].getGoodPixelPercent(GOOD_PIXEL_THRESHOLD);

      // TotalQuality is the sum of all image quality
      totalGoodPercent += testGoodPercent;
    } // end for s

    float timePassedForAllImages = time.getSeconds();

    float meanGoodPercent  = totalGoodPercent       / NB_MODELS;
    float meanTime         = timePassedForAllImages / NB_MODELS;

    float overalScore      = meanGoodPercent - meanTime * RATIO_PERCENT_TO_SECONDS;
    // inits the reference values
    if (count==0)
    {
	    refPercent = meanGoodPercent;
	    refTime    = meanTime;
    }
    count++;

    DEBUG_VALUE("Score", overalScore);
    DEBUG_VALUE("Meantime (s)", meanTime);

    //////// Tests if we found a new interresting value (tested score is
    //       smaller than last best score found)
    //
    if (overalScore > bestQuality)
    {
      for (int step=0; step<NB_STEPS ; step++)
        bestparameters[step]=testparameters[step];

      bestMASKED_BLUR_THRESHOLD  = testMASKED_BLUR_THRESHOLD  ;
      bestMASKED_BLUR_THRESHOLD2 = testMASKED_BLUR_THRESHOLD2 ;
      bestMASKED_BLUR_RANGE      = testMASKED_BLUR_RANGE      ;

      // specifies that the new best score is this test score
      bestQuality=overalScore;

      DEBUG_WRITE("\n\n//>>>>>>>>>>> new best overall score = <<<<<<<<<<<<\n");
      DEBUG_VALUE("  Overall score", overalScore);

      float improvingPercent = (meanGoodPercent - refPercent)
                            / refPercent * 100.0f  ;
      float improvingTime    = (refTime - meanTime)
                            / refTime * 100.0f     ;

      DEBUG_VALUE("NB_MODELS", NB_MODELS);
      DEBUG_VALUE("NB_STEPS", NB_STEPS);
      DEBUG_VALUE("meanGoodPercent", meanGoodPercent);
      DEBUG_VALUE("improvingPercent", improvingPercent);
      DEBUG_VALUE("meanTime", meanTime);
      DEBUG_VALUE("improvingTime", improvingTime);

      DEBUG_VALUE("//    number of recipes tested so far", count);

      // Outputs the choosen parameters to console
      DEBUG_WRITE("//    best parameters are now following :\n");

      for (int modelnumber=0; modelnumber<NB_MODELS ; modelnumber++)
      {
        PIntMap corrected = sculpt[modelnumber].computeIntMap();
        Image displace=ImageGeneration::generateDepth  (*corrected);
        *corrected = IntMap(); // free memory
        char name[200]={0};
        sprintf(name,"Test-best_displace%d.bmp",modelnumber);

        displace.save(name);

        Image morphed=PhotoSculptTools::generateMorphed(sculpt[modelnumber].computeRightImage(),
                                                        sculpt[modelnumber].getMap());

        char value[200]={0};
        float testGoodPercent = sculpt[modelnumber].getGoodPixelPercent(GOOD_PIXEL_THRESHOLD);
        sprintf(value," %.2f %%", testGoodPercent);
        unsigned char textColor[3] = {255, 0, 0 }; // red
        morphed.draw_text(value,0,0,textColor);

        sprintf(name,"Test-best_morphed%d.bmp",modelnumber);
        morphed.save(name);
      }

      for (int step=0; step<NB_STEPS ; step++)
      {
        Parameter parameters=bestparameters[step];

        parameters.print();

        DEBUG_WRITE("-");
      }

      DEBUG_VALUE("float PhotoSculpt::MASKED_BLUR_THRESHOLD    ", bestMASKED_BLUR_THRESHOLD   );
      DEBUG_VALUE("float PhotoSculpt::MASKED_BLUR_THRESHOLD2   ", bestMASKED_BLUR_THRESHOLD2  );
      DEBUG_VALUE("int   PhotoSculpt::MASKED_BLUR_RANGE        ", bestMASKED_BLUR_RANGE       );
      DEBUG_VALUE("int   PhotoSculpt::RANGE_FOR_EDGE_DETECTION ", bestRANGE_FOR_EDGE_DETECTION);
      DEBUG_VALUE("float PhotoSculpt::EDGE_MASK_THRESHOLD      ", bestEDGE_MASK_THRESHOLD     );
      DEBUG_VALUE("int   PhotoSculpt::RANGE_FOR_EDGE_BLUR      ", bestRANGE_FOR_EDGE_BLUR     );
      DEBUG_VALUE("float PhotoSculpt::THRESHOLD_EDGE_BLUR      ", bestTHRESHOLD_EDGE_BLUR     );

      DEBUG_WRITE("-");
    }

    // resets all maps
    for (int modelnumber=0; modelnumber<NB_MODELS ; modelnumber++)
      sculpt[modelnumber].resetMap();
  }
}


/*
//>>>>>>>>>>> new best overall score = <<<<<<<<<<<<

  Overall score=74.365639
NB_MODELS=3
NB_STEPS=5
meanGoodPercent=74.489143
improvingPercent=39.108047
meanTime=12.350333
improvingTime=14.164254
//    number of recipes tested so far=126
//    best parameters are now following :

{0.0781f,19,45,4, 58,70,1, 10,13,1, 0.04f, 0.07f, 0.03f, 0.04f, 34,38,60, 35, 150, 0.075f, 75,0.000f}, 
-
{0.0259f,26,43,3, 59,69,1, 17,21,1, 0.03f, 0.11f, 0.04f, 0.09f, 31,38,52, 41, 150, 0.016f, 68,0.000f}, 
-
{0.0167f,33,41,3, 59,68,1, 23,29,1, 0.08f, 0.04f, 0.10f, 0.10f, 28,38,44, 48, 150, 0.098f, 60,0.000f}, 
-
{0.0205f,40,38,2, 60,68,1, 30,37,2, 0.07f, 0.06f, 0.04f, 0.02f, 26,38,36, 54, 150, 0.078f, 53,0.000f}, 
-
{0.0207f,54,34,1, 61,66,1, 43,53,2, 0.01f, 0.04f, 0.03f, 0.08f, 20,38,50, 70, 150, 0.080f, 38,0.000f}, 
-
float PhotoSculpt::MASKED_BLUR_THRESHOLD    =0.072000
float PhotoSculpt::MASKED_BLUR_THRESHOLD2   =0.500000
int   PhotoSculpt::MASKED_BLUR_RANGE        =37
int   PhotoSculpt::RANGE_FOR_EDGE_DETECTION =15
float PhotoSculpt::EDGE_MASK_THRESHOLD      =2.985000
int   PhotoSculpt::RANGE_FOR_EDGE_BLUR      =150
float PhotoSculpt::THRESHOLD_EDGE_BLUR      =3.000000
*/