/* MorphingEditor.cpp
 *
 *  Created on: 7 fev. 2011
 *      Author: Hippolyte Mounier, Copyright 2011 All rights reserved
 */

#include "MorphingEditor.h"
#include "RESOURCE.H"
#include "Win32Tools.h"
#include "PhotoSculptTools.h"
#include "Limit.h"
#include "Includes.h"
#include "Color.h"
#include "Frame.h"
#include "Square.h"

static const int DOT_SIZE_RADIUS_PIXELS = 4  ;
static const int DETECTION_PIXEL_SIZE   = 40 ;
static unsigned char EDIT_MESH_LINE_COLOR[3] = { 0, 0, 0 };

static int MAX_MODEL_SIZE_AFTER_MORPHING_EDITION = 100;

// Constructor of the interface
MorphingEditor::MorphingEditor(HINSTANCE hinstance, HWND parenthwnd, PhotoSculpt &sculpt):
                DialogWindow  (hinstance,  parenthwnd, IDS_APP_TITLE_EDIT, 
                               IDC_EDITOR, IDI_SMALL, IDC_EDITOR)
{
  isOk_             = false;
  isDraging_        = false;
  sculpt_           = &sculpt;

  inputMapLevel_    = sculpt_ -> computeLevel();
  inputMapSizeX_    = sculpt_ -> computeLeftImage(inputMapLevel_).dimx();
  inputMapSizeY_    = sculpt_ -> computeLeftImage(inputMapLevel_).dimy();

  editedMapLevel_   = sculpt_ -> computeLevelUsingSize(1200,800);

  editedLeftImage_  = sculpt_ -> computeLeftImage (editedMapLevel_);
  editedRightImage_ = sculpt_ -> computeRightImage(editedMapLevel_);
  editedMapSizeX_   = editedLeftImage_.dimx();
  editedMapSizeY_   = editedLeftImage_.dimy();

  // sets the working displacement map to this size
  editedMap_        = sculpt_ -> getMap().getResizeAndMultiply(editedMapSizeX_,editedMapSizeY_);

  // reload the background image for the buttons
  background_       = Image(60,600,1,3).fill(255);

  bandSize_         = 100;
  xoffset_          = background_.dimx() + bandSize_;
  yoffset_          = bandSize_;

  windowSizeX_      = editedMapSizeX_ + xoffset_ + bandSize_;
  windowSizeY_      = editedMapSizeY_ + bandSize_ * 2;

  if (windowSizeY_ < background_.dimy()) 
  {
    windowSizeY_    = background_.dimy();
    yoffset_        = (background_.dimy() - editedMapSizeY_) / 2;
  }

  nbDivisionsX_ = 8;
  nbDivisionsY_ = nbDivisionsX_ * editedMapSizeY_ / editedMapSizeX_;
  displayMode_  = ACTION_MANUAL_EDIT_LEFTRIGHT;

  buttons_.setOutput(interfaceImage_);

  initButtons();
  
}

void MorphingEditor::displayGrid(const VectorMap& map, int nbdivx, int nbdivy,
                                 const unsigned char* linecolor)
{
  int sizemapx = map.getSizeX()-1;
  int sizemapy = map.getSizeY()-1;

  float offsetx = float(sizemapx-1) / float(nbdivx);
  float offsety = float(sizemapy-1) / float(nbdivy);

  for   (float fx = 0.0f; fx <= sizemapx; fx += offsetx )
    for (float fy = 0.0f; fy <= sizemapy; fy += offsety )
    {
      int gx = round (fx);
      int gy = round (fy);
      limit(gx, 0, sizemapx-1);
      limit(gy, 0, sizemapy-1);

      Vector basecoord = map.get(gx,gy) / MULTIPLY + Vector(gx,gy);

      //horizontal lines
      if (round( fx + offsetx) < sizemapx)
      {
        int hx = round (fx + offsetx);
        int hy = round (fy);
        limit(hx, 0, sizemapx-1);
        limit(hy, 0, sizemapy-1);

        Vector addcoord = map.get(hx, hy) / MULTIPLY + Vector(hx, hy);

        screen_.drawLine(basecoord.x()/float(sizemapx-1), basecoord.y()/float(sizemapy-1), 
                         addcoord .x()/float(sizemapx-1), addcoord .y()/float(sizemapy-1), 
                         interfaceImage_, linecolor, 1.0f);
      }

      //vertical lines
      if (round (fy + offsety) < sizemapy)
      {
        int hx = round (fx);
        int hy = round (fy + offsety);
        limit(hx, 0, sizemapx-1);
        limit(hy, 0, sizemapy-1);
        Vector addcoord = map.get(hx, hy) / MULTIPLY + Vector(hx, hy);

        screen_.drawLine(basecoord.x()/float(sizemapx-1), basecoord.y()/float(sizemapy-1), 
                         addcoord .x()/float(sizemapx-1), addcoord .y()/float(sizemapy-1), 
                         interfaceImage_, linecolor, 1.0f);
      }
    }
}

// show both morphed left photo and the right photo
void MorphingEditor::drawTwoImagesOnTop(Image& dest,const Image& left,const Image& right,
                                             const VectorMap& map, int coordx, int coordy)
{
  int sizex = left.dimx();
  int sizey = left.dimy();

  Image displacedRightToLeft = PhotoSculptTools::generateMorphedLeft(left, map, false);
  for (int x=0;x<sizex;x++)
    for (int y=0;y<sizey;y++)
    {
      bool odd = bool( (x + y) % 2 );
      Color color;
      if (odd) color=getPixel(displacedRightToLeft,x,y);
      else     color=getPixel(right,x,y);

      if (x+coordx>=0 && x+coordx<dest.dimx() && y+coordy>=0 && y+coordy<dest.dimy() )
        setPixel(dest, x+coordx,y+coordy, color);
    }
}


#include "POV.h"

void MorphingEditor::detectAction()
{
  bool isValidated = false, isEscape = false;

  int action = buttons_.detectUserAction(mouseX_,mouseY_,mouseButton_);

  bool isjustclicked = false;
  static int previousbutton = 0;
  if (mouseButton_ == 1 && previousbutton == 0)
    isjustclicked = true;
  previousbutton = mouseButton_;

  switch (action)
  {
    case ACTION_MANUAL_EDIT_PLUS:
    case ACTION_MANUAL_EDIT_MINUS:
      {
        if (action == ACTION_MANUAL_EDIT_PLUS)  nbDivisionsX_ *= 2;
        if (action == ACTION_MANUAL_EDIT_MINUS) nbDivisionsX_ /= 2;
        limit(nbDivisionsX_,2,32);

        nbDivisionsY_ = nbDivisionsX_ * editedMapSizeY_ / editedMapSizeX_;
        break;
      }
    case ACTION_MANUAL_EDIT_OK:     isOk_ = true; destroy(); break;
    case ACTION_MANUAL_EDIT_CANCEL: destroy();               break;
    case ACTION_REPAINT_SCREEN:     requestRedraw();         break;

    case ACTION_MANUAL_EDIT_CLEAR:    
      {
        int sizex = editedMap_.getSizeX();
        int sizey = editedMap_.getSizeY();

        POV pov; // creates a default point of view
        editedMap_ = pov.generateMapOfFlatObject(sizex,sizey) ;

        //editedMap_.fill(Vector(0,0));
        break;
      }

    case ACTION_MANUAL_EDIT_LEFT:
    case ACTION_MANUAL_EDIT_RIGHT:
    case ACTION_MANUAL_EDIT_LEFTRIGHT: displayMode_ = action; break;
  }

  if (mouseButton_ == 0) // cancels the drag
    isDraging_ = false;

  float mousefx,mousefy;
  bool isinimage= screen_.isCursorInImage(mouseX_, mouseY_, mousefx, mousefy);

  int bestmapx, bestmapy;  
  if (!isDraging_) // if not dragging already, the detect the proximity of the dot
  {
    isDotDetected_ = detectDot(mousefx, mousefy, bestmapx, bestmapy);

    if (isDotDetected_) // then store the dot coordinate to show a circle there
    {
      Vector dotcoord = editedMap_.get(bestmapx,bestmapy) / MULTIPLY + Vector (bestmapx, bestmapy);
      coordDotx_ = float (dotcoord.x()) / float (editedMap_.getSizeX()-1);
      coordDoty_ = float (dotcoord.y()) / float (editedMap_.getSizeY()-1);

      if ( isjustclicked ) // launch a dragging phase
        startDragDot(mousefx, mousefy, bestmapx, bestmapy);
    }
  }
  else // currently dragging with mouse
    dragDot(mousefx,mousefy);
}

void MorphingEditor::startDragDot(float cursorfx, float cursorfy, int bestmapx, int bestmapy)
{
  isDraging_       = true    ;

  mouseDragStartX_ = round ( cursorfx * (editedMap_.getSizeX() - 1) ) ;
  mouseDragStartY_ = round ( cursorfy * (editedMap_.getSizeY() - 1) ) ;
  mapCoordDragX_   = bestmapx;
  mapCoordDragY_   = bestmapy;

  oldDisplacementBeforeDrag_ = editedMap_.get(bestmapx,bestmapy);
}

void MorphingEditor::dragDot(float cursorfx, float cursorfy)
{
  int mousedragcurrentx = round ( cursorfx * (editedMap_.getSizeX() - 1) ) ;
  int mousedragcurrenty = round ( cursorfy * (editedMap_.getSizeY() - 1) ) ;

  // ratio between small map and big map
  float ratio = float (nbDivisionsX_) / float(editedMap_.getSizeX());
  int movex = round( float( mousedragcurrentx-mouseDragStartX_ ) * MULTIPLY * ratio ) ;
  int movey = round( float( mousedragcurrenty-mouseDragStartY_ ) * MULTIPLY * ratio ) ;

  VectorMap smallmap = editedMap_.getResizeAndMultiply(nbDivisionsX_+1, nbDivisionsY_+1);
    
  int smallmapx = mapCoordDragX_ * (nbDivisionsX_+1) / editedMap_.getSizeX();
  int smallmapy = mapCoordDragY_ * (nbDivisionsY_+1) / editedMap_.getSizeY();
  limit (smallmapx, 0, nbDivisionsX_+1);
  limit (smallmapy, 0, nbDivisionsY_+1);

  Vector displacement = smallmap.get(smallmapx,smallmapy);

  int olddispx = round (float (oldDisplacementBeforeDrag_.x()) * float (nbDivisionsX_) / float(editedMap_.getSizeX())) ;
  int olddispy = round (float (oldDisplacementBeforeDrag_.y()) * float (nbDivisionsY_) / float(editedMap_.getSizeY())) ;

  smallmap.set(smallmapx,smallmapy, olddispx + movex, olddispy + movey);

  VectorMap tempmap = smallmap.getResizeAndMultiply(editedMapSizeX_,  editedMapSizeY_);
    
  float offsetx = float(editedMap_.getSizeX()-1) / float(nbDivisionsX_);
  float offsety = float(editedMap_.getSizeY()-1) / float(nbDivisionsY_);

  for   (float fy = mapCoordDragY_ - offsety; fy < mapCoordDragY_ + offsety; fy++)
    for (float fx = mapCoordDragX_ - offsetx; fx < mapCoordDragX_ + offsetx; fx++)
    {
      int ix = round (fx);
      int iy = round (fy);

      if (ix >= 0 && iy >= 0 && ix < editedMap_.getSizeX() && iy < editedMap_.getSizeY())
        editedMap_.set (ix,iy, tempmap.get(ix,iy));
    }
}

bool MorphingEditor::detectDot(float cursorfx, float cursorfy, int& bestx, int& besty)
{
  bool isok = false;
  int sizemapx = editedMap_.getSizeX();
  int sizemapy = editedMap_.getSizeY();

  float offsetx = float(sizemapx-1) / float(nbDivisionsX_);
  float offsety = float(sizemapy-1) / float(nbDivisionsY_);

  int cursormapx = round ( cursorfx * float(sizemapx-1) );
  int cursormapy = round ( cursorfy * float(sizemapy-1) );

  int bestdistance = 100000;

  for   (float fx = 0.0f; fx <= sizemapx; fx += offsetx )
    for (float fy = 0.0f; fy <= sizemapy; fy += offsety )
    {
      int gx = round (fx);
      int gy = round (fy);
      limit(gx, 0, sizemapx-1);
      limit(gy, 0, sizemapy-1);
      Vector pointcoord = editedMap_.get(gx,gy) / MULTIPLY + Vector(gx,gy);

      int distance = square(cursormapx-pointcoord.x())+square(cursormapy-pointcoord.y());

      if (distance < bestdistance && distance < DETECTION_PIXEL_SIZE * DETECTION_PIXEL_SIZE)
      {
        bestdistance = distance;
        bestx = gx; besty = gy;
        isok = true;
      }
    }

  return isok;
}



void MorphingEditor::displayToWindow()
{
  HDC hdc = GetDC(paintZonehWnd_);

  // white
  interfaceImage_.fill(255); 

  const int frame = 100;
  
  int windowsizex=getClientSizeX_() - frame;
  int windowsizey=getClientSizeY_() - frame - buttons_.getSizeY() ;

  int resizedxl,resizedyl;

  Image     leftreduced, rightreduced; 
  VectorMap mapreduced;

  resizedxl = editedLeftImage_.dimx();
  resizedyl = editedLeftImage_.dimy();
  Frame::fit(resizedxl, resizedyl, windowsizex, windowsizey);

  if (resizedxl>0 && resizedyl>0)
  {
    leftreduced  = editedLeftImage_  .get_resize(resizedxl, resizedyl);
    rightreduced = editedRightImage_ .get_resize(resizedxl, resizedyl);
    mapreduced   = editedMap_        .getResizeAndMultiply(resizedxl, resizedyl);
  }

  int xl= (getClientSizeX_() - resizedxl) / 2;
  int yl= (getClientSizeY_() - buttons_.getSizeY() - resizedyl) / 2;

  if (!leftreduced.is_empty())
    interfaceImage_.draw_image(leftreduced, xl, yl);

  screen_.set(xl,yl, resizedxl, resizedyl);

  xoffset_= xl;
  yoffset_= yl;

  switch (displayMode_)
  {
  case ACTION_MANUAL_EDIT_LEFTRIGHT:
    {
      drawTwoImagesOnTop(interfaceImage_,leftreduced,rightreduced,mapreduced, xl,yl);
      break;
    }
  case ACTION_MANUAL_EDIT_LEFT:
    {
      Image displaced = PhotoSculptTools::generateMorphedLeft(editedLeftImage_, editedMap_, 
          false);

      interfaceImage_.draw_image(displaced.get_resize(leftreduced,rightreduced), xl,yl);
      break;
    }
  case ACTION_MANUAL_EDIT_RIGHT:
    {
      interfaceImage_.draw_image(editedRightImage_.get_resize(leftreduced,rightreduced), xl,yl);
      break;
    }
  }

  displayGrid(mapreduced, nbDivisionsX_, nbDivisionsY_, EDIT_MESH_LINE_COLOR);

  if (isDotDetected_)
  {
    int dotx, doty;
    screen_.convertCoordToImage(coordDotx_, coordDoty_, dotx, doty);
    interfaceImage_.draw_circle(dotx, doty, DOT_SIZE_RADIUS_PIXELS, EDIT_MESH_LINE_COLOR, 1.0);
  }

  buttons_.drawBackground();
  buttons_.draw();

  bool isOk = Win32Tools::copyImageToDC(interfaceImage_, hdc);

  ReleaseDC(paintZonehWnd_, hdc);
}

VectorMap MorphingEditor::getResultMap()
{
  if (isOk_)
  {
    int outputLevel   = sculpt_ -> computeLevelUsingSize(MAX_MODEL_SIZE_AFTER_MORPHING_EDITION,
                                                         MAX_MODEL_SIZE_AFTER_MORPHING_EDITION);
    Image tempimage   = sculpt_ -> computeLeftImage(outputLevel);

    int sizex   = tempimage.dimx();
    int sizey   = tempimage.dimy();
    tempimage = Image();
    
    return editedMap_.getResizeAndMultiply(sizex, sizey);
  }
  else       return VectorMap();
}

void MorphingEditor::resizeMainWindow(int newSizeX, int newSizeY)
{
  DialogWindow::resizeMainWindow(newSizeX, newSizeY);

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()    );
}

void MorphingEditor::initButtons()
{
  buttons_ = ButtonManager();
  buttons_.setOutput(interfaceImage_);

  Image image(320,80,1,3);
  image.fill(200);
  buttons_.storeBackground(image);
  // Warning this function does not define the button 'output' field. it will lead to a program crash
  // if member function setOutput is not used

  buttons_.defineNewButton(ACTION_MANUAL_EDIT_PLUS,      20, 20,	60, 60, "+"      );
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_MINUS,     60, 20,	100,60, "-"      ); 
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_LEFT,      100,20,	140,60, "front"  );
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_RIGHT,     140,20,	180,60, "right"  );
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_LEFTRIGHT, 180,20,	220,60, "both"   );
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_CLEAR,     220,20,	260,60, "clear"  );
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_OK,        260,20,	300,60, "OK"     );
  buttons_.defineNewButton(ACTION_MANUAL_EDIT_CANCEL,    300,20,	340,60, "Cancel" );

  buttons_.setPos( (getClientSizeX_()-buttons_.getSizeX())/2 ,
                           getClientSizeY_()-buttons_.getSizeY()     );
}
