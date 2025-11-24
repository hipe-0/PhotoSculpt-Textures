#include "ExposureAnalyser.h"
#include "Includes.h"
#include "PhotoSculptTools.h"


ExposureAnalyser::ExposureAnalyser(void)
{
  difference_ = tooDark_ = tooLight_ = contrasted_ = tooMuchShines_ = 
    edgeleftright_ = -1.0f;
}
  
bool ExposureAnalyser::isTooDark           () { return tooDark_        > 0.40f  ; }
bool ExposureAnalyser::isTooLight          () { return tooLight_       > 0.40f  ; }
bool ExposureAnalyser::isTooMuchContrast   () { return contrasted_     > 150.0f ; }
bool ExposureAnalyser::isTooLittleContrast () { return contrasted_     < 50.0f  ; }
bool ExposureAnalyser::isTooShiny          () { return tooMuchShines_  > 0.90f  ; }
bool ExposureAnalyser::isTooDifferent      () { return difference_     > 30.0f  ; }
bool ExposureAnalyser::isLeftBlurred       () { return edgeleftright_  > 0.70f  ; }
bool ExposureAnalyser::isRightBlurred      () { return edgeleftright_  < 0.30f  ; }


int countEdge(const Image& i, float limit)
{
  if (i.dimx() == 0 || i.dimy() == 0)
    THROW_INTERNAL_ERROR("Empty input image (in countEdge())");

  // edges are low values;
  PFloatMap edge = PhotoSculptTools::computeEdgeDetection(i, 30);

  int sx = i.dimx() , sy = i.dimy();
  int countedge = 0 ;
  for (int x = 0 ; x < sx; x++)
    for (int y = 0 ; y < sy; y++)
    {
      float ed = edge->get(x,y);

      if (ed < limit) 
        countedge++;
    }

  return countedge;
}


void ExposureAnalyser::init(const Image& left, const Image& right)
{
  if (left.dimx() == 0 || left.dimy() == 0 || right.dimx() == 0 || right.dimy() == 0)
    THROW_INTERNAL_ERROR("Empty input image (in ExposureAnalyser::init())");

  PHistogram h      = computeHistogram(left);
  PHistogram hright = computeHistogram(right);

  int total         = h      -> computeQuantityBetween(0,255);
  int totalRight    = hright -> computeQuantityBetween(0,255);

  if (total != totalRight)
    THROW_INTERNAL_ERROR("Wrong input parameter, images have different size (in ExposureAnalyser::init())");

  float meanleft    = h  -> computeMeanValue();
  float meanright   = hright -> computeMeanValue();

  difference_ = abs( meanleft - meanright ) ;
  tooDark_    = float( h->computeQuantityBetween(0,50)    ) / float(total);
  tooLight_   = float( h->computeQuantityBetween(205,255) ) / float(total);
  contrasted_ = float(h->computeQuantile(total*9/10) - h->computeQuantile(total/10));

  int   nearHighlights = h->computeQuantityBetween(230,250);
  int   highlights     = h->computeQuantityBetween(250,255);

  if (highlights + nearHighlights != 0)
    tooMuchShines_ = float( highlights ) / float(highlights + nearHighlights);
  else
    tooMuchShines_ = 0;

  //DEBUG_SAVE_IMAGE(*(h->computeImage()), "Histogram.bmp");

  float LIMIT = 2.3f ; int RANGE = 30;
  int edgeleft  = countEdge(left , LIMIT );
  int edgeright = countEdge(right, LIMIT );

  // edges are white = low values
  //DEBUG_SAVE_IMAGE ( ImageGeneration::generateThreshold(
  //  *PhotoSculptTools::computeEdgeDetection(left, RANGE), LIMIT) , "EdgeLeft.bmp" );
  //DEBUG_SAVE_IMAGE ( ImageGeneration::generateThreshold(
  //  *PhotoSculptTools::computeEdgeDetection(right, RANGE), LIMIT) , "EdgeRight.bmp" );

  edgeleftright_ = float ( edgeright) / float ( edgeleft+edgeright );
}



PHistogram ExposureAnalyser::computeHistogram(const Image& i)
{
  PHistogram hist = new Histogram;

  int sx = i.dimx(), sy = i.dimy();

  // reset histogram to zero
  for (int a=0; a < 256; a++) 
    hist->pixelCount_[a] = 0;

  for (int x = 0 ; x < sx ; x++)
    for (int y = 0 ; y < sy ; y++)
    {
      int computevalue = ( int(i(x,y,0,0)) + int(i(x,y,0,1)) + int(i(x,y,0,2)) ) / 3 ;

      if (computevalue<0 || computevalue>255)
        THROW_INTERNAL_ERROR( "Error with input image (in computeHistogram())" );

      hist->pixelCount_[computevalue] ++ ;
    }

  return hist;
}


int Histogram::computeQuantityBetween(int value1, int value2)
{
  if (value1<0 || value1>255) THROW_INTERNAL_ERROR("Wrong 1st parameter (in getQuantityBetween())");
  if (value2<0 || value2>255) THROW_INTERNAL_ERROR("Wrong 2nd parameter (in getQuantityBetween())");

  if (value1 > value2) // then swap 1 and 2
  {
    int tmp = value1;
    value1  = value2;
    value2  = tmp;
  }

  int count = 0;

  for (int a = value1; a <= value2; a++) 
    count += pixelCount_[a];

  return count;
}

float Histogram::computeMeanValue()
{
  int count = 0;
  int total_count= 0;
  
  for (int a = 0; a <= 255; a++) 
  {
    total_count += a * pixelCount_[a];
    count += pixelCount_[a];
  }

  return float(total_count) / float (count);
}

int Histogram::computeQuantile(int quantity)
{
  int count = 0 ;
  
  for (int a = 0 ; a <= 255 ; a++) 
  {
    count += pixelCount_[a];
    if ( count > quantity )
      return a;
  }

  return 255;
}

PImage Histogram::computeImage()
{
  PImage i = new Image(256,256,1,3);
  i->fill(255);

  // compute max height of the histogram bars
  int max = 0;
  for (int a = 0 ; a <= 255 ; a++) 
  {
    int val = pixelCount_[a];
    if ( val > max )
      max = val;
  }

  for (int a = 0 ; a <= 255 ; a++) 
  {
    int height = 255 * pixelCount_[a] / max;
    unsigned char black[3] = {0,0,0};
    i->draw_line(a,255-height, a, 255, black);
  }
  
  return i;
}
