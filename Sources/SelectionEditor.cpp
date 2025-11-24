#include "SelectionEditor.h"
#include "Win32Tools.h"
#include "Frame.h"
#include "Limit.h"
#include "Options.h"

SelectionEditor::SelectionEditor(HINSTANCE hInstance, HWND parenthwnd, 
                                 const PImage alpha, const PImage left):
                 DialogWindow   (hInstance,  parenthwnd, IDS_APP_TITLE_SELECTEDIT, 
                                 IDC_SELECTEDITOR, IDI_SMALL, IDC_SELECTEDITOR)
{
  isOk_           = false;

  // alpha_ is a separate copy of the alpha Image given as argument
  alpha_             = new Image(*alpha) ;
  alphaSavedForUndo_ = new Image(*alpha) ;

  source_ = left ;
  if (alpha_->is_empty())
  {
    if (source_->is_empty())
      alpha_ = new Image(1024, 1024, 1, 3);
    else
      alpha_ = new Image(source_->dimx(), source_->dimy(), 1, 3);

    alpha_ -> fill (255); // fill with white
  }

  color_          = ACTION_SELECT_BLACK   ;
  tool_           = ACTION_SELECT_LASSO   ;

  initButtons_  ();
  setButtonLogic();

  frame_.init   (hInstance, "IMAGE_FRAME", 12);
}


PImage SelectionEditor::getAlpha()
{
  return alpha_;
}

void SelectionEditor::displayToWindow()
{
  HDC hdc = GetDC(paintZonehWnd_);

  renderSourceImages_();
  buttons_.drawBackground();
  buttons_.draw();

  float floatlx, floatly;
  bool isInLeft = monoScreen_.isCursorInImage(mouseX_,mouseY_,floatlx,floatly);

  bool isOk2 = Win32Tools::copyImageToDC(interfaceImage_, hdc);

  ReleaseDC(paintZonehWnd_, hdc);
}

void SelectionEditor::setButtonLogic()
{
  // clears all highlights
  buttons_.declareStatusOff( ACTION_SELECT_WHITE   );
  buttons_.declareStatusOff( ACTION_SELECT_BLACK   );
  buttons_.declareStatusOff( ACTION_SELECT_LASSO   );
  buttons_.declareStatusOff( ACTION_SELECT_POLYGON );

  buttons_.declareStatusOn( color_ );
  buttons_.declareStatusOn( tool_  );
}

unsigned char COLOR_WHITE [] = {255,255,255} ;
unsigned char COLOR_BLACK [] = {0,0,0}       ;

#define MAX_LASSO_LOOPING_DISTANCE_PIXELS 10.0f

bool isDetectLoop(const cimg_library::CImgList<int> & points, int dx, int dy)
{
  int nbitems = points.size;

  if (nbitems<=2) return false;

  int firstx, firsty;

  firstx = points[0].operator()(0,0) ;
  firsty = points[0].operator()(0,1) ;

  float distance = sqrtf( float(firstx-dx)*float(firstx-dx) + float(firsty-dy)*float(firsty-dy) ) ;

  if (distance < MAX_LASSO_LOOPING_DISTANCE_PIXELS)     return true ; // it's a loop
  else                                                  return false;
}


void SelectionEditor::detectAction()
{
  float mousefloatx, mousefloaty;
  bool isInside = monoScreen_.isCursorInImage(mouseX_,mouseY_,mousefloatx,mousefloaty);
  bool isinsideinterface = buttons_.isInsideInterface(mouseX_,mouseY_);

  static int oldButton = 0;
  bool released = (oldButton==1 && mouseButton_==0);
  bool clicked  = (oldButton==0 && mouseButton_==1);
  oldButton=mouseButton_;

  if (!isinsideinterface)
  {
    bool isAddPointsToSelection = false;
    bool isApplyPaintNow        = false;

    if (tool_ == ACTION_SELECT_LASSO  ) 
    {
      // adds points if left mouse button is pressed    
      isAddPointsToSelection = (mouseButton_ == 1) && (isInside || !selectedPoints_.is_empty());
      // apply paint as soon as mouse is released
      isApplyPaintNow = released && !selectedPoints_.is_empty();
    }
    else if (tool_ == ACTION_SELECT_POLYGON) 
    {
      if (clicked) // adds points if left mouse button is clicked down (not just kept pressed)
      {
        isAddPointsToSelection = isInside || !selectedPoints_.is_empty();

        if (isAddPointsToSelection)    // apply paint as soon as a selection loop is done
        {
          int dx,dy; // coordinates in displayed image
          monoScreen_.convertCoordToImage(mousefloatx, mousefloaty, dx,dy);

          isApplyPaintNow = isDetectLoop(displayedPoints_, dx, dy);
        }

        if (isApplyPaintNow) // don't cumulate adding a point and painting, priority is paint only
          isAddPointsToSelection = false;  
      }
    }

    if (isAddPointsToSelection)
      addPointToLasso(mousefloatx, mousefloaty);

    if (isApplyPaintNow)  
      applyPaintWithLasso();  // drops the filled polygon on the final alpha map
  }

  if (displayedPoints_.is_empty()) // user is not currently "lassoing"
  {
     int action = buttons_.detectUserAction(mouseX_,mouseY_,mouseButton_);
     doAction( action );
  }
}

void SelectionEditor::addPointToLasso(float mousefloatx, float mousefloaty)
{
  MonoScreen alphamonoscreen;
  alphamonoscreen.set(0,0, alpha_->dimx(), alpha_->dimy());
  limit(mousefloatx, 0.0f, 1.0f);
  limit(mousefloaty, 0.0f, 1.0f);

  int x, y; // map coordinates
  alphamonoscreen.convertCoordToImage(mousefloatx, mousefloaty, x,y);

  // adds the very first point twice because img::draw_polygon dont display a polygon with 2 points only = strange bug
  int nbtimes = 1;
  if (selectedPoints_.size == 0) 
    nbtimes = 2;

  for (int i=0; i< nbtimes ; i++)
  {
    cimg_library::CImg<int> currentpoint(1,2,1,1);
    currentpoint(0,0) = x; currentpoint(0,1) = y;
    selectedPoints_ << currentpoint ; // adds this point to the list "SelectedPoints_"

    int dx,dy; // display coordinates
    monoScreen_.convertCoordToImage(mousefloatx, mousefloaty, dx,dy);
    currentpoint(0,0) = dx; currentpoint(0,1) = dy;
    displayedPoints_ << currentpoint ; // adds this point to the list "displayedPoints_"
  }
}

void SelectionEditor::applyPaintWithLasso()
{
  alphaSavedForUndo_ = new Image(*alpha_);

  int sizex = alpha_->dimx();
  int sizey = alpha_->dimy();
  int maxsquare = (sizex>sizey)?sizex:sizey;

  Image square (maxsquare, maxsquare, 1,3);
  square.draw_image(*alpha_, 0,0);

  if      (color_ == ACTION_SELECT_BLACK)
    square.draw_polygon(selectedPoints_, COLOR_BLACK, 1.0f);
  else if (color_ == ACTION_SELECT_WHITE)
    square.draw_polygon(selectedPoints_, COLOR_WHITE, 1.0f);

  *alpha_ = alpha_->draw_image(square,0,0);

  selectedPoints_.clear();
  displayedPoints_.clear();
}

void SelectionEditor::cancelLasso()
{
  selectedPoints_.clear();
  displayedPoints_.clear();
}


void SelectionEditor::doAction(int action)
{
  switch (action)
  {
  case ACTION_SELECT_OK:
    if (displayedPoints_.is_empty()) // user is not currently "lassoing"
    {    
      isOk_ = true;
      destroy();
    }
    else
      applyPaintWithLasso();

    break;

  case ACTION_SELECT_CANCEL:
    if (displayedPoints_.is_empty()) // user is not currently "lassoing"
    {      
      isOk_ = false;
      destroy();
    }
    else
      cancelLasso();

    break;

  case ACTION_REPAINT_SCREEN:    requestRedraw();    break;

  case ACTION_SELECT_INVERT:
    {
      alphaSavedForUndo_ = new Image(*alpha_);

      Image i(alpha_->dimx(), alpha_->dimy(), 1, 3);
      i.fill(255);
      alpha_ = new Image (i - *alpha_ );
      break;
    }
  case ACTION_SELECT_CLEAR:
    alphaSavedForUndo_ = new Image(*alpha_);
    alpha_ -> fill (255);
    break;

  case ACTION_SELECT_LASSO:
  case ACTION_SELECT_POLYGON:      tool_ = action;       break;

  case ACTION_SELECT_WHITE:
  case ACTION_SELECT_BLACK:        color_ = action;      break;

  case ACTION_SELECT_UNDO:
    {
      Image buffer       = *alpha_;
      alpha_             = new Image (*alphaSavedForUndo_);
      alphaSavedForUndo_ = new Image(buffer);
      break;
    }
  }
  setButtonLogic();
}

#define OPACITY_OF_ALPHA_MASK 0.60f

void SelectionEditor::renderSourceImages_()
{
  // white
  interfaceImage_.fill(255); 

  const int FRAME = 20;

  int windowsizex=getClientSizeX_() - FRAME;
  int windowsizey=getClientSizeY_() - FRAME - buttons_.getSizeY();

  int resizedxl = alpha_->dimx(), resizedyl = alpha_->dimy();

  if (!source_->is_empty()) 
  {
    resizedxl = source_->dimx();
    resizedyl = source_->dimy();
  }

  Frame::fit(resizedxl, resizedyl, windowsizex, windowsizey);

  if (resizedxl>0 && resizedyl>0)
  {
    int xl= ( getClientSizeX_()                       - resizedxl ) / 2 ;
    int yl= ( getClientSizeY_() - buttons_.getSizeY() - resizedyl ) / 2 ;
    monoScreen_.set(xl,yl, resizedxl, resizedyl);

    Image alphareduced = alpha_->get_resize(resizedxl, resizedyl);

    if (!source_->is_empty())
    {
      Image leftreduced = source_->get_resize(resizedxl, resizedyl);
      alphareduced.draw_image(leftreduced,0,0,0,0, 1.0f - OPACITY_OF_ALPHA_MASK);
    }

    interfaceImage_.draw_image(alphareduced, xl, yl);
    frame_.draw(interfaceImage_, xl,yl, xl+resizedxl-1, yl+resizedyl-1);

    if (!displayedPoints_.is_empty())
    {
      if      (color_ == ACTION_SELECT_BLACK)
        interfaceImage_.draw_polygon(displayedPoints_, COLOR_BLACK, 1.0f, ~0U);
      else if (color_ == ACTION_SELECT_WHITE)
        interfaceImage_.draw_polygon(displayedPoints_, COLOR_WHITE, 1.0f, ~0U);
    }

    if (tool_ == ACTION_SELECT_POLYGON)
    {
      if (!displayedPoints_.is_empty()) // draw rubber line from mouse cursor to last point
      {
        int lastitem = displayedPoints_.size - 1;
        int lastx = displayedPoints_[lastitem].operator()(0,0);
        int lasty = displayedPoints_[lastitem].operator()(0,1);
        
        int firstx = displayedPoints_[0].operator()(0,0);
        int firsty = displayedPoints_[0].operator()(0,1);
 
        const float         OPACITY                       = 0.5f ;
        const int           RADIUS_OF_CLOSE_SYMBOL_CIRCLE = 5 ;
        const unsigned char COLOR_OF_CLOSE_SYMBOL[3]      = { 255, 0, 0 } ;

        // draw 2 rubber lines attached to the mouse
        if      (color_ == ACTION_SELECT_BLACK)
        {
          interfaceImage_.draw_line  (mouseX_,mouseY_, lastx,  lasty,  COLOR_BLACK, OPACITY);        
          interfaceImage_.draw_line  (mouseX_,mouseY_, firstx, firsty, COLOR_BLACK, OPACITY);
        }
        else if (color_ == ACTION_SELECT_WHITE)
        {
          interfaceImage_.draw_line  (mouseX_,mouseY_, lastx,  lasty,  COLOR_WHITE, OPACITY);        
          interfaceImage_.draw_line  (mouseX_,mouseY_, firstx, firsty, COLOR_WHITE, OPACITY);
        }

        // draw a close symbol circle
        interfaceImage_.draw_circle(firstx, firsty, RADIUS_OF_CLOSE_SYMBOL_CIRCLE, 
                                    COLOR_OF_CLOSE_SYMBOL, OPACITY);
      }
    }
  }
}

void SelectionEditor::resizeMainWindow(int newSizeX, int newSizeY)
{
  DialogWindow::resizeMainWindow(newSizeX, newSizeY);

  cancelLasso();

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                              getClientSizeY_()-buttons_.getSizeY()    );
}

bool SelectionEditor::isOk()
{
  return isOk_ ;
}

extern Options * official_option;

void SelectionEditor::initButtons_()
{
  Image normal       = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_SELECT"  );
  Image hide_out     = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_SELECT_G");
  Image highlights   = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_SELECT_H");
  Image pressed_down = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_SELECT_S");
  Image turn_on      = Win32Tools::loadImageFromResource(interfacehInst_, "IMAGE_SELECT_A");

  buttons_ = ButtonManager();
  buttons_.setOutput(interfaceImage_);
  buttons_.storeBackground(hide_out);

  buttons_.defineNewButton (ACTION_SELECT_WHITE,        81,3,  122,58, "white"      );
  buttons_.defineNewButton (ACTION_SELECT_BLACK,        123,3, 163,58, "black"      );
  buttons_.defineNewButton (ACTION_SELECT_LASSO,        185,3, 226,58, "lasso"      );
  buttons_.defineNewButton (ACTION_SELECT_POLYGON,      227,3, 269,58, "polygon"    );
  buttons_.defineNewButton (ACTION_SELECT_INVERT,       270,3, 312,58, "invert"     );
  buttons_.defineNewButton (ACTION_SELECT_CLEAR,        313,3, 356,58, "clear"      );
  buttons_.defineNewButton (ACTION_SELECT_UNDO,         379,3, 423,58, "undo"       );
  buttons_.defineNewButton (ACTION_SELECT_CANCEL,       452,0, 537,28, "cancel"     );
  buttons_.defineNewButton (ACTION_SELECT_OK,           452,29,537,59, "ok"         );

  buttons_.declareTextOnButton(ACTION_SELECT_WHITE,       "White",      8, 40);
  buttons_.declareTextOnButton(ACTION_SELECT_BLACK,       "Black",      7, 40);
  buttons_.declareTextOnButton(ACTION_SELECT_LASSO,       "Lasso",      5, 40);
  buttons_.declareTextOnButton(ACTION_SELECT_POLYGON,     "Polygon",    2, 40);
  buttons_.declareTextOnButton(ACTION_SELECT_INVERT,      "Invert",     5, 40);
  buttons_.declareTextOnButton(ACTION_SELECT_CLEAR,       "Clear",      5, 40);
  buttons_.declareTextOnButton(ACTION_SELECT_UNDO,        "Undo",       10,40);
  buttons_.declareTextOnButton(ACTION_SELECT_CANCEL,      "Cancel",     25,10);
  buttons_.declareTextOnButton(ACTION_SELECT_OK,          "Ok",         35,8 );

  buttons_.loadAllImages     ( normal       );
  buttons_.loadAllImagesHoover( highlights   );
  buttons_.loadAllImagesInactive ( hide_out     );
  buttons_.loadAllImagesSelected  ( pressed_down );
  buttons_.loadAllImagesClicked    ( turn_on      );
  buttons_.loadAllImagesClickedAndSelected ( highlights   );

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()     );

  buttons_.repaintStoredButtons(official_option->hue_, official_option->saturation_);
}
