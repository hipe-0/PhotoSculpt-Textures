/* Constants.cpp
 *
 *  Created on: 06 jan 2011
 *  Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_
#pragma once

#include "constants.h"

const bool          DUMP_TIMES_TO_FILE       = false;

const oint          PROGRESSION_BAR_LEFT     = 87   ;
const oint          PROGRESSION_BAR_UP       = 9    ;
const oint          PROGRESSION_BAR_RIGHT    = 384  ;  
const oint          PROGRESSION_BAR_DOWN     = 10   ;
const unsigned char MAIN_COLOR_R             = 218  ;
const unsigned char MAIN_COLOR_G             = 120  ;
const unsigned char MAIN_COLOR_B             = 50   ;
const oint          PROGRESSION_BAR_TEXT_X   = 317  ;
const oint          PROGRESSION_BAR_TEXT_Y   = 4    ;

const oint          ID_ANIMATION_TIMER       = 58   ;
const oint          NB_MILISECONDS_ANIMATION = 66   ;

const int           MIN_WINDOW_SIZE          = 400  ;
const int           MIN_WINDOW_OVERLAP       = 200  ;

const oint          IMAGE_BLANK_FRAME        = 20   ;
const int           NB_RECENT_FILES          = 9    ;

//  qualityTarget displacementRange1   scope1   skip1   displacementRange2   
//  scope2   skip2   displacementRange3  scope3  skip3  displacementFalloff 
//  pixelFalloff  falloffDisplaceStepDown falloffRangeStepDown  blur1  blur2 
//  blur3 smartBlur edgeBlur smartThreshold   rangeOfKeepBest 

Parameter RECIPE[]=
{
{0.0759f,38,44,2, 41,71,1, 13,10,1, 0.01f, 0.08f, 0.03f, 0.06f, 100,30,60,  37, 150, 0.100f, 19, 0.000f},
{0.0021f,42,43,2, 43,70,1, 17,21,1, 0.05f, 0.10f, 0.05f, 0.08f, 100,35,60,  44, 150, 0.016f, 23, 0.000f},
{0.0198f,46,43,2, 45,70,1, 21,32,1, 0.05f, 0.02f, 0.02f, 0.01f, 100,40,60,  50, 150, 0.015f, 26, 0.000f},
{0.0205f,49,42,1, 46,69,1, 26,43,1, 0.06f, 0.01f, 0.03f, 0.03f, 100,45,60,  57, 150, 0.068f, 30, 0.000f},
{0.0176f,57,41,1, 50,68,1, 34,65,1, 0.01f, 0.04f, 0.03f, 0.01f, 100,55,60,  70, 150, 0.080f, 37, 0.000f},
{0.0067f,70,47,1, 47,20,5, 60,46,1, 0.09f, 0.04f, 0.07f, 0.05f, 100,50,70,  70, 150, 0.080f, 26, 0.010f},

{0.0055f,70,60,5, 10,30,1, 10,20,1, 0.03f, 0.02f, 0.10f, 0.05f, 100,50,60,  60, 150, 0.100f, 60, 0.008f},  // step 6
{0.0200f,30,30,5, 20,20,3, 0,0,0,   0.03f, 0.02f, 0.10f, 0.00f, 100,50,40,  60, 150, 0.100f, 40, 0.005f},  // step 7 

{0.0300f,10,20,1, 0,0,0,   0,0,0,   0.03f, 0.02f, 0.10f, 0.00f, 100, 0,40,  40, 150, 0.100f, 40, 0.004f},  // step 8
{0.0400f,10,20,1, 0,0,0,   0,0,0,   0.03f, 0.02f, 0.10f, 0.00f, 80,  0,30,  30,  50, 0.100f, 30, 0.003f},  // step 9
{0.0400f,10,20,1, 0,0,0,   0,0,0,   0.03f, 0.02f, 0.10f, 0.00f, 60,  0,30,  30,  20, 0.100f, 20, 0.003f}   // step 10
}; 

float               MASKED_BLUR_THRESHOLD         = 0.072f  ;
ofloat              MASKED_BLUR_THRESHOLD2        = 0.500f  ;
int                 MASKED_BLUR_RANGE             = 37      ;
oint                RANGE_FOR_EDGE_DETECTION      = 15      ;
ofloat              EDGE_MASK_THRESHOLD           = 2.98500f;
oint                RANGE_FOR_EDGE_BLUR           = 150     ;
ofloat              THRESHOLD_EDGE_BLUR           = 3.00000f;

oint                MIN_IMAGE_SIZE                = 10      ;
ofloat              BLUR_IMAGES_VALUE             = 0.5f    ;

oint                VERTICAL_SEARCH_LIMIT         = 1       ;

ofloat              EXPOSURE_BLUR_RATIO           = 10.0f   ;
ofloat              EXPOSURE_FADE                 = 1.0f    ;

oint                MASKED_BLUR_COUNT            = 9999     ; // big value == unlimited

oint                Y_BLUR_VALUE                 = 30       ;

ofloat              MIX_QUALITY_TOLERANCE        = 0.0000f  ; // default

ofloat              DEFAULT_THRESHOLD            = 0.5f     ;
ofloat              IGNORE_VALUE                 = 0.00006f ;

oint                INITIAL_BLUR_RANGE           = 100      ;
const oint          NB_SUB_STEP                  = 10       ;

static const int    NB_MAX_LEVELS                = 11       ;

float INTERRUPT_FACTOR[INTERRUPT_NB]= { 
		    2.0f , 2.0f , 
		    2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 
		    2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f, 2.0f,
		    1.5f };

const oint          MAX_PIXEL_IN_IMAGE           = 15000000 ;

float               FAKE_BLUR_RANGES[5] = { 20.0f , 10.0f  , 5.0f  , 2.5f  , 0.0f  };
float               FAKE_BLUR_FADES [5] = { 1.0f  ,  0.45f , 0.25f , 0.25f , 0.16f };

extern const int    BURSTMAP_RANGE_IN_PIXEL      = 15       ;
extern const int    BURSTMAP_FIXED_COMA          = 10       ;


const ofloat        PYRAMID_BLUR_IMAGES_VALUE    = 0.5f     ;

const unsigned char TEXT_MUTED_COLOR  []      = { 50, 50, 75    };
const unsigned char TEXT_BRIGHT_COLOR []      = { 200, 200, 250 };
const unsigned char TEXT_ALPHA        []      = { 240           };
const int           SWITCH_MUTED_BRIGHT_COLOR = 100 ;
