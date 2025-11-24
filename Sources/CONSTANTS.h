/*
 * Constants.h
 *
 * Created on: 06 jan 2011
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_

#pragma once

#include "Obfuscate.h"
#include "Parameter.h"

#ifdef DEMO_MODE
  #define VERSION_PHOTOSCULPT_SOFTWARE     "Demo v2.04"
#else
  #define VERSION_PHOTOSCULPT_SOFTWARE     "v2.04"
#endif

#ifdef DEMO_MODE
  #define APPLICATION_NAME  "Photosculpt Textures 2 Demo"
  #define REGISTRY_KEY      "Software\\Photosculpt\\PhotoSculpt Textures Demo 2"
  #define REGISTRY_KEY_LM   "SOFTWARE\\Photosculpt\\PhotoSculpt Textures Demo 2"
#else
  #define APPLICATION_NAME  "Photosculpt Textures 2"
  #define REGISTRY_KEY      "Software\\Photosculpt\\PhotoSculpt Textures 2"
  #define REGISTRY_KEY_LM   "SOFTWARE\\Photosculpt\\PhotoSculpt Textures 2"
#endif

#define COPYRIGHT         "Copyright (C) Hippolyte Mounier 2011"

#define WEB_SITE          "http://www.photosculpt.net"
#define WEB_SITE_SUPPORT  "http://www.photosculpt.net/support/"
#define WEB_SITE_BUY      "http://www.photosculpt.net/buy-now/"
#define WEB_SITE_TUTORIAL "http://www.photosculpt.net/tutorial/"
#define WEB_SITE_SUGGEST  "http://photosculpt.uservoice.com"
#define WEB_SITE_FACEBOOK "http://www.facebook.com/photosculpt"
#define WEB_SITE_TWEETER  "http://twitter.com/photosculpt"

#define STEP_SPIN_3D_ANGLE           4.0f 
#define STEP_3D_SCALE                1.2f
#define SEAMLESS_SCALE_FACTOR        0.8f
#define MAX_SECONDS_BETWEEN_SHOTS    40
#define INITIAL_CROP_BLEND_FRAME     0.13f
#define DEFAULT_BATCH_OUT_DIRECTORY  "\\out\\"
#define DEFAULT_BATCH_OUT_DIR_TILES  "\\tiles\\"
#define CHECK_IMAGE_SIMILARITY_SIZE  30
#define IMAGE_SIMILARITY_LIMIT       0.2f
#define IMAGE_SINGULARITY_BLUR       1.0f
#define REPEATED_SEAM_PERCENT        20
#define FLAT_MAP_INIT_SIZE           10
#define APPWINDOW_INITIAL_SIZE_X     720
#define APPWINDOW_INITIAL_SIZE_Y     540
#define POSITION_MENU_QUESTION       4
#define POSITION_MENU_LABORATORY     10

// Declarations
extern const oint          PROGRESSION_BAR_LEFT     ;
extern const oint          PROGRESSION_BAR_UP       ;
extern const oint          PROGRESSION_BAR_RIGHT    ;  
extern const oint          PROGRESSION_BAR_DOWN     ;
extern const unsigned char MAIN_COLOR_R             ;
extern const unsigned char MAIN_COLOR_G             ;
extern const unsigned char MAIN_COLOR_B             ;
extern const oint          PROGRESSION_BAR_TEXT_X   ;
extern const oint          PROGRESSION_BAR_TEXT_Y   ;

extern const oint          ID_ANIMATION_TIMER       ;
extern const oint          NB_MILISECONDS_ANIMATION ;

extern const int           MIN_WINDOW_SIZE          ;
extern const int           MIN_WINDOW_OVERLAP       ;

extern const oint          IMAGE_BLANK_FRAME        ;
extern const int           NB_RECENT_FILES          ;

extern Parameter           RECIPE[]                 ; 

extern float               MASKED_BLUR_THRESHOLD    ;
extern ofloat              MASKED_BLUR_THRESHOLD2   ;
extern int                 MASKED_BLUR_RANGE        ;
extern oint                RANGE_FOR_EDGE_DETECTION ;
extern ofloat              EDGE_MASK_THRESHOLD      ;
extern oint                RANGE_FOR_EDGE_BLUR      ;
extern ofloat              THRESHOLD_EDGE_BLUR      ;

extern oint                MIN_IMAGE_SIZE           ;
extern ofloat              BLUR_IMAGES_VALUE        ;

extern oint                VERTICAL_SEARCH_LIMIT    ;

extern ofloat              EXPOSURE_BLUR_RATIO      ;
extern ofloat              EXPOSURE_FADE            ;

extern oint                MASKED_BLUR_COUNT        ;

extern oint                Y_BLUR_VALUE             ;

extern ofloat              MIX_QUALITY_TOLERANCE    ; 

extern ofloat              DEFAULT_THRESHOLD        ;
extern ofloat              IGNORE_VALUE             ;

extern oint                INITIAL_BLUR_RANGE       ;
extern const oint          NB_SUB_STEP              ;

extern const int           NB_MAX_LEVELS            ;

#define                    INTERRUPT_NB             25 
extern float               INTERRUPT_FACTOR[INTERRUPT_NB];

extern const oint          MAX_PIXEL_IN_IMAGE       ;

extern float               FAKE_BLUR_RANGES[5]      ;
extern float               FAKE_BLUR_FADES [5]      ;

extern const int           BURSTMAP_RANGE_IN_PIXEL  ;
extern const int           BURSTMAP_FIXED_COMA      ;

extern const unsigned char TEXT_MUTED_COLOR  []     ;
extern const unsigned char TEXT_BRIGHT_COLOR []     ;
extern const unsigned char TEXT_ALPHA        []     ;
extern const int           SWITCH_MUTED_BRIGHT_COLOR ;

extern const bool          DUMP_TIMES_TO_FILE;