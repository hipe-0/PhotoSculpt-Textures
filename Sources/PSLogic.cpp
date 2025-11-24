/*
 * InterfaceLogic.cpp
 *
 *  Created on: 3 mai 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

#include "RESOURCE.H"
#include "PSApplication.h"
#include <stdexcept>
#include "Includes.h"
#include "Options.h"
#include "MenuIcons.h"

#define CHECK_MENU(boolean, actioncode)\
  if (boolean)                         \
       CheckMenuItem(interface_.menuHandle_, actioncode, MF_BYCOMMAND | MF_CHECKED);    \
  else CheckMenuItem(interface_.menuHandle_, actioncode, MF_BYCOMMAND | MF_UNCHECKED);  \

// Button logic : Decides whitch button is inactive and witch one is active
void PSApplication::setButtonLogic()
{
  DEBUG(setlo);
  //updateMenuRecentFiles_();

  setButtonLogicTopZone_();
  setButtonLogicMainArea_();

  if (!isSculpting_)
    setButtonLogicNotSculpting_();
  else
    setButtonLogicCurrentlySculpting_();

  setButtonLogicShaders_();
}

void PSApplication::setButtonLogicTopZone_()
{
  DEBUG(setBut);
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_SOURCES   );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_3D        );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_OLD_3D    );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_NORMAL    );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_DISPLACE  );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_AMBOCC    );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_SPECULAR  );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_DIFFUSE   );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_ALPHA     );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_BUMP      );
  interface_.topTabBar_.declareStatusOff( ACTION_MODE_LIMDISP   );
  interface_.topTabBar_.declareStatusOn ( result_.getMode()     );

  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_3D,        MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_OLD_3D,    MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_SOURCES,   MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_NORMAL,    MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_DISPLACE,  MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_AMBOCC,    MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_SPECULAR,  MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_DIFFUSE,   MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_ALPHA,     MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_BUMP,      MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, ACTION_MODE_LIMDISP,   MF_BYCOMMAND | MF_UNCHECKED);
  CheckMenuItem(interface_.menuHandle_, result_.getMode(),     MF_BYCOMMAND | MF_CHECKED  );

  interface_.activate( ACTION_MODE_SOURCES   );
  interface_.activate( ACTION_MODE_3D        );
  interface_.activate( ACTION_MODE_OLD_3D    );
  interface_.activate( ACTION_MODE_NORMAL    );
  interface_.activate( ACTION_MODE_DISPLACE  );
  interface_.activate( ACTION_MODE_AMBOCC    );
  interface_.activate( ACTION_MODE_SPECULAR  );
  interface_.activate( ACTION_MODE_DIFFUSE   );
  interface_.activate( ACTION_MODE_BUMP      );
  interface_.activate( ACTION_MODE_LIMDISP   );

  // desactivate the alpha display, but keep the menu item activated
  if (sources_.isAlpha())
    interface_.activate( ACTION_MODE_ALPHA  );
  else
    interface_.desactivate( ACTION_MODE_ALPHA  );

  EnableMenuItem(interface_.menuHandle_, ACTION_MODE_ALPHA, MF_BYCOMMAND | MF_ENABLED);
}

void PSApplication::setButtonLogicMainArea_()
{
  DEBUG(mainAr_);
  interface_.desactivate( ACTION_PASTE_LEFT      );
  interface_.desactivate( ACTION_PASTE_RIGHT     );
  interface_.desactivate( ACTION_DELETE_LEFT     );
  interface_.desactivate( ACTION_DELETE_RIGHT    );
  interface_.desactivate( ACTION_CLEAR_ALL       );
  interface_.desactivate( ACTION_3D_ROTATE_LEFT  );
  interface_.desactivate( ACTION_3D_ROTATE_RIGHT );
  interface_.desactivate( ACTION_3D_ROTATE_UP    );
  interface_.desactivate( ACTION_3D_ROTATE_DOWN  );
  interface_.desactivate( ACTION_3D_DEPTH_PLUS   );
  interface_.desactivate( ACTION_3D_DEPTH_MINUS  );
  interface_.desactivate( ACTION_3D_TEXTURE      );
  interface_.desactivate( ACTION_3D_WHITE        );
  interface_.desactivate( ACTION_3D_WIREFRAME    );
  interface_.desactivate( ACTION_STOP            );
  interface_.desactivate( ACTION_SAVE_ALL        );

  interface_.activate   ( ACTION_COPY_TO_CLIPBOARD );

  // This determines the logic of the mode button
  switch(result_.getMode())
  {
    case ACTION_MODE_3D :
    case ACTION_MODE_OLD_3D:

      interface_.activate   ( ACTION_3D_TEXTURE      );
      interface_.activate   ( ACTION_3D_WHITE        );
      interface_.activate   ( ACTION_3D_WIREFRAME    );
      interface_.activate   ( ACTION_3D_ROTATE_LEFT  );
      interface_.activate   ( ACTION_3D_ROTATE_RIGHT );
      interface_.activate   ( ACTION_3D_ROTATE_UP    );
      interface_.activate   ( ACTION_3D_ROTATE_DOWN  );
      interface_.activate   ( ACTION_3D_DEPTH_PLUS   );
      interface_.activate   ( ACTION_3D_DEPTH_MINUS  );   
      interface_.desactivate( ACTION_COPY_TO_CLIPBOARD );

      break;

    case ACTION_MODE_NORMAL :

      interface_.activate( ACTION_3D_DEPTH_PLUS   );
      interface_.activate( ACTION_3D_DEPTH_MINUS  );
      break;  

    case ACTION_MODE_AMBOCC :

      interface_.activate( ACTION_3D_DEPTH_PLUS  );
      interface_.activate( ACTION_3D_DEPTH_MINUS );
      break;  

    case ACTION_MODE_SPECULAR :

      interface_.activate   ( ACTION_3D_DEPTH_PLUS   );
      interface_.activate   ( ACTION_3D_DEPTH_MINUS  );
      break;

    case ACTION_MODE_ALPHA:   interface_.activate ( ACTION_MODE_ALPHA   ); break;
    case ACTION_MODE_BUMP:    interface_.activate ( ACTION_MODE_BUMP    ); break;
    case ACTION_MODE_LIMDISP: interface_.activate ( ACTION_MODE_LIMDISP ); break;
  }

  if (result_.isSeamless())
  {
    interface_.popup3d_.declareStatusOn  ( ACTION_TILEABLE        );
    CheckMenuItem(interface_.menuHandle_, ACTION_TILEABLE, MF_BYCOMMAND | MF_CHECKED);
    interface_.desactivate( ACTION_FLAT_MAP       );
  }
  else
  {
    CheckMenuItem(interface_.menuHandle_, ACTION_TILEABLE, MF_BYCOMMAND | MF_UNCHECKED);
    interface_.popup3d_.declareStatusOff( ACTION_TILEABLE        );
    interface_.activate   ( ACTION_FLAT_MAP       );    
  }

  if (result_.isFlat())
  {
    interface_.popup3d_.declareStatusOn  ( ACTION_FLAT_MAP        );
    CheckMenuItem(interface_.menuHandle_, ACTION_FLAT_MAP, MF_BYCOMMAND | MF_CHECKED);
  }
  else
  {
    interface_.popup3d_.declareStatusOff( ACTION_FLAT_MAP        );
    CheckMenuItem(interface_.menuHandle_, ACTION_FLAT_MAP, MF_BYCOMMAND | MF_UNCHECKED);
  }

  if (result_.isCrop())
  {
    interface_.popup3d_.declareStatusOn  ( ACTION_CROPEDIT        );
    CheckMenuItem(interface_.menuHandle_, ACTION_CROPEDIT, MF_BYCOMMAND | MF_CHECKED);
  }
  else
  {
    interface_.popup3d_.declareStatusOff( ACTION_CROPEDIT        );
    CheckMenuItem(interface_.menuHandle_, ACTION_CROPEDIT, MF_BYCOMMAND | MF_UNCHECKED);
  }

  bool istilemask = result_.getMode() == ACTION_MODE_TILE_MASK ;
  CHECK_MENU (GlobalOptions.isAutoLocalDetail_,           ACTION_DETAIL_DURING_SCULPT  );
  CHECK_MENU (GlobalOptions.isExport3DClosed_,            ACTION_CLOSE_3D_EXPORT       );
  CHECK_MENU (GlobalOptions.isSeamCorrectedDiffuse_,      ACTION_SEAM_CORRECTED_DIFFUSE);
  CHECK_MENU (GlobalOptions.isHalfSpherePreview_,         ACTION_HALF_SPHERE_PREVIEW   );
  CHECK_MENU (!GlobalOptions.isPerspectiveCorrected_,     ACTION_PERSPECTIVE_CORRECTED );
  CHECK_MENU (interface_.directX_.getSmooth(),            ACTION_SWITCH_SMOOTH_MODE    );
  CHECK_MENU (result_.isAnimation_,                       ACTION_ANIMATE_MODEL         );
  CHECK_MENU (istilemask,                                 ACTION_MODE_TILE_MASK        );

  if (sources_.isAlpha()) interface_.activate    ( ACTION_CLOSE_ALPHA );
  else                    interface_.desactivate ( ACTION_CLOSE_ALPHA );
}

void PSApplication::setButtonLogicNotSculpting_()
{
  DEBUG(seBuNoScu);
  interface_.activate( ACTION_TILEABLE                 );
  interface_.activate( ACTION_DOWNSAMPLE               );
  interface_.activate( ACTION_OPEN_LEFT_IMAGE          );
  interface_.activate( ACTION_OPEN_RIGHT_IMAGE         );
  interface_.activate( ACTION_LOAD_IDENTICAL_L_R       );
  interface_.activate( ACTION_OPEN_RECENT1             );
  interface_.activate( ACTION_OPEN_RECENT2             );
  interface_.activate( ACTION_OPEN_RECENT3             );
  interface_.activate( ACTION_OPEN_RECENT4             );
  interface_.activate( ACTION_OPEN_RECENT5             );
  interface_.activate( ACTION_OPEN_RECENT6             );
  interface_.activate( ACTION_OPEN_RECENT7             );
  interface_.activate( ACTION_OPEN_RECENT8             );
  interface_.activate( ACTION_OPEN_RECENT9             );
  interface_.activate( ACTION_LEFT_IMAGE_ROTATE_LEFT   );
  interface_.activate( ACTION_LEFT_IMAGE_ROTATE_RIGHT  );
  interface_.activate( ACTION_RIGHT_IMAGE_ROTATE_LEFT  );
  interface_.activate( ACTION_RIGHT_IMAGE_ROTATE_RIGHT );
  interface_.activate( ACTION_DELETE_LEFT              );
  interface_.activate( ACTION_DELETE_RIGHT             );
  interface_.activate( ACTION_CLEAR_ALL                );
  interface_.activate( ACTION_LEFT_RIGHT_SWAP          );
  interface_.activate( ACTION_EDIT_MORPHING            );
  interface_.activate( ACTION_EDIT_PINNING             );
  interface_.activate( ACTION_LOAD_DEPTHMAP            );
  interface_.activate( ACTION_ALPHA_ROTATE_LEFT        );
  interface_.activate( ACTION_ALPHA_ROTATE_RIGHT       );
  interface_.activate( ACTION_LOAD_SCULPT_FILE         );
  interface_.activate( ACTION_LOAD_ALPHA               );

  if ( sources_.leftImage_.is_empty() )
  {
    interface_.desactivate( ACTION_EDIT_SELECTION );
    interface_.desactivate( ACTION_ENTER_FOCAL_LENGTH );
  }
  else
  {
    interface_.activate( ACTION_EDIT_SELECTION );
    interface_.activate( ACTION_ENTER_FOCAL_LENGTH );
  }

  if ( sources_.leftImage_.is_empty() || sources_.isAlpha() )
       interface_.desactivate( ACTION_ADD_ALPHA );
  else interface_.activate   ( ACTION_ADD_ALPHA );

  if (sculpt_.isModel() || result_.isResult() ) interface_.activate   ( ACTION_ERASE_RESULT );
  else                                          interface_.desactivate( ACTION_ERASE_RESULT );

  // check if no results yet
  if (!result_.isResult())
  { 
    // NO result
    interface_.desactivate( ACTION_3D_ROTATE_LEFT );
    interface_.desactivate( ACTION_3D_ROTATE_RIGHT);
    interface_.desactivate( ACTION_3D_ROTATE_UP   );
    interface_.desactivate( ACTION_3D_ROTATE_DOWN );
    interface_.desactivate( ACTION_3D_DEPTH_PLUS  );
    interface_.desactivate( ACTION_3D_DEPTH_MINUS );
    interface_.desactivate( ACTION_3D_TEXTURE     );
    interface_.desactivate( ACTION_3D_WHITE       );
    interface_.desactivate( ACTION_3D_WIREFRAME   );
    interface_.desactivate( ACTION_SAVE_AS        );
    interface_.desactivate( ACTION_SAVE_AS_IMAGE  );
    interface_.desactivate( ACTION_SAVE_MODEL_FILE);
    interface_.desactivate( ACTION_SAVE_DEPTH_AS_16BIT_TIF);
    interface_.desactivate( ACTION_SAVE_MULTIPLE  );
    interface_.desactivate( ACTION_SAVE_ALL_IMAGES);
    interface_.desactivate( ACTION_SAVE_ALL       );
    interface_.desactivate( ACTION_COPY_TO_CLIPBOARD );
    interface_.desactivate( ACTION_MODE_NORMAL    );
    interface_.desactivate( ACTION_MODE_DISPLACE  );
    interface_.desactivate( ACTION_MODE_3D        );
    interface_.desactivate( ACTION_MODE_OLD_3D    );
    interface_.desactivate( ACTION_MODE_AMBOCC    );
    interface_.desactivate( ACTION_MODE_SPECULAR  );
    interface_.desactivate( ACTION_MODE_DIFFUSE   );
    interface_.desactivate( ACTION_MODE_ALPHA     );
    interface_.desactivate( ACTION_MODE_BUMP      );
    interface_.desactivate( ACTION_MODE_LIMDISP   );
    interface_.desactivate( ACTION_TILEABLE       );
    interface_.desactivate( ACTION_FLAT_MAP       );
    interface_.desactivate( ACTION_DOWNSAMPLE     );
    interface_.desactivate( ACTION_CROPEDIT       );
    interface_.desactivate( ACTION_ADD_DETAIL     );
    interface_.desactivate( ACTION_SUB_DETAIL     );
    interface_.desactivate( ACTION_ANIMATE_MODEL  );
    interface_.desactivate( ACTION_MINIMIZE_SCULPT_BAR);
    interface_.desactivate( ACTION_MODEL_SIZE_BAR );
  }
  else // Results are in memory
  {
    interface_.activate( ACTION_SAVE_AS           );
    interface_.activate( ACTION_SAVE_AS_IMAGE     );
    interface_.activate( ACTION_SAVE_MODEL_FILE   );
    interface_.activate( ACTION_SAVE_DEPTH_AS_16BIT_TIF );
    interface_.activate( ACTION_SAVE_MULTIPLE     );
    interface_.activate( ACTION_SAVE_ALL_IMAGES   );
    interface_.activate( ACTION_SAVE_ALL          );
    interface_.activate( ACTION_ERASE_RESULT      );
    interface_.activate( ACTION_CROPEDIT          );
    interface_.activate( ACTION_ADD_DETAIL        );
    interface_.activate( ACTION_SUB_DETAIL        );
    interface_.activate( ACTION_ANIMATE_MODEL     );
    interface_.activate( ACTION_MINIMIZE_SCULPT_BAR);
    interface_.activate( ACTION_MODEL_SIZE_BAR    );
  }

  // this determines the logic of the left and right image buttons
  if (sources_.leftImage_.is_empty() || sources_.rightImage_.is_empty())
    interface_.desactivate( ACTION_LEFT_RIGHT_SWAP );

  if ( !sources_.leftImage_.is_empty() && !sources_.rightImage_.is_empty() && 
        !isCurrentModelJustSculpted_ )
           interface_.activate   ( ACTION_SCULPT_NOW );
  else     interface_.desactivate( ACTION_SCULPT_NOW );

  if (sources_.leftImage_.is_empty())
  {
    interface_.desactivate( ACTION_LEFT_IMAGE_ROTATE_LEFT  );
    interface_.desactivate( ACTION_LEFT_IMAGE_ROTATE_RIGHT );
    interface_.desactivate( ACTION_DELETE_LEFT             );
  }

  if (sources_.rightImage_.is_empty())
  {
    interface_.desactivate( ACTION_RIGHT_IMAGE_ROTATE_LEFT  );
    interface_.desactivate( ACTION_RIGHT_IMAGE_ROTATE_RIGHT );
    interface_.desactivate( ACTION_DELETE_RIGHT             );
  }
}

void PSApplication::setButtonLogicCurrentlySculpting_()
{
  DEBUG(seButtoogicCurr_);
  interface_.desactivate( ACTION_OPEN_LEFT_IMAGE          );
  interface_.desactivate( ACTION_OPEN_RIGHT_IMAGE         );
  interface_.desactivate( ACTION_LOAD_IDENTICAL_L_R       );
  interface_.desactivate( ACTION_OPEN_RECENT1             );
  interface_.desactivate( ACTION_OPEN_RECENT2             );
  interface_.desactivate( ACTION_OPEN_RECENT3             );
  interface_.desactivate( ACTION_OPEN_RECENT4             );
  interface_.desactivate( ACTION_OPEN_RECENT5             );
  interface_.desactivate( ACTION_OPEN_RECENT6             );
  interface_.desactivate( ACTION_OPEN_RECENT7             );
  interface_.desactivate( ACTION_OPEN_RECENT8             );
  interface_.desactivate( ACTION_OPEN_RECENT9             );
  interface_.desactivate( ACTION_DELETE_LEFT              );
  interface_.desactivate( ACTION_DELETE_RIGHT             );
  interface_.desactivate( ACTION_CLEAR_ALL                );
  interface_.desactivate( ACTION_LEFT_IMAGE_ROTATE_LEFT   );
  interface_.desactivate( ACTION_LEFT_IMAGE_ROTATE_RIGHT  );
  interface_.desactivate( ACTION_RIGHT_IMAGE_ROTATE_LEFT  );
  interface_.desactivate( ACTION_RIGHT_IMAGE_ROTATE_RIGHT );
  interface_.desactivate( ACTION_LEFT_RIGHT_SWAP          );
  interface_.desactivate( ACTION_SCULPT_NOW               );
  interface_.desactivate( ACTION_EDIT_MORPHING            );
  interface_.desactivate( ACTION_EDIT_PINNING             );
  interface_.desactivate( ACTION_SAVE_AS                  );
  interface_.desactivate( ACTION_SAVE_AS_IMAGE            );
  interface_.desactivate( ACTION_SAVE_DEPTH_AS_16BIT_TIF  );
  interface_.desactivate( ACTION_SAVE_MODEL_FILE          );
  interface_.desactivate( ACTION_SAVE_MULTIPLE            );
  interface_.desactivate( ACTION_SAVE_ALL_IMAGES          );
  interface_.desactivate( ACTION_SAVE_ALL                 );
  interface_.desactivate( ACTION_COPY_TO_CLIPBOARD        );
  interface_.desactivate( ACTION_ERASE_RESULT             );
  interface_.desactivate( ACTION_DOWNSAMPLE               );
  interface_.desactivate( ACTION_LOAD_ALPHA               );
  interface_.desactivate( ACTION_ALPHA_ROTATE_LEFT        );
  interface_.desactivate( ACTION_ALPHA_ROTATE_RIGHT       );
  interface_.desactivate( ACTION_EDIT_SELECTION           );
  interface_.desactivate( ACTION_ADD_ALPHA                );
  interface_.desactivate( ACTION_CLOSE_ALPHA              );
  interface_.desactivate( ACTION_LOAD_DEPTHMAP            );
  interface_.desactivate( ACTION_LOAD_SCULPT_FILE         );
  interface_.desactivate( ACTION_ENTER_FOCAL_LENGTH       );
  interface_.activate   ( ACTION_MINIMIZE_SCULPT_BAR      );
  interface_.activate   ( ACTION_MODEL_SIZE_BAR           );
   
  if (result_.isResult())
  {
    interface_.activate( ACTION_CROPEDIT        );
    interface_.activate( ACTION_MODE_NORMAL     );
    interface_.activate( ACTION_MODE_DISPLACE   );
    interface_.activate( ACTION_MODE_3D         );
    interface_.activate( ACTION_MODE_OLD_3D     );
    interface_.activate( ACTION_MODE_AMBOCC     );
    interface_.activate( ACTION_MODE_SPECULAR   );
    interface_.activate( ACTION_MODE_DIFFUSE    );
    interface_.activate( ACTION_MODE_BUMP       );
    interface_.activate( ACTION_MODE_LIMDISP    );
    interface_.activate( ACTION_TILEABLE        );
    interface_.activate( ACTION_FLAT_MAP        ); 
    interface_.activate( ACTION_ANIMATE_MODEL   );

    if (result_.getMode()==ACTION_MODE_DISPLACE)
    {
      interface_.desactivate( ACTION_3D_DEPTH_PLUS   );
      interface_.desactivate( ACTION_3D_DEPTH_MINUS  );    
    }
    else
    {
      interface_.activate   ( ACTION_3D_DEPTH_PLUS   );
      interface_.activate   ( ACTION_3D_DEPTH_MINUS  );
    }

    if (result_.getMode()==ACTION_MODE_3D)
    {
      interface_.activate( ACTION_3D_ROTATE_DOWN  );
      interface_.activate( ACTION_3D_ROTATE_UP    );
      interface_.activate( ACTION_3D_ROTATE_LEFT  );
      interface_.activate( ACTION_3D_ROTATE_RIGHT );
      interface_.activate( ACTION_3D_TEXTURE      );
      interface_.activate( ACTION_3D_WHITE        );
      interface_.activate( ACTION_3D_WIREFRAME    );
    }
    else
    {
      interface_.desactivate( ACTION_3D_ROTATE_DOWN  );
      interface_.desactivate( ACTION_3D_ROTATE_UP    );
      interface_.desactivate( ACTION_3D_ROTATE_LEFT  );
      interface_.desactivate( ACTION_3D_ROTATE_RIGHT );
    }
  }
  else // has no result in memory
  {
    interface_.desactivate( ACTION_CROPEDIT        );
    interface_.desactivate( ACTION_MODE_3D         );
    interface_.desactivate( ACTION_MODE_OLD_3D     );
    interface_.desactivate( ACTION_MODE_NORMAL     );
    interface_.desactivate( ACTION_MODE_DISPLACE   );
    interface_.desactivate( ACTION_MODE_AMBOCC     );
    interface_.desactivate( ACTION_MODE_SPECULAR   );
    interface_.desactivate( ACTION_MODE_DIFFUSE    );
    interface_.desactivate( ACTION_MODE_ALPHA      );
    interface_.desactivate( ACTION_MODE_BUMP       );
    interface_.desactivate( ACTION_MODE_LIMDISP    );
    interface_.desactivate( ACTION_TILEABLE        );
    interface_.desactivate( ACTION_FLAT_MAP        );  
    interface_.desactivate( ACTION_3D_ROTATE_DOWN  );
    interface_.desactivate( ACTION_3D_ROTATE_UP    );
    interface_.desactivate( ACTION_3D_ROTATE_LEFT  );
    interface_.desactivate( ACTION_3D_ROTATE_RIGHT );
    interface_.desactivate( ACTION_3D_TEXTURE      );
    interface_.desactivate( ACTION_3D_WHITE        );
    interface_.desactivate( ACTION_3D_WIREFRAME    );  
    interface_.desactivate( ACTION_3D_DEPTH_PLUS   );
    interface_.desactivate( ACTION_3D_DEPTH_MINUS  );
    interface_.desactivate( ACTION_TILEABLE        );
    interface_.desactivate( ACTION_FLAT_MAP        );
    interface_.desactivate( ACTION_ANIMATE_MODEL   );
  }

  interface_.activate( ACTION_STOP );
  interface_.activate( ACTION_EXIT );
}

void PSApplication::setButtonLogicShaders_()
{
  DEBUG(setButgicSha);
  if (result_.getShader() == ACTION_3D_TEXTURE)
  {
    interface_.popup3d_.declareStatusOn  ( ACTION_3D_TEXTURE   );
    interface_.popup3d_.declareStatusOff ( ACTION_3D_WHITE     );
    interface_.popup3d_.declareStatusOff ( ACTION_3D_WIREFRAME );
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_TEXTURE  , MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_WHITE    , MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_WIREFRAME, MF_BYCOMMAND | MF_UNCHECKED  );
  }
  else if (result_.getShader() == ACTION_3D_WHITE)
  {
    interface_.popup3d_.declareStatusOff  ( ACTION_3D_TEXTURE   );
    interface_.popup3d_.declareStatusOn   ( ACTION_3D_WHITE     );
    interface_.popup3d_.declareStatusOff  ( ACTION_3D_WIREFRAME );
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_TEXTURE  , MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_WHITE    , MF_BYCOMMAND | MF_CHECKED);
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_WIREFRAME, MF_BYCOMMAND | MF_UNCHECKED  );
  }
  else if (result_.getShader() == ACTION_3D_WIREFRAME)
  {
    interface_.popup3d_.declareStatusOff  ( ACTION_3D_TEXTURE   );
    interface_.popup3d_.declareStatusOff  ( ACTION_3D_WHITE     );
    interface_.popup3d_.declareStatusOn   ( ACTION_3D_WIREFRAME );
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_TEXTURE  , MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_WHITE    , MF_BYCOMMAND | MF_UNCHECKED);
    CheckMenuItem(interface_.menuHandle_, ACTION_3D_WIREFRAME, MF_BYCOMMAND | MF_CHECKED  );
  }

  if (result_.getMode() == ACTION_MODE_OLD_3D)
    interface_.desactivate( ACTION_3D_WIREFRAME );

  if (sculpt_.isModel() || isSculpting_) interface_.activate   ( ACTION_TIME_LEFT_BAR  );
  else                                   interface_.desactivate( ACTION_TIME_LEFT_BAR  );

  if (result_.isResult())
  {
    if (result_.subdivision_ == 256)  interface_.desactivate ( ACTION_SUBDIVISION_REDUCE );
    else                              interface_.activate    ( ACTION_SUBDIVISION_REDUCE );

    if (result_.subdivision_ == 1)    interface_.desactivate ( ACTION_SUBDIVISION_INCREASE );
    else                              interface_.activate    ( ACTION_SUBDIVISION_INCREASE );
  }
  else
  {
    interface_.desactivate( ACTION_SUBDIVISION_INCREASE );
    interface_.desactivate( ACTION_SUBDIVISION_REDUCE   );
  }
}

void GraphicUserInterface::desactivate(int action) 
{
  bottomBar_     .declare_grayed_out(action);
  bottomMiniBar_ .declare_grayed_out(action);
  popupSave_     .declare_grayed_out(action);
  popup3d_       .declare_grayed_out(action);
  popupLoadL_    .declare_grayed_out(action);
  popupLoadR_    .declare_grayed_out(action);
  popupLoadAlpha_.declare_grayed_out(action);
  topTabBar_     .declare_grayed_out(action);
  bottomSolo_    .declare_grayed_out(action);
  
  EnableMenuItem(menuHandle_, action, MF_BYCOMMAND | MF_GRAYED);
}

void GraphicUserInterface::activate(int action) 
{
  bottomBar_     .declare_active(action);
  bottomMiniBar_ .declare_active(action);
  popupSave_     .declare_active(action);
  popup3d_       .declare_active(action);
  popupLoadL_    .declare_active(action);
  popupLoadR_    .declare_active(action);
  popupLoadAlpha_.declare_active(action);
  topTabBar_     .declare_active(action);
  bottomSolo_    .declare_active(action);
  
  EnableMenuItem(menuHandle_, action, MF_BYCOMMAND | MF_ENABLED);
  
  int x1,y1,x2,y2;
  char txt[50]="";
  
  if (bottomBar_.    isButton(action))
  {
    bottomBar_.getButton      (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (bottomMiniBar_.isButton(action))
  {
    bottomMiniBar_.getButton  (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (popupSave_.    isButton(action))
  {
    popupSave_.getButton      (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (popup3d_.      isButton(action))
  {
    popup3d_.getButton        (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (popupLoadL_.   isButton(action))
  {
    popupLoadL_.getButton     (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (popupLoadR_.   isButton(action))
  {
    popupLoadR_.getButton     (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (popupLoadAlpha_.isButton(action))
  {
    popupLoadAlpha_.getButton (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (topTabBar_.    isButton(action))
  {
    topTabBar_.    getButton  (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }

  if (bottomSolo_.   isButton(action))
  {
    bottomSolo_.   getButton  (action, x1,y1,x2,y2, txt);
    toolTip_.add              (action, x1,y1,x2,y2, txt);
  }
}

bool PSApplication::isLastStep()
{
  if (sculpt_.isModel()) return sculpt_.isLastStep();
  else                   return false;
}

void PSApplication::updateMenuRecentFiles_()
{
  DEBUG(updateRec);
  for (int i=0 ; i<NB_RECENT_FILES; i++)
  {
    char menutext[MAX_PATH] = "\0" ;

    char key[50] = "\0" ;
    sprintf(key, "last opened left image %d", i+1);

    bool isok = registry_.loadString(key,menutext, MAX_PATH);
    if (!isok)
      strcpy(menutext, "None");

    bool isok2 = ModifyMenu(interface_.menuHandle_, ACTION_OPEN_RECENT1+i, MF_BYCOMMAND | MF_STRING , 
                           ACTION_OPEN_RECENT1+i,
                           menutext);
    if (!isok2)
      THROW_INTERNAL_ERROR( "Could not modify menu (in updateMenuRecentFiles_())" );
  }
}