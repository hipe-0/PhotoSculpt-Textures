#pragma once
#include "Image.h"

class MonoScreen
{
public:
  MonoScreen();
  bool isCursorInImage(int x, int y, float& coordx, float& coordy);
  void convertCoordToImage(float coordx, float coordy, int& x, int& y);

  void drawPoint (float coordx, float coordy,   Image& i, unsigned char color[3]);
  void drawLine  (float coordx1, float coordy1, 
                  float coordx2, float coordy2, Image& i, const unsigned char color[3], float opacity);
  void drawFrame (float coordx1, float coordy1, 
                  float coordx2, float coordy2, Image& i, unsigned char color[3], float opacity);
  void drawFrame (float coordx1, float coordy1, 
                  float coordx2, float coordy2,
                  float coordx3, float coordy3, 
                  float coordx4, float coordy4, Image& i, unsigned char color[3], float opacity);
  void drawDot   (float coordx1, float coordy1,Image& i, unsigned char color[3], float opacity);

  void set(int xl, int yl, int sxl, int syl);
  int getSizeX();
  int getSizeY();
  int getPosX();
  int getPosY();
  int getDistance(float coordx1, float coordy1, float coordx2, float coordy2);

  static int   FtoI_ ( float valuefloat , int size);
  static float ItoF_ ( int valueint , int size);

private:
  int screenPosX_, screenPosY_;
  int screenSizeX_, screenSizeY_;
};

