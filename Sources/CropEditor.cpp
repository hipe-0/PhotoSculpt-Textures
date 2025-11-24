#include "CropEditor.h"
#include "Win32Tools.h"
#include "Frame.h"
#include "Limit.h"
#include "Options.h"

unsigned char COLOR_CREATE[] ;
unsigned char COLOR_DELETE[] ;
unsigned char COLOR_NORMAL[] ;
unsigned char COLOR_FRAME [] = {0,0,0} ;

CropEditor::CropEditor  (HINSTANCE hInstance, HWND parenthwnd):
            DialogWindow(hInstance,  parenthwnd, IDS_APP_TITLE_CROPEDIT, 
                         IDC_CROPEDITOR, IDI_SMALL, IDC_CROPEDITOR)
{
  isOk_   = false;

  initButtons_();
  setButtonLogic();

  source_ = new Image();
  cropDisplayed_ = NULL  ;

  frame_.init(hInstance, "IMAGE_FRAME", 12);
}

void CropEditor::setCropperAndImage(Cropper* cropper, Image& source)
{
  source_        = new Image(source);
  cropEdited_    = *cropper ;
  cropBackup_    = *cropper ;
  cropDisplayed_ = cropper  ;
}

void CropEditor::displayToWindow()
{
  HDC hdc = GetDC(paintZonehWnd_);

  renderSourceImages_();
  buttons_.drawBackground();
  buttons_.draw();

  float floatlx, floatly;
  bool isInLeft = monoScreen_.isCursorInImage(mouseX_,mouseY_,floatlx,floatly);

  float x1,y1,x2,y2,x3,y3,x4,y4;
  
  cropEdited_.getCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);

  if (cropEdited_.isCrop())
  {
    // Draw the outside frame
    if (!cropEdited_.isPerspective())
    {
      monoScreen_.drawFrame (x1, y1, x2, y2, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x1, y1, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x1, y2, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x2, y1, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x2, y2, interfaceImage_, COLOR_FRAME, 1.0f);
    }
    else
    {
      monoScreen_.drawFrame (x1, y1, x2, y2, x3, y3, x4, y4, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x1, y1, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x2, y2, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x3, y3, interfaceImage_, COLOR_FRAME, 1.0f);
      monoScreen_.drawDot (x4, y4, interfaceImage_, COLOR_FRAME, 1.0f);
    }

    if (cropEdited_.isSeamless())
    {
      // Draws the inside frame if the seamless tileable mode is on
      float sx = (x2-x1) * cropEdited_.getFrameSize();
      float sy = (y2-y1) * cropEdited_.getFrameSize();

      if (!cropEdited_.isPerspective())
        // that's easy it's a rectangle
        monoScreen_.drawFrame (x1+sx, y1+sy, x2-sx, y2-sy, interfaceImage_, COLOR_FRAME, 0.8f);
      else
      {
        // that's a bit more complicated as it's a polygon
        float centerx = (x1+x2+x3+x4)/4;
        float centery = (y1+y2+y3+y4)/4;
        
        float ratio = 1.0f - 1.0f * cropEdited_.getFrameSize() * 2.0f;

        float newx1 = (x1-centerx) * ratio + centerx;
        float newx2 = (x2-centerx) * ratio + centerx;
        float newx3 = (x3-centerx) * ratio + centerx;
        float newx4 = (x4-centerx) * ratio + centerx;
        float newy1 = (y1-centery) * ratio + centery;
        float newy2 = (y2-centery) * ratio + centery;
        float newy3 = (y3-centery) * ratio + centery;
        float newy4 = (y4-centery) * ratio + centery;

        monoScreen_.drawFrame (newx1, newy1, newx2, newy2, newx3, newy3, newx4, newy4, 
          interfaceImage_, COLOR_FRAME, 1.0f);
      }
    }
  }  

  bool isOk2 = Win32Tools::copyImageToDC(interfaceImage_, hdc);

  ReleaseDC(paintZonehWnd_, hdc);
}

void CropEditor::setButtonLogic()
{
  if (cropEdited_.isSquare()) buttons_.declareStatusOn (ACTION_CROPEDIT_MODE_SQUARE);
  else                        buttons_.declareStatusOff(ACTION_CROPEDIT_MODE_SQUARE);

  if (cropEdited_.is2N())     buttons_.declareStatusOn (ACTION_CROPEDIT_MODE_2N);
  else                        buttons_.declareStatusOff(ACTION_CROPEDIT_MODE_2N);

  if (cropEdited_.isSeamless())  
  {
    buttons_.declareStatusOn (ACTION_TILEABLE);
    buttons_.declare_active  (ACTION_CROPEDIT_FRAME_MINUS);
    buttons_.declare_active  (ACTION_CROPEDIT_FRAME_PLUS);
  }
  else                    
  {
    buttons_.declareStatusOff  (ACTION_TILEABLE);
    buttons_.declare_grayed_out(ACTION_CROPEDIT_FRAME_MINUS);
    buttons_.declare_grayed_out(ACTION_CROPEDIT_FRAME_PLUS);
  }
  
  if (cropEdited_.isCrop()) buttons_.declare_active    (ACTION_CROPEDIT_DELETE);
  else                      buttons_.declare_grayed_out(ACTION_CROPEDIT_DELETE);
  
  if (cropEdited_.isPerspective()) buttons_.declareStatusOn (ACTION_CROPEDIT_PERSPECTIVE);
  else                             buttons_.declareStatusOff(ACTION_CROPEDIT_PERSPECTIVE);
}

#include "PSApplication.h"
extern PSApplication MainApp;

void CropEditor::doAction(int action)
{
  DEBUG(CropEd);
  switch (action)
  {
  case ACTION_CROPEDIT_OK:
    finalise();
    destroy();
    MainApp.updateResult();
    MainApp.reinit3DDisplay();
    break;

  case ACTION_CROPEDIT_APPLY:
    finalise();
    MainApp.updateResult();
    MainApp.reinit3DDisplay();
    MainApp.requestRedraw();
    break;

  case ACTION_CROPEDIT_CANCEL:
    *cropDisplayed_ = cropBackup_ ;
    destroy();
    break;

  case ACTION_REPAINT_SCREEN:    requestRedraw();    break;

  case ACTION_CROPEDIT_MODE_SQUARE:
    cropEdited_.setSquareRectify( !cropEdited_.isSquare(), monoScreen_.getSizeX(), 
                                                           monoScreen_.getSizeY());
    break;

  case ACTION_CROPEDIT_MODE_2N:
    cropEdited_.set2N( ! cropEdited_.is2N() );

    if (! cropEdited_.isSquare())
      cropEdited_.setSquareRectify( true, monoScreen_.getSizeX(), 
                                          monoScreen_.getSizeY());
    break;

  case ACTION_TILEABLE:
    cropEdited_.setSeamless( ! cropEdited_.isSeamless() );

    if (cropEdited_.isSeamless()) // if seamless mode is on, turn on square mode too
      if (! cropEdited_.isSquare())
        cropEdited_.setSquareRectify( true, monoScreen_.getSizeX(), 
                                            monoScreen_.getSizeY());
    
    break;

  case ACTION_CROPEDIT_DELETE:    cropEdited_.cancel();    break;

  case ACTION_CROPEDIT_FRAME_PLUS:
    cropEdited_.setFrameSize(cropEdited_.getFrameSize() + 0.01f );
    break;

  case ACTION_CROPEDIT_FRAME_MINUS:
    cropEdited_.setFrameSize(cropEdited_.getFrameSize() - 0.01f );
    break;

  case ACTION_CROPEDIT_PERSPECTIVE:
    {
      if (!cropEdited_.isPerspective())
      {
        cropEdited_.setPerspective(true);

        if (!cropEdited_.isCrop())
          cropEdited_.setCropDimensions(0.0f, 0.0f, 1.0f, 1.0f);

        float x1,y1,x2,y2;
        cropEdited_.getCropDimensions(x1,y1,x2,y2);
        cropEdited_.setCropDimensions(x1,y1,x2,y1,x1,y2,x2,y2);
      }
      else
      {
        cropEdited_.setPerspective(false);

        float x1,y1,x2,y2,x3,y3,x4,y4;
        cropEdited_.getCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);
        cropEdited_.setCropDimensions(x1,y1,x4,y4);

        if (cropEdited_.isSquare())
          cropEdited_.makeSquare ( float(monoScreen_.getSizeX()), 
                             float(monoScreen_.getSizeY()), 0 );
      }
      break;
    }
  }
}

void CropEditor::detectAction()
{
  bool isinsideinterface = buttons_.isInsideInterface(mouseX_,mouseY_);
  int action = buttons_.detectUserAction(mouseX_,mouseY_,mouseButton_);

  doAction(action);

  setButtonLogic();

  static int oldButton = 0;
  bool released = (oldButton==1 && mouseButton_==0);
  bool clicked  = (oldButton==0 && mouseButton_==1);
  oldButton=mouseButton_;

  if (!isinsideinterface)
  {
    float mousefloatx, mousefloaty;
    monoScreen_.isCursorInImage(mouseX_,mouseY_,mousefloatx,mousefloaty);
    limit(mousefloatx, 0.0f, 1.0f);
    limit(mousefloaty, 0.0f, 1.0f);

    float x1,y1,x2,y2,x3,y3,x4,y4;
    cropEdited_.getCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);

    static int corner = HANDLE_CORNER_NOT_DETECTED;

    if (mouseButton_ == 0)
      corner = HANDLE_CORNER_NOT_DETECTED;

    if (cropEdited_.isCrop() && clicked)
      corner = detectHandle_( mousefloatx, mousefloaty );

    //if (mouseButton_) // dragging
      //corner = switchHandle_( corner, mousefloatx, mousefloaty);

    if (corner == HANDLE_CORNER_NOT_DETECTED)
    {
      if (!cropEdited_.isPerspective())
      {
        if (clicked) // only once at the beggining
        {
          x1 = mousefloatx;  y1 = mousefloaty;
          x2 = mousefloatx;  y2 = mousefloaty;
          corner = HANDLE_CORNER_BOTTOM_RIGHT;
          cropEdited_.setCropDimensions(x1,y1,x2,y2);
        }
      }
      else // mode perspective
      {
        if (clicked)
        {
          x1 = mousefloatx;  y1 = mousefloaty;
          x2 = mousefloatx;  y2 = mousefloaty;
          x3 = mousefloatx;  y3 = mousefloaty;
          x4 = mousefloatx;  y4 = mousefloaty;
          corner = HANDLE_CORNER_ALL ;
          cropEdited_.setCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);
        }
      }
    }
    else if (corner != HANDLE_CORNER_NOT_DETECTED && mouseButton_)
      dragHandle_(corner, mousefloatx, mousefloaty);
  }
}

#define DETECT_HANDLE_DISTANCE 20

int CropEditor::detectHandle_(float mousefloatx, float mousefloaty)
{
  if (!cropEdited_.isPerspective())
  {
    float x1,y1,x2,y2;
    cropEdited_.getCropDimensions(x1,y1,x2,y2);

    int corner = HANDLE_CORNER_NOT_DETECTED;

    int distance = monoScreen_.getDistance(x1,y1,mousefloatx,mousefloaty);
    int bestdistance = distance;
    if (distance < DETECT_HANDLE_DISTANCE)
      corner = HANDLE_CORNER_TOP_LEFT;

    distance = monoScreen_.getDistance(x2,y1,mousefloatx,mousefloaty);
    if (distance < DETECT_HANDLE_DISTANCE && distance < bestdistance)
    {
      corner = HANDLE_CORNER_TOP_RIGHT;
      bestdistance=distance;
    }

    distance = monoScreen_.getDistance(x1,y2,mousefloatx,mousefloaty);
    if (distance < DETECT_HANDLE_DISTANCE && distance < bestdistance)
    {
      corner = HANDLE_CORNER_BOTTOM_LEFT;
      bestdistance=distance;
    }  

    distance = monoScreen_.getDistance(x2,y2,mousefloatx,mousefloaty);
    if (distance < DETECT_HANDLE_DISTANCE && distance < bestdistance)
      corner = HANDLE_CORNER_BOTTOM_RIGHT;

    return corner;
  }
  else
  {
    float x1,y1,x2,y2,x3,y3,x4,y4;
    cropEdited_.getCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);

    int corner = HANDLE_CORNER_NOT_DETECTED;

    int distance = monoScreen_.getDistance(x1,y1,mousefloatx,mousefloaty);
    int bestdistance = distance;
    if (distance < DETECT_HANDLE_DISTANCE)
      corner = HANDLE_CORNER_TOP_LEFT;

    distance = monoScreen_.getDistance(x2,y2,mousefloatx,mousefloaty);
    if (distance < DETECT_HANDLE_DISTANCE && distance < bestdistance)
    {
      corner = HANDLE_CORNER_TOP_RIGHT;
      bestdistance=distance;
    }

    distance = monoScreen_.getDistance(x3,y3,mousefloatx,mousefloaty);
    if (distance < DETECT_HANDLE_DISTANCE && distance < bestdistance)
    {
      corner = HANDLE_CORNER_BOTTOM_LEFT;
      bestdistance=distance;
    }

    distance = monoScreen_.getDistance(x4,y4,mousefloatx,mousefloaty);
    if (distance < DETECT_HANDLE_DISTANCE && distance < bestdistance)
      corner = HANDLE_CORNER_BOTTOM_RIGHT;

    return corner;
  }
}

static void swap(float& a, float& b) { float c=a; a=b ; b=c; }

int CropEditor::switchHandle_ (int corner, float mousefloatx, float mousefloaty )
{
  if (!cropEdited_.isPerspective())
  {
    if (corner != HANDLE_CORNER_NOT_DETECTED)
    { 
      float x1,y1,x2,y2;
      cropEdited_.getCropDimensions(x1,y1,x2,y2);

      if (x1 > x2) swap (x1, x2); // reorder the points
      if (y1 > y2) swap (y1, y2);

      if (x2 == mousefloatx && y2 == mousefloaty) return HANDLE_CORNER_BOTTOM_RIGHT;
      if (x1 == mousefloatx && y1 == mousefloaty) return HANDLE_CORNER_TOP_LEFT;
      if (x2 == mousefloatx && y1 == mousefloaty) return HANDLE_CORNER_TOP_RIGHT;
      if (x1 == mousefloatx && y2 == mousefloaty) return HANDLE_CORNER_BOTTOM_LEFT;

      cropEdited_.setCropDimensions(x1,y1,x2,y2);
    }
  }
  return HANDLE_CORNER_NOT_DETECTED;
}



void CropEditor::dragHandle_(int corner, float mousefloatx, float mousefloaty )
{
  DEBUG(dragHan);
  
  if (!cropEdited_.isPerspective())
  {
    if (corner != HANDLE_CORNER_NOT_DETECTED)
    {
      float x1,y1,x2,y2;
      cropEdited_.getCropDimensions(x1,y1,x2,y2);

      switch (corner)
      {
        case HANDLE_CORNER_TOP_LEFT    : x1 = mousefloatx;  y1 = mousefloaty;  break;
        case HANDLE_CORNER_TOP_RIGHT   : x2 = mousefloatx;  y1 = mousefloaty;  break;
        case HANDLE_CORNER_BOTTOM_LEFT : x1 = mousefloatx;  y2 = mousefloaty;  break;
        case HANDLE_CORNER_BOTTOM_RIGHT: x2 = mousefloatx;  y2 = mousefloaty;  break;
        case HANDLE_CORNER_ALL         : x2 = mousefloatx;  y2 = mousefloaty;  break;
      }
      
      cropEdited_.setCropDimensions(x1,y1,x2,y2);

      if (cropEdited_.isSquare())
      {
        corner = switchHandle_( corner, mousefloatx, mousefloaty);

        cropEdited_.makeSquare ( float(monoScreen_.getSizeX()), 
                                 float(monoScreen_.getSizeY()), corner );
      }
    }
  }
  else
    if (corner != HANDLE_CORNER_NOT_DETECTED)
    {
      float x1,y1,x2,y2,x3,y3,x4,y4;
      cropEdited_.getCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);

      switch (corner)
      {
        case HANDLE_CORNER_TOP_LEFT    : x1 = mousefloatx;  y1 = mousefloaty;  break;
        case HANDLE_CORNER_TOP_RIGHT   : x2 = mousefloatx;  y2 = mousefloaty;  break;
        case HANDLE_CORNER_BOTTOM_LEFT : x3 = mousefloatx;  y3 = mousefloaty;  break;
        case HANDLE_CORNER_BOTTOM_RIGHT: x4 = mousefloatx;  y4 = mousefloaty;  break;
        case HANDLE_CORNER_ALL         : x2 = x4 = mousefloatx;  y3 = y4 = mousefloaty;  break;
      }
      
      cropEdited_.setCropDimensions(x1,y1,x2,y2,x3,y3,x4,y4);
    }
}

void CropEditor::renderSourceImages_()
{
  Image& leftimage  = *source_;

  // white
  interfaceImage_.fill(255); 

  const int frame = 20;

  int windowsizex=getClientSizeX_() - frame;
  int windowsizey=getClientSizeY_() - frame - buttons_.getSizeY();

  if (!leftimage.is_empty()) 
  {
    int resizedxl,resizedyl;

    Image leftreduced; 

    resizedxl = leftimage.dimx();
    resizedyl = leftimage.dimy();
    Frame::fit(resizedxl, resizedyl, windowsizex, windowsizey);

    if (resizedxl>0 && resizedyl>0)
      leftreduced  = leftimage .get_resize(resizedxl, resizedyl);

    int xl= ( getClientSizeX_()                       - resizedxl ) / 2 ;
    int yl= ( getClientSizeY_() - buttons_.getSizeY() - resizedyl ) / 2 ;

    if (!leftreduced.is_empty())
    {
      interfaceImage_.draw_image(leftreduced, xl, yl);
      frame_.draw(interfaceImage_, xl,yl, xl+resizedxl-1, yl+resizedyl-1);
    }

    monoScreen_.set(xl,yl, resizedxl, resizedyl);
    snapDistance_= float (CURSOR_SNAP_DISTANCE) / float(resizedxl);
  }
}

void CropEditor::resizeMainWindow(int newSizeX, int newSizeY)
{
  // calls the generic version of the fuction
  DialogWindow::resizeMainWindow(newSizeX, newSizeY);

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()    );
}

bool CropEditor::isOk()
{
  return isOk_ ;
}

void CropEditor::finalise()
{
  cropEdited_ .correct();
  isOk_ = true ;
  *cropDisplayed_ = cropEdited_ ;
}

extern Options * official_option;

void CropEditor::initButtons_()
{
  Image normal       = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_CROP"  );
  Image hide_out     = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_CROP_G");
  Image highlights   = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_CROP_H");
  Image pressed_down = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_CROP_S");
  Image turn_on      = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_CROP_A");

  buttons_ = ButtonManager();
  buttons_.setOutput(interfaceImage_);

  buttons_.storeBackground(hide_out);

  buttons_.defineNewButton (ACTION_CROPEDIT_MODE_SQUARE, 86,3,  135,58, "square"     );
  buttons_.defineNewButton (ACTION_CROPEDIT_MODE_2N,     136,3, 182,58, "2^n size"   );
  buttons_.defineNewButton (ACTION_CROPEDIT_PERSPECTIVE, 183,3, 232,58, "persp"      );
  buttons_.defineNewButton (ACTION_TILEABLE,             233,3, 280,58, "tileable"   );
  buttons_.defineNewButton (ACTION_CROPEDIT_FRAME_PLUS,  281,3, 331,58, "BlendRadius+" );
  buttons_.defineNewButton (ACTION_CROPEDIT_FRAME_MINUS, 332,3, 382,58, "BlendRadius-" );
  buttons_.defineNewButton (ACTION_CROPEDIT_DELETE,      383,3, 429,58, "delete"     );
  buttons_.defineNewButton (ACTION_CROPEDIT_CANCEL,      426,0, 479,28, "cancel"     );
  buttons_.defineNewButton (ACTION_CROPEDIT_APPLY,       480,0, 538,28, "Apply"      );
  buttons_.defineNewButton (ACTION_CROPEDIT_OK,          426,29,538,59, "ok"         );

  buttons_.declareTextOnButton(ACTION_CROPEDIT_MODE_SQUARE, "Square",   10,40);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_MODE_2N,     "2^n size", 0,40);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_PERSPECTIVE, "Persp.",   10,40);
  buttons_.declareTextOnButton(ACTION_TILEABLE,             "Tileable", 2,40);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_FRAME_PLUS,  "Blend+",   7,40);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_FRAME_MINUS, "Blend-",   5,40);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_DELETE,      "Delete",   7,40);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_CANCEL,      "Cancel",   10,10);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_APPLY,       "Apply",    14,10);
  buttons_.declareTextOnButton(ACTION_CROPEDIT_OK,          "Ok",       47,8);

  buttons_.loadAllImages     (normal);
  buttons_.loadAllImagesHoover(highlights);
  buttons_.loadAllImagesInactive (hide_out);
  buttons_.loadAllImagesSelected  (pressed_down);
  buttons_.loadAllImagesClicked    (turn_on);
  buttons_.loadAllImagesClickedAndSelected (highlights);

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()     );

  buttons_.repaintStoredButtons(official_option->hue_, official_option->saturation_);
}

Cropper CropEditor::getCropper()
{
  return cropEdited_;
}