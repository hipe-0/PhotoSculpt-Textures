#include "PerspectiveAnalyser.h"
#include "Limit.h"
#include "Image.h"

PerspectiveAnalyser::PerspectiveAnalyser(void)
{
  offcenter_ = zoomedout_ = zoomedin_ = horizonangle_ = 
    pinchedleft_ = pinchedright_ = pinchedtop_ = pinchedbottom_ = pinch_ = -1.0f;
}

inline float convertdim(float x, float middle, int scale)
{
  return (float(x)-float(middle)) / float(scale) * 2.0f ;
}

bool PerspectiveAnalyser::isTooOffCenter()       { return offcenter_     >  0.20f;   }
bool PerspectiveAnalyser::isTooZoomed   ()       { return zoomedout_     >  0.10f    || 
                                                          zoomedin_      >  0.10f;   }
bool PerspectiveAnalyser::isTooStrongAngle ()    { return horizonangle_  >  17.0f;   }
bool PerspectiveAnalyser::isLeftRightInverted () { return direction_     >= 135.0f  &&
                                                          pinch_         >  0.1f; }
bool PerspectiveAnalyser::isRotate90 ()          { return direction_     >  50.0f    &&
                                                          direction_     <  135.0f   &&
                                                          pinch_         >  0.3f  ;  }
bool PerspectiveAnalyser::isCameraNotLevel ()    { return pinchedtop_    >  0.20f    ||
                                                          pinchedbottom_ >  0.20f;   }
bool PerspectiveAnalyser::isNotEnoughAngle()     { return false;  }
bool PerspectiveAnalyser::isTooMuchAngle()       { return pinchedleft_   >  0.30f;   }

void PerspectiveAnalyser::init(const VectorMap& map)
{
  int sx = map.getSizeX();
  int sy = map.getSizeY();

  int L=sx/4, R=3*sx/4, U=sy/4, D=3*sy/4;
  //int L=0, R=sx-1, U=0, D=sy-1;

  // L = left, R = Right, U = up , D = down
  VectorF map00 ( float( map.x(L,U) ) / MULTIPLY + L , 
                  float( map.y(L,U) ) / MULTIPLY + U );
  VectorF map10 ( float( map.x(R,U) ) / MULTIPLY + R , 
                  float( map.y(R,U) ) / MULTIPLY + U );
  VectorF map01 ( float( map.x(L,D) ) / MULTIPLY + L , 
                  float( map.y(L,D) ) / MULTIPLY + D );
  VectorF map11 ( float( map.x(R,D) ) / MULTIPLY + R , 
                  float( map.y(R,D) ) / MULTIPLY + D );
  
  VectorF mapc (float( map.x((L+R)/2,(U+D)/2) ) / MULTIPLY + (L+R)/2 , 
                float( map.y((L+R)/2,(U+D)/2) ) / MULTIPLY + (U+D)/2 );
  /*
  Image debug(sx,sy,1,3); 
  debug.fill(255);
  unsigned char black[3] = {0,0,0}, red [3] = {255,0,0};
  unsigned char green[3] = {0,255,0}, cyan[3] = {0,255,255};
  unsigned char gray[3] = {200,200,200};
  debug.draw_line (L , U, R , U, gray  ); // top
  debug.draw_line (L , D, R , D, gray  ); // bottom
  debug.draw_line (L , U, L , D, gray  ); // left
  debug.draw_line (R , U, R , D, gray  ); // right
  debug.draw_point((L+R)/2,(U+D)/2, gray  ); // center

  debug.draw_line(round(map00.x()) , round(map00.y()), round(map10.x()) , round(map10.y()), cyan  ); // top
  debug.draw_line(round(map01.x()) , round(map01.y()), round(map11.x()) , round(map11.y()), black ); // bottom
  debug.draw_line(round(map00.x()) , round(map00.y()), round(map01.x()) , round(map01.y()), red   ); // left
  debug.draw_line(round(map10.x()) , round(map10.y()), round(map11.x()) , round(map11.y()), green ); // right
  debug.draw_point(round(mapc.x()) , round(mapc.y()),  black  ); // center

  //DEBUG_SAVE_IMAGE(debug, "PerspectiveAnalyse.bmp");
  */

  VectorF P00 ( convertdim( map00.x() , float((sx)/2), sx-1) ,
                convertdim( map00.y() , float((sy)/2), sx-1) );
  VectorF P10 ( convertdim( map10.x() , float((sx)/2), sx-1) ,
                convertdim( map10.y() , float((sy)/2), sx-1) );
  VectorF P01 ( convertdim( map01.x() , float((sx)/2), sx-1) ,
                convertdim( map01.y() , float((sy)/2), sx-1) );
  VectorF P11 ( convertdim( map11.x() , float((sx)/2), sx-1) ,
                convertdim( map11.y() , float((sy)/2), sx-1) );

  VectorF C   ( convertdim( mapc.x()  , float((sx)/2), sx-1),
                convertdim( mapc.y()  , float((sy)/2), sx-1) );

  offcenter_ = C.getLength();

  float leftimagewidth  = 1.0f ;
  float leftimageheight = 1.0f * sy / sx;
  float leftimagemeansize = (leftimagewidth + leftimageheight) / 2 ;

  float heightleft  = (P01 - P00).getLength() ; 
  float heightright = (P11 - P10).getLength() ; 
  float widthtop    = (P10 - P00).getLength() ; 
  float widthbottom = (P01 - P11).getLength() ; 

  float meanheight = (heightleft + heightright ) / 2;
  float meanwidth = (widthtop + widthbottom ) / 2;
  float meansize = (meanheight + meanwidth) / 2 ;


  float zoom = meansize / leftimagemeansize;
  if (zoom>1.0f)
  {
    zoomedin_  = zoom - 1.0f;
    zoomedout_ = 0.0f;
  }
  else 
  {
    zoomedout_  = 1.0f / zoom - 1.0f;
    zoomedin_   = 0.0f;
  }

  float pinchv = heightright / heightleft;
  if (pinchv>1.0f)
  {
    pinchedleft_  = pinchv - 1.0f;
    pinchedright_ = 0.0f;
  }
  else 
  {
    pinchedright_  = 1.0f / pinchv - 1.0f;
    pinchedleft_   = 0.0f;
  }

  float pinchh = widthtop / widthbottom;
  if (pinchh>1.0f)
  {
    pinchedbottom_  = pinchh - 1.0f;
    pinchedtop_     = 0.0f;
  }
  else 
  {
    pinchedtop_    = 1.0f / pinchh - 1.0f; 
    pinchedbottom_ = 0.0f;
  }

  VectorF directionV(pinchedleft_ - pinchedright_ , pinchedbottom_ - pinchedtop_);
  direction_ = abs(directionV.getAzimuth());
  pinch_ = directionV.getLength();


  VectorF centerleft  = (P01 + P00) / 2;
  VectorF centerright = (P10 + P11) / 2;
  float angle = (centerright - centerleft).getAzimuth();
  horizonangle_ = abs(angle);

  limit( offcenter_ ,    0.0f,1.0f);
  limit( zoomedout_,     0.0f,1.0f);
  limit( zoomedin_,      0.0f,1.0f);
  limit( pinchedleft_,   0.0f,1.0f);
  limit( pinchedright_,  0.0f,1.0f);
  limit( pinchedtop_,    0.0f,1.0f);
  limit( pinchedbottom_, 0.0f,1.0f);
}