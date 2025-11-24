
#include "CONSTANTS.h"

#define IDS_APP_TITLE			      100
#define IDS_APP_TITLE_EDIT      101
#define IDS_APP_TITLE_CROPEDIT  102
#define IDR_MAINFRAME			      103
#define IDS_APP_TITLE_SELECTEDIT  104

#define IDD_PHOTOSCULPT_DIALOG	104
#define IDD_ABOUTBOX			      105
#define IDD_EDITBOX             106
#define IDD_PROGRESS_BOX        107
#define IDD_SAVE_MULTIPLE_BOX   108
#define IDD_WARNINGBOX          109
#define IDD_OPTION_BOX          110
#define IDD_FOCAL_BOX           111
#define IDD_BLUR_BOX            112
#define IDD_3DPOSITION_BOX      113
#define IDD_MULTITHREAD_BOX     114
#define IDD_FLATMODESAMPLE_BOX  115

#define IDI_PHOTOSCULPT	          1
#define IDI_SMALL				          2
#define IDI_MENUICON_LOAD         3
#define IDI_MENUICON_DISK         4
#define IDI_MENUICON_2DISKS       5
#define IDI_MENUICON_ALPHA        6
#define IDI_MENUICON_COPY         7
#define IDI_MENUICON_CROSS        8
#define IDI_MENUICON_CROSS2       9
#define IDI_MENUICON_FLAT        10
#define IDI_MENUICON_INCREASE    11
#define IDI_MENUICON_REDUCE      12
#define IDI_MENUICON_SCULPT      13
#define IDI_MENUICON_STOP        14
#define IDI_MENUICON_TILEABLE    15
#define IDI_MENUICON_CROP        16
#define IDI_MENUICON_QUESTION    17
#define IDI_MENUICON_LABORATORY  18
#define IDI_MENUICON_FOLDER      19
#define IDI_MENUICON_WHEELS      20

#define IDC_PHOTOSCULPT	        130
#define IDC_EDITOR	            131
#define IDC_CROPEDITOR	        132
#define IDC_WARNING_IMG         133
#define IDC_WARNING_TXT         134
#define IDC_SELECTEDITOR        135

#ifndef IDC_STATIC
  #define IDC_STATIC				     -1
#endif

#define KEY_LAST_L1 "last opened left image 1"
#define KEY_LAST_R1 "last opened right image 1"
#define KEY_LAST_L2 "last opened left image 2"
#define KEY_LAST_R2 "last opened right image 2"
#define KEY_LAST_L3 "last opened left image 3"
#define KEY_LAST_R3 "last opened right image 3"
#define KEY_LAST_L4 "last opened left image 4"
#define KEY_LAST_R4 "last opened right image 4"

#define     NOACTION                         -2
#define     ACTION_REPAINT_SCREEN            -1

#define     ACTION_OPEN_LEFT_IMAGE           120
#define     ACTION_OPEN_RIGHT_IMAGE          121
#define     ACTION_SCULPT_NOW                122
#define     ACTION_EXIT                      124
#define     ACTION_EDIT_MORPHING             125

#define     ACTION_SAVE_AS                   131
#define     ACTION_SAVE_AS_IMAGE             132


#define     ACTION_ERASE_RESULT              150
#define     ACTION_LEFT_IMAGE_ROTATE_LEFT    151
#define     ACTION_LEFT_IMAGE_ROTATE_RIGHT   152
#define     ACTION_RIGHT_IMAGE_ROTATE_LEFT   153
#define     ACTION_RIGHT_IMAGE_ROTATE_RIGHT  154
#define     ACTION_LEFT_RIGHT_SWAP           155
#define     ACTION_3D_ROTATE_LEFT            156
#define     ACTION_3D_ROTATE_RIGHT           157
#define     ACTION_3D_ROTATE_UP              158
#define     ACTION_3D_ROTATE_DOWN            159
#define     ACTION_3D_DEPTH_PLUS             160
#define     ACTION_3D_DEPTH_MINUS            161
#define     ACTION_3D_TEXTURE                162
#define     ACTION_3D_WHITE                  163
#define     ACTION_3D_WIREFRAME              164
#define     ACTION_TILEABLE                  165
#define     ACTION_FLAT_MAP                  166
#define     ACTION_DOWNSAMPLE                168
#define     ACTION_STOP                      169
#define     ACTION_3D_ZOOM_PLUS              170
#define     ACTION_3D_ZOOM_MINUS             171

#define     ACTION_WEBSITE                   181
#define     ACTION_LICENSE                   182
#define     ACTION_TECHNICAL_SUPPORT         183
#define     ACTION_SAVE_ALL_IMAGES           185
#define     ACTION_SAVE_ALL                  186
#define     ACTION_COPY_TO_CLIPBOARD         187
#define     ACTION_OPEN_RECENT1              189
#define     ACTION_OPEN_RECENT2              190
#define     ACTION_OPEN_RECENT3              191
#define     ACTION_OPEN_RECENT4              192
#define     ACTION_OPEN_RECENT5              193
#define     ACTION_OPEN_RECENT6              194
#define     ACTION_OPEN_RECENT7              195
#define     ACTION_OPEN_RECENT8              196
#define     ACTION_OPEN_RECENT9              197
#define     ACTION_STOP_WITH_KEY             199
#define     ACTION_DELETE_LEFT               201
#define     ACTION_DELETE_RIGHT              202
#define     ACTION_PASTE_LEFT                203
#define     ACTION_PASTE_RIGHT               204
#define     ACTION_EDIT_PINNING              207
#define     ACTION_CROPEDIT                  208
#define     ACTION_ABOUT                     227

#define     ACTION_EDIT_OK                   240
#define     ACTION_EDIT_CANCEL               241
#define     ACTION_EDIT_MODE_ADD_POINT       242
#define     ACTION_EDIT_MODE_ADD_LINE        243
#define     ACTION_EDIT_MODE_DELETE          244
#define     ACTION_EDIT_MODE_MOVE_POINT      245

#define     ACTION_CROPEDIT_OK               250
#define     ACTION_CROPEDIT_CANCEL           251
#define     ACTION_CROPEDIT_MODE_SQUARE      252
#define     ACTION_CROPEDIT_MODE_2N          253
#define     ACTION_CROPEDIT_DELETE           254
#define     ACTION_CROPEDIT_CHOOSE_SIZE      255
#define     ACTION_CROPEDIT_FRAME_PLUS       256
#define     ACTION_CROPEDIT_FRAME_MINUS      257
#define     ACTION_CROPEDIT_PERSPECTIVE      258
#define     ACTION_CROPEDIT_APPLY            259

#define     ACTION_WEBSITE_BUY               260
#define     ACTION_LOAD_DEPTHMAP             261
#define     ACTION_SAVE_MODEL_FILE           263

#define     ACTION_IMAGE_SOURCE_LEFT         264
#define     ACTION_IMAGE_SOURCE_RIGHT        265
#define     ACTION_IMAGE_CENTER              266
#define     ACTION_IMAGE_3D                  267
#define     ACTION_SUBDIVISION_REDUCE        268
#define     ACTION_SUBDIVISION_INCREASE      269
#define     ACTION_MODEL_SIZE_BAR            270
#define     ACTION_TIME_LEFT_BAR             271
#define     ACTION_LOAD_SCULPT_FILE          273
#define     ACTION_SAVE_MULTIPLE             274
#define     ACTION_LOAD_OLD_SCULPT_FILE      275
#define     ACTION_IMAGE_SOURCE_ALPHA        276

#define     MULTI_SAVE_MODEL                 280
#define     MULTI_STL                        281
#define     MULTI_OBJ                        282
#define     MULTI_SCULPT                     283
#define     MULTI_LIST_SIZE_MODEL            284
#define     MULTI_SAVE_MAP                   285
#define     MULTI_2N_SIZE                    286
#define     MULTI_LIST_SIZE_MAP              287
#define     MULTI_LIST_MAP_TYPE              288
#define     MULTI_PLY                        289

#define     ACTION_BOOST_TRIANGLE_COUNT      290
#define     ACTION_QUERY_MEMORY              291
#define     ACTION_OPEN_LAST_DIRECTORY       292
#define     ACTION_ADD_DETAIL                293
#define     ACTION_SUB_DETAIL                294
#define     ACTION_SWITCH_SMOOTH_MODE        296
#define     ACTION_ANALYSIS                  297
//#define     ACTION_PROGRESS_WINDOW           298
#define     ACTION_ANIMATE_MODEL             299
#define     ACTION_OPTIONS                   300

#define     OPTION_INVERT_NORMAL_X           301
#define     OPTION_INVERT_NORMAL_Y           302
#define     OPTION_TRIANGLE_MAX              303
#define     OPTION_TRIANGLE_TXT              304
#define     OPTION_INTERFACE_HUE             305
#define     OPTION_HUE_TXT                   306
#define     OPTION_INTERFACE_SAT             307
#define     OPTION_SAT_TXT                   308
#define     OPTION_JPEG_PERCENT              309
#define     OPTION_JPEG_TXT                  310
#define     OPTION_INTERFACE_LUMINOSITY      311
#define     OPTION_LUM_TXT                   312
#define     OPTION_INTERFACE_OPACITY         314
#define     OPTION_OPAC_TXT                  315

#define     ACTION_TUTORIAL                  320
#define     ACTION_LOAD_ALPHA                321

#define     ABOUT_LICENCED_TO                330
#define     ACTION_OPEN_SAMPLE_DIRECTORY     331

#define     ACTION_MANUAL_EDIT_PLUS          340
#define     ACTION_MANUAL_EDIT_MINUS         341
#define     ACTION_MANUAL_EDIT_LEFT          342
#define     ACTION_MANUAL_EDIT_RIGHT         343
#define     ACTION_MANUAL_EDIT_OK            344
#define     ACTION_MANUAL_EDIT_CANCEL        345
#define     ACTION_MANUAL_EDIT_RESET         346
#define     ACTION_MANUAL_EDIT_CLEAR         347
#define     ACTION_MANUAL_EDIT_LEFTRIGHT     348

#define     WARNING                          "W0"
#define     WARNING_IMAGE_1                  "W1"
#define     WARNING_IMAGE_2                  "W2"
#define     WARNING_IMAGE_3                  "W3"
#define     WARNING_IMAGE_4                  "W4"
#define     WARNING_IMAGE_5                  "W5"
#define     WARNING_IMAGE_6                  "W6"
#define     WARNING_IMAGE_7                  "W7"
#define     WARNING_IMAGE_8                  "W8"
#define     WARNING_IMAGE_9                  "W9"
#define     WARNING_IMAGE_10                 "W10"
#define     WARNING_IMAGE_11                 "W11"
#define     WARNING_IMAGE_12                 "W12"
#define     WARNING_IMAGE_13                 "W13"
#define     WARNING_IMAGE_14                 "w14"
#define     WARNING_IMAGE_15                 "W15"
#define     WARNING_IMAGE_16                 "W16"
#define     WARNING_IMAGE_17                 "W17"
#define     WARNING_IMAGE_18                 "W18"

#define     ACTION_MODE_FIRST                   350
#define     ACTION_MODE_SOURCES              351
#define     ACTION_MODE_3D                   352
#define     ACTION_MODE_DISPLACE         353
#define     ACTION_MODE_NORMAL               354
#define     ACTION_MODE_AMBOCC               355
#define     ACTION_MODE_SPECULAR             356
#define     ACTION_MODE_DIFFUSE              357
#define     ACTION_MODE_ALPHA                358
#define     ACTION_MODE_OLD_3D               359
#define     ACTION_MODE_LIMDISP              360
#define     ACTION_MODE_BUMP                 361
#define     ACTION_MODE_TILE_MASK            362
#define     ACTION_MODE_LAST                    369

#define     ACTION_DETAIL_DURING_SCULPT      370
#define     ACTION_CLOSE_ALPHA               371

#define     ACTION_SAVE_DEPTH_AS_16BIT_TIF   372
#define     ACTION_CLEAR_ALL                 373
#define     ACTION_LOAD_IDENTICAL_L_R        374

#define     ACTION_CLOSE_3D_EXPORT           377
#define     ACTION_ADDITIONAL_PHOTO          378

#define     ACTION_SEARCH_CAMERA_POS         379
#define     ACTION_PERSPECTIVE_CORRECTED     380

#define     ACTION_ALPHA_ROTATE_LEFT         381
#define     ACTION_ALPHA_ROTATE_RIGHT        382

#define     ACTION_ENTER_FOCAL_LENGTH        383
#define     FOCAL_EDIT                       384

#define     ACTION_ENTER_BLUR_RADIUS         385
#define     BLUR_RADIUS_EDIT                 386

#define     POSITION_COMPUTE                 387
#define     POSITION_STOP                    388

#define     ACTION_MULTITASKING_TRIAL        389
#define     ACTION_MULTITHREAD_SETTINGS      390
#define     CORES_EDIT                       391
#define     ACTION_REINIT_3D_DISPLAY         392

#define     ACTION_EDIT_SELECTION            400
#define     ACTION_SELECT_WHITE              401
#define     ACTION_SELECT_BLACK              402
#define     ACTION_SELECT_LASSO              403
#define     ACTION_SELECT_POLYGON            404
#define     ACTION_SELECT_INVERT             405
#define     ACTION_SELECT_CLEAR              406
#define     ACTION_SELECT_LOAD_ALPHA         407
#define     ACTION_SELECT_SAVE_ALPHA         408
#define     ACTION_SELECT_UNDO               409
#define     ACTION_SELECT_CANCEL             410
#define     ACTION_SELECT_OK                 411

#define     ACTION_QUIT_AT_ONCE              420
#define     SAVE_AS_PER_COMMAND_LINE         421
#define     ACTION_ADD_ALPHA                 422
#define     ACTION_ZERO_DISPLACE_MAP         423

#define     ACTION_BATCH_SCULPT_FOLDER       424
#define     ACTION_COMMANDED_SCULPT          425
#define     ACTION_BATCH_SCULPT_TILES        426
#define     ACTION_SEAM_CORRECTED_DIFFUSE    427
#define     ACTION_HALF_SPHERE_PREVIEW       428

#define     SAMPLES_EDIT                     429
#define     ACTION_FLAT_MODE_SAMPLE_DIALOG   430

#define     ACTION_MINIMIZE_SCULPT_BAR       431
#define     ACTION_OPTIMIZE_SCULPT_PARAMETERS 432
#define     ACTION_LABORATORY                433
#define     ACTION_SUGGEST_FUNCTION          434
#define     ACTION_FACEBOOK                  435
#define     ACTION_TWEETER                   436


// Valeurs par défaut suivantes des nouveaux objets
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS

#define _APS_NO_MFC					130
#define _APS_NEXT_RESOURCE_VALUE	129
#define _APS_NEXT_COMMAND_VALUE		32771
#define _APS_NEXT_CONTROL_VALUE		1000
#define _APS_NEXT_SYMED_VALUE		110
#endif
#endif

