/* MorphingEditor.h
 *
 *  Created on: 7 fev. 2011
 *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */
#pragma once

#include "VectorMap.h"
#include "PhotoSculpt.h"
#include "ButtonManager.h"
#include "DialogWindow.h"
#include "MonoScreen.h"

class MorphingEditor: public DialogWindow
{
public:
  MorphingEditor(HINSTANCE hinstance, HWND parenthwnd, PhotoSculpt& sculpt);

  void          detectAction();
  VectorMap     getResultMap();

  void          displayGrid(const VectorMap& map, int nbdivx, int nbdivy,
                            const unsigned char* linecolor);

  static void   drawTwoImagesOnTop(Image& dest,const Image& left,const Image& right,const VectorMap& map,
                          int coordx, int coordy);

  void          initButtons();
  void          close();
  void          resizeMainWindow(int newSizeX, int newSizeY);
  void          displayToWindow();
  bool          detectDot(float cursorfx, float cursorfy, int& bestx, int& besty);
  void          startDragDot(float cursorfx, float cursorfy, int bestmapx, int bestmapy);
  void          dragDot(float cursorfx, float cursorfy);

protected:
  VectorMap     editedMap_;

  PhotoSculpt*  sculpt_;

  int           inputMapLevel_;
  int           inputMapSizeX_,inputMapSizeY_;
  int           editedMapLevel_;

  Image         editedLeftImage_, editedRightImage_;
  int           editedMapSizeX_,  editedMapSizeY_;
  //int           outputMapSizeX_,  outputMapSizeY_;

  //int           skip_;
  int           nbDivisionsX_, nbDivisionsY_;

  Image         background_;
  int           bandSize_;
  int           xoffset_, yoffset_;

  int           windowSizeX_, windowSizeY_;

  Image         initialInterfaceImage_;

  MonoScreen    screen_;

  bool          isOk_;
  ButtonManager buttons_;
  bool          isDotDetected_;
  bool          isDraging_;
  float         coordDotx_, coordDoty_;
  int           mouseDragStartX_, mouseDragStartY_;
  int           mapCoordDragX_, mapCoordDragY_;
  Vector        oldDisplacementBeforeDrag_;

  int           displayMode_;
};

