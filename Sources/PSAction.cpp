/*
 * InterfaceAction.cpp
 *
 *  Created on: 28 07 2009
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */


#include "RESOURCE.H"
#include "PSApplication.h"
#include <stdexcept>
#include "Limit.h"
#include <process.h>
#include "Includes.h"
#include "CropEditor.h"
#include "UserInterrupt.h"
#include "Obfuscate.h"
#include "Win32Tools.h"
#include "PhotosculptTools.h"
#include "POVFinder.h"
#include "MultiTaskingTrial.h"

static int actionForThread;
 
void threadedAction(void* Ptr)
{
  ((PSApplication*)(Ptr))->doAction(actionForThread);
} 

void PSApplication::doActionOnNewThreadIfNecessary(oint actionNo)
{
  //DEBUG(doAct); // too much verbose
  switch(actionNo)
  {
    case ACTION_SCULPT_NOW: // all these actions will be launched on a new thread
    case ACTION_EDIT_MORPHING: // in order not to stop the windows procedure updates
    case ACTION_SEARCH_CAMERA_POS:
    case ACTION_BATCH_SCULPT_FOLDER:
    case ACTION_BATCH_SCULPT_TILES:
    case ACTION_MULTITASKING_TRIAL:
    case ACTION_COMMANDED_SCULPT:
    case SAVE_AS_PER_COMMAND_LINE:
    case ACTION_OPTIMIZE_SCULPT_PARAMETERS:
      {
        //DEBUG_VALUE("new thread", actionNo);
        actionForThread = actionNo;

        // Launches the corresponding action and draws results
        uintptr_t pointer = _beginthread(threadedAction,0,(void*) this);
        //DEBUG_VALUE(" thread", int(pointer));
        break;
      }
    default:
      //DEBUG_VALUE("action ", actionNo);
      doAction(actionNo);
  }
}

void PSApplication::doActionOnFirstThread(oint actionNo)
{
  SendMessage(interface_.windowsHandle_,WM_COMMAND, LOWORD(actionNo), 0);
}
void PSApplication::postActionOnFirstThread(oint actionNo)
{
  PostMessage(interface_.windowsHandle_,WM_COMMAND, LOWORD(actionNo), 0);
}

bool PSApplication::askBeforeDelete()
{
  DEBUG(askBefo);
  
  if (!isSculpting_)
  {
    if (sculpt_.isModel() || result_.isResult())
    {
      if(MessageBox(interface_.windowsHandle_,"3d model will be deleted before continuing, are you sure ?",
                    "Question ?",MB_YESNO | MB_ICONQUESTION ) == IDYES)
      {
        clearSculptAndResults();
        return true;
      }
      else
        return false; // do nothing as ordered
    }
    else
      // nothing to delete, let's proceed
      return true;
  }

  return false; // do nothing
}


void PSApplication::actionSwitchOption(bool &boolean, char* textiftrue, char* textiffalse)
{
  boolean = !boolean;
  OptionStore::saveToRegistry(GlobalOptions, &registry_);

  reinit3DDisplay();

  if (boolean)
    MESSAGEHWND(textiftrue)
  else
    MESSAGEHWND(textiffalse)
}

void PSApplication::doAction(oint actionNo)
{
  try
  {
    bool isPendingRedraw=true;

    switch(actionNo)
    {
    case ACTION_REPAINT_SCREEN:              break;
      // will launch a redraw of the interface because isPendingRedraw is true by default

    case ACTION_OPEN_RECENT1: loadRecent(0); break;
    case ACTION_OPEN_RECENT2: loadRecent(1); break;
    case ACTION_OPEN_RECENT3: loadRecent(2); break;
    case ACTION_OPEN_RECENT4: loadRecent(3); break;
    case ACTION_OPEN_RECENT5: loadRecent(4); break;
    case ACTION_OPEN_RECENT6: loadRecent(5); break;
    case ACTION_OPEN_RECENT7: loadRecent(6); break;
    case ACTION_OPEN_RECENT8: loadRecent(7); break;
    case ACTION_OPEN_RECENT9: loadRecent(8); break;

    case ACTION_EXIT :
   
      if(MessageBox(interface_.windowsHandle_,"Quit, are you sure ?",
                    "Question ?",MB_YESNO | MB_ICONQUESTION ) == IDYES)
      {
        saveInterfacePosition();
        DEBUG_CLEAN_FILE();
        DestroyWindow(interface_.windowsHandle_);
      }
      
      break;
    case ACTION_QUIT_AT_ONCE:       
      DestroyWindow(interface_.windowsHandle_); break;

    case ACTION_OPEN_LEFT_IMAGE:    loadDialog(true);        break;
    case ACTION_OPEN_RIGHT_IMAGE:   loadDialog(false);       break;
    case ACTION_LOAD_IDENTICAL_L_R: loadDialogIdenticalLR(); break;
    case ACTION_LOAD_DEPTHMAP :     loadDepthMapDialog();    break;
    case ACTION_LOAD_ALPHA:         loadAlphaDialog();       break;

    case ACTION_LOAD_SCULPT_FILE : if (askBeforeDelete()) loadSculptFileDialog(); break;

    case ACTION_SCULPT_NOW :             runExtraction();       break;
    case ACTION_EDIT_MORPHING :          runManualEdit();       break;
    case ACTION_SAVE_AS :                saveAsDialog();        break;
    case ACTION_SAVE_AS_IMAGE :          saveAsImageDialog();   break;
    case ACTION_SAVE_MODEL_FILE:         saveAsModelDialog();   break;
    case ACTION_SAVE_MULTIPLE:           actionSaveMultiple();  break;
    case ACTION_SAVE_DEPTH_AS_16BIT_TIF: save16BitTiffDialog(); break;

    case ACTION_MODE_3D        :         case ACTION_MODE_DISPLACE :
    case ACTION_MODE_NORMAL    :         case ACTION_MODE_DIFFUSE   :
    case ACTION_MODE_ALPHA     :         case ACTION_MODE_SOURCES   :
    case ACTION_MODE_OLD_3D    :         case ACTION_MODE_TILE_MASK :
    case ACTION_MODE_AMBOCC : case ACTION_MODE_SPECULAR  :
    case ACTION_MODE_BUMP              : case ACTION_MODE_LIMDISP  :
                                         setPage(actionNo);     break;

    case ACTION_ERASE_RESULT :
      if(MessageBox(interface_.windowsHandle_,"Delete 3d model, are you sure ?",
                    "Question ?",MB_YESNO | MB_ICONQUESTION ) == IDYES)
        clearSculptAndResults();

      break;

    case ACTION_LEFT_IMAGE_ROTATE_LEFT   :  rotateImages(0.0f, -90.0f,   0.0f,   0.0f); break;
    case ACTION_LEFT_IMAGE_ROTATE_RIGHT  :  rotateImages(0.0f,  90.0f,   0.0f,   0.0f); break;
    case ACTION_RIGHT_IMAGE_ROTATE_LEFT  :  rotateImages(0.0f,   0.0f, -90.0f,   0.0f); break;
    case ACTION_RIGHT_IMAGE_ROTATE_RIGHT :  rotateImages(0.0f,   0.0f,  90.0f,   0.0f); break;
    case ACTION_ALPHA_ROTATE_LEFT        :  rotateImages(0.0f,   0.0f,   0.0f, -90.0f); break;
    case ACTION_ALPHA_ROTATE_RIGHT       :  rotateImages(0.0f,   0.0f,   0.0f,  90.0f); break;

    case ACTION_DELETE_LEFT: if (askBeforeDelete()) sources_.leftImage_=Image();  break;
    case ACTION_DELETE_RIGHT:if (askBeforeDelete()) sources_.rightImage_=Image(); break;

    case ACTION_LEFT_RIGHT_SWAP :
      if (askBeforeDelete())
      {
        // Inverts the 2 images left and right
        Image dummy = sources_.leftImage_;
        sources_.leftImage_  = sources_.rightImage_;
        sources_.rightImage_ = dummy;

        break;
      }
    case ACTION_3D_ROTATE_LEFT :    case ACTION_3D_ROTATE_RIGHT :
    case ACTION_3D_ROTATE_UP   :    case ACTION_3D_ROTATE_DOWN  :
    case ACTION_3D_ZOOM_PLUS   :    case ACTION_3D_ZOOM_MINUS   : 
      actionMove3D(actionNo); break;

    case ACTION_3D_DEPTH_PLUS  :  multiplyDepth(1.2f     );  break;
    case ACTION_3D_DEPTH_MINUS :  multiplyDepth(1.0f/1.2f);  break;

    case ACTION_3D_TEXTURE   :    case ACTION_3D_WHITE :
    case ACTION_3D_WIREFRAME :    change3DShader(actionNo);  break;

    case ACTION_TILEABLE :

      if (result_.isSeamless())  result_.cancelMode();
      else                       result_.setSeamlessMode();

      updateResult();
      reinit3DDisplay();

      break;

    case ACTION_FLAT_MAP :
      {
        if (result_.isFlat())     result_.cancelMode();
        else                      result_.setFlatMode();

        PROGRESS(UPDATING)
        
        updateResult();
        reinit3DDisplay();

        break;
      }
    case ACTION_DOWNSAMPLE : actionDownSample();      break;
      
    case ACTION_STOP_WITH_KEY:
    case ACTION_STOP:
      if (isSculpting_)
        UserInterrupt::orderStop();
      break;

    case ACTION_COPY_TO_CLIPBOARD: copyImageToClipboard();                 break;
    case ACTION_PASTE_LEFT:        pasteImageLFromClipboard() ;            break;
    case ACTION_PASTE_RIGHT:       pasteImageRFromClipboard() ;            break;
     
    case ACTION_EDIT_PINNING:      actionEditPinning();                    break;
    case ACTION_CROPEDIT:          actionCropEdit();                       break;
    case ACTION_WEBSITE:           actionShellExecute(WEB_SITE         );  break;
    case ACTION_WEBSITE_BUY:       actionShellExecute(WEB_SITE_BUY     );  break;
    case ACTION_TECHNICAL_SUPPORT: actionShellExecute(WEB_SITE_SUPPORT );  break;
    case ACTION_TUTORIAL:          actionShellExecute(WEB_SITE_TUTORIAL);  break;
    case ACTION_SUGGEST_FUNCTION:  actionShellExecute(WEB_SITE_SUGGEST );  break;
    case ACTION_FACEBOOK:          actionShellExecute(WEB_SITE_FACEBOOK );  break;
    case ACTION_TWEETER:           actionShellExecute(WEB_SITE_TWEETER );  break;
    case ACTION_ABOUT:             actionAbout();                          break;

    case ACTION_SUBDIVISION_REDUCE:
      result_.subdivision_ *= 2;
      limit( result_.subdivision_ , 1, 256 );
      updateResult();
      reinit3DDisplay();
      break;

    case ACTION_SUBDIVISION_INCREASE:
      result_.subdivision_ /= 2;
      limit( result_.subdivision_ , 1, 256 );
      updateResult();
      reinit3DDisplay();
      break;

    case ACTION_OPEN_LAST_DIRECTORY:
    {
      char text[MAX_PATH];

      bool isOk = loadLastPath(text);
      if (isOk)
        ShellExecute(interface_.windowsHandle_, "open", text, NULL, NULL, SW_SHOWNORMAL);
      else
        MESSAGEHWND("No last directory was recorded, open something first in order to use this function later");

      break;
    }
    case ACTION_OPEN_SAMPLE_DIRECTORY:
    {
      char text[MAX_PATH];

      bool isOk = loadSamplePath(text);
      if (isOk)
        ShellExecute(interface_.windowsHandle_, "open", text, NULL, NULL, SW_SHOWNORMAL);
      else
        MESSAGEHWND("No Sample directory, please reinstall the software and check the option SAMPLES if you want to use samples.");

      break;
    }
    case ACTION_ADD_DETAIL: actionAddDetail();       break;
    case ACTION_SUB_DETAIL: actionSubstractDetail(); break;

    case ACTION_SWITCH_SMOOTH_MODE:
    {
      interface_.directX_.setSmooth(!interface_.directX_.getSmooth());
      if (interface_.directX_.getSmooth()) MESSAGEHWND("Smooth mesh display mode now turned on")
      else                                 MESSAGEHWND("Smooth mesh display mode now turned off")

      updateResult();
      reinit3DDisplay();

      break;
    }
    case ACTION_ANALYSIS:        analyseAndWarnUser();        break;
    case ACTION_ANIMATE_MODEL:   actionAnimateModel();        break;
    case ACTION_OPTIONS: 
      if (OptionStore::launchDialog(&registry_, interface_.instanceHandle_, interface_.windowsHandle_))
        applyOptions();

      break;
    
    case ACTION_DETAIL_DURING_SCULPT:
       actionSwitchOption(GlobalOptions.isAutoLocalDetail_,
         "Local Detailing during sculpting now turned on",
         "Local Detailing during sculpting now turned off");
      break;

    case ACTION_CLOSE_ALPHA         : closeAlpha();                   break;
    case ACTION_CLEAR_ALL           : clearAll();                     break;
    case ACTION_ADDITIONAL_PHOTO    : loadAdditionalPhotoDialog();    break;      
    case ACTION_SEARCH_CAMERA_POS   : searchCameraPosition      ();   break;
    case ACTION_ENTER_FOCAL_LENGTH  : actionSetFocalDialog      ();   break;
    case ACTION_ENTER_BLUR_RADIUS   : actionSetBlurRadiusDialog ();   break;
    case ACTION_MULTITASKING_TRIAL  : 
      {
        PROGRESS(MULTITASK);
        MultiTaskingTrial trial;
        trial.launch();
        break;
      }
    case ACTION_REINIT_3D_DISPLAY:    reinit3DDisplay           ();   break;
    
    case ACTION_ADD_ALPHA: 
    case ACTION_EDIT_SELECTION:       actionEditSelection       ();   break;

    case SAVE_AS_PER_COMMAND_LINE:    saveAsPerCommandLine(commandLine_);            break;
    case ACTION_ZERO_DISPLACE_MAP:    zeroDisplacementMap();                         break;
    case ACTION_BATCH_SCULPT_FOLDER:  actionBatchSculpt(ACTION_BATCH_SCULPT_FOLDER); break;
    case ACTION_BATCH_SCULPT_TILES:   actionBatchSculpt(ACTION_BATCH_SCULPT_TILES);  break;
    case ACTION_COMMANDED_SCULPT:     executeCommandedExtraction(commandLine_);      break;
          
    case ACTION_CLOSE_3D_EXPORT: actionSwitchOption(GlobalOptions.isExport3DClosed_,
      "Closed 3D model in OBJ export now turned on",
      "Closed 3D model in OBJ export now turned off"); break;

    case ACTION_PERSPECTIVE_CORRECTED: actionSwitchOption(GlobalOptions.isPerspectiveCorrected_,
      "Perspective corrected 3D model now turned on",
      "Perspective corrected 3D model now turned off"); break;

    case ACTION_SEAM_CORRECTED_DIFFUSE: actionSwitchOption(GlobalOptions.isSeamCorrectedDiffuse_,
      "Seam corrected diffuse map now turned on (slow)",
      "Seam corrected diffuse map now turned off"); break;

    case ACTION_HALF_SPHERE_PREVIEW: actionSwitchOption(GlobalOptions.isHalfSpherePreview_, 
      "Half Sphere preview now turned on", 
      "Half Sphere preview now turned off"); break;
    
    case ACTION_FLAT_MODE_SAMPLE_DIALOG: actionFlatOptionsDialog ();   break;
    case ACTION_MINIMIZE_SCULPT_BAR:
      interface_.isSculptBarMinimised_ = !interface_.isSculptBarMinimised_;
      resizeMainWindow(interface_.getClientSizeX(), interface_.getClientSizeY());
      break;
    case ACTION_OPTIMIZE_SCULPT_PARAMETERS: PhotoSculpt::searchForAllParameters(); break;
    case NOACTION:
    default:
      isPendingRedraw=false; // Cancels redraw of Interface

    }// end switch

    // Redraws the complete interface now
    if (isPendingRedraw)
      requestRedraw();

  }// end try
  INTERFACE_CATCH_BLOC(ACTION)
}
