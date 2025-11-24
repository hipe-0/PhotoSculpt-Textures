#include "Result.h"
#include "Seamless.h"
#include "RESOURCE.H"
#include "View3D.h"
#include "Cropper.h"
#include "CropEditor.h"
#include "Includes.h"
#include "Exposure.h"
#include "PhotoSculptTools.h"
#include "Convert.h"
#include "ImageTools.h"
#include "Limit.h"
#include "Options.h"

Result::Result()
{
  clear();
  alpha_          = new Image();
}

void Result::clear()
{
  MUTEX;
  clear_();
  MUTEXRELEASE;
}

void Result::clear_()
{
  azimuth_        = -10.0f ;
  zenith_         = 10.0f  ;
  roll_           = 0.0f   ;
  zScale_         = 3.0f   ;
  xTrans_         = yTrans_ = zTrans_ = 0.0f ;
  modelScale_     = 1.0f   ;
  subdivision_    = 1      ;

  shader_         = ACTION_3D_TEXTURE;
  originalMap_    = workMap_  = borderedMap_ = seamlessUncropedMap_ =
    finalMap_     = IntMap();

  frontPhoto_ = finalImage_ = Image();

  isFlatMap_      = isInvertNormalX_ = isInvertNormalY_ = false;
  
  isAnimation_    = false; // turns off the turntable animation if it was on
  bestPOV_        = POV();
  cropper_        = Cropper();
  modeNb_         = ACTION_MODE_SOURCES;
  cropper_.cancel();
}

bool Result::isResult()
{
  MUTEX;

  bool tmp  = ! finalImage_.is_empty();

  MUTEXRELEASE;
  return tmp;
}

const Image Result::getImage()
{
  MUTEX;

  Image tmp  = finalImage_;

  if (finalImage_.dimv() != 3) // should be 3 color channels
    tmp.resize(finalImage_.dimx(), finalImage_.dimy(),  1, 3);

  MUTEXRELEASE;
  return tmp;
}

const IntMap Result::getMap()
{
  MUTEX;

  IntMap tmp  = finalMap_;

  MUTEXRELEASE;
  return tmp;
}

PFloatMap Result::getFloatMap()
{
  MUTEX;

  float scale = (zScale_ / MULTIPLY) / float(subdivision_);
  PFloatMap tmp = new FloatMap(convertToFloat( finalMap_ ) * scale) ;

  MUTEXRELEASE;
  return tmp;
}

void Result::setFlatMode()
{
  MUTEX;

  isFlatMap_  = true;
  cropper_.setSeamless(false);

  MUTEXRELEASE;
}

void Result::setSeamlessMode()
{
  MUTEX;

  cropper_.setSeamless(true);
  cropper_.setSquareRectify(true, getOriginalSizeX(), getOriginalSizeY());

  isFlatMap_  = false;

  MUTEXRELEASE;
}
void Result::cancelMode()
{
  MUTEX;

  isFlatMap_  = false;
  cropper_.setSeamless(false);

  MUTEXRELEASE;
}

void Result::updateAll(const IntMap& map, const Image& image, int mode, int shadercode)
{
  DEBUG(Reuupatll);
  // Warning : this function is thread protected with a mutex, 
  // use   MUTEXRELEASE; before escaping with a "throw" or return
  MUTEX;
  DEBUG_WRITE("  Repdelthrease");
 
  isAnimation_ = false; // turns off the turntable animation if it was on

  if (mode < ACTION_MODE_FIRST || mode > ACTION_MODE_LAST)
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR("Initialisation error : don't know this mode (in updateAll)");
  }
  if (shadercode != ACTION_3D_TEXTURE && shadercode != ACTION_3D_WHITE && shadercode != ACTION_3D_WIREFRAME)
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR("Initialisation error : don't know this shader code (in updateAll)");
  }
 
  // clear all maps and images before starting update
  originalMap_ = workMap_  = borderedMap_ =
    seamlessUncropedMap_ = finalMap_ = IntMap();
  frontPhoto_ = finalImage_ = Image();

  if (map.isEmpty())
  {
    MUTEXRELEASE;
    return;
  }
 
  // uses d to compute the maps (seamlessMap_, flatMap_, workMap_)
  workMap_ = setMapAndComputeSeamless_(map);

  // Set base image frontPhoto_, either full version or croped version
  if (!cropper_.isCrop())       frontPhoto_ = image ;
  else                          frontPhoto_ = cropper_.crop(image);

  modeNb_ =                     mode;        // Set mode
  shader_ =                     shadercode;  // ACTION_3D_TEXTURE, ACTION_3D_WHITE, ACTION_3D_WIREFRAME

  // computes all images tiledTexture_, baseTexture_
  // using frontPhoto_ and workMap_
  finalImage_ = computeTexture_();
  finalMap_ = workMap_  ; 
  workMap_ = IntMap();

  // eventually reduces both image and map to a power of 2
  if (cropper_.is2N())
  {
    finalMap_   = Cropper::reduce2N ( finalMap_   );
    finalImage_ = Cropper::reduce2N ( finalImage_ );
  }

  // end of process: clear all work data
  workMap_= borderedMap_= seamlessUncropedMap_= IntMap(); // free memory 260 Mb
  frontPhoto_ = Image(); // free memory 90 Mb

  originalMap_  = map;  // saves the original untouched map into originalMap_

  MUTEXRELEASE;
}

void Result::updateShaderOnly(int shadercode)
{
  // Warning : this function is thread protected with a mutex, 
  // use   MUTEXRELEASE; before escaping with a "throw" or return
  MUTEX;

  if (shadercode != ACTION_3D_TEXTURE && shadercode != ACTION_3D_WHITE && shadercode != ACTION_3D_WIREFRAME)
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR("Initialisation error : don't know this shader code (in updateAll)");
  }
 
  // Set Shader (ACTION_3D_TEXTURE, ACTION_3D_WHITE, ACTION_3D_WIREFRAME)
  shader_ = shadercode;

  MUTEXRELEASE;
}

IntMap Result::setMapAndComputeSeamless_(const IntMap& map)
{
  // Warning : this function is thread protected with a mutex, 
  // use   MUTEXRELEASE; before escaping with a "throw"
  IntMap workmap = map;

  if (!map.isEmpty())
  {
    int sx = map.getSizeX()/subdivision_ , sy = map.getSizeY()/subdivision_ ;
    if (sx<2) sx = 2; if (sy<2) sy = 2;

    IntMap newmap = map.getResize(sx,sy);

    if (!cropper_.isCrop())
      workmap = newmap;
    else
      workmap = cropper_.crop(newmap);

    if (!workmap.isEmpty())
    {
      if (workmap.getSizeX()<2 || workmap.getSizeY()<2)
         workmap = (IntMap(2,2).fill(0));  
    }
    else
      workmap = (IntMap(2,2).fill(0));

    if (!newmap.isEmpty())
      workmap = computeSeamlessMap_(workmap);
  }

  return workmap;
}

// updates the seamless+bordered+uncroped+flat map based on workMap_
IntMap Result::computeSeamlessMap_(const IntMap &map)
{
  IntMap workmap = map;
  // Warning : this function is thread protected with a mutex, 
  // use   MUTEXRELEASE; before escaping with a "throw" 
  if (isFlatMap_)
    workmap = Seamless::computeFlattenMap(workmap, GlobalOptions.nbFlatModeSamples_);

  if (cropper_.isSeamless())
  {
    int cropx = round(float(workmap.getSizeX()) * cropper_.getFrameSize());
    int cropy = round(float(workmap.getSizeY()) * cropper_.getFrameSize());

    borderedMap_ = Seamless::computeBorderedMap(workmap, 
             cropx, cropy , zScale_ /float(subdivision_) );

    seamlessUncropedMap_ = Seamless::computeSeamlessMapFormBorderedUncroped(borderedMap_,
      cropx , cropy);

    workmap = Seamless::cropTextureMap(seamlessUncropedMap_, cropx, cropy);
  }

  return workmap;
}

Image repeatNInvertOdd(const Image& texture, int nb, int framesize)
{
  DEBUG(rpeaNIvtdd);
  if (nb <= 0) THROW_INTERNAL_ERROR("wrong argument in repeatNInvertOdd()");

  Image invertedtexture = 255 - texture;
      
  //DEBUG_SAVE_IMAGE(texture,         "test texture.jpg"); 
  //DEBUG_SAVE_IMAGE(invertedtexture, "test invertedtexture.jpg"); 

  Image result (texture.dimx()*nb+framesize*2, texture.dimy()*nb+framesize*2, 1, 3);
  int start = 0;
  if (framesize>0) start = 1;

  for   (int y=-start; y<nb+start; y++)
    for (int x=-start; x<nb+start; x++)
    {
      if ( (x+2+y+2)%2 == 0 )
        result.draw_image(texture,        texture.dimx()*x+framesize, texture.dimy()*y+framesize);
      else
        result.draw_image(invertedtexture,texture.dimx()*x+framesize, texture.dimy()*y+framesize);
      
      //DEBUG_SAVE_IMAGE(result, "test repeatNInvertOdd.jpg"); 
    }

  return result;
}

Image blurMultiple(Image & image, float radius)
{
  DEBUG(blrMule);

  Image temp, result;

  result = image.get_blur(radius);
 
  temp = image.get_blur(radius / 3.0f);
  result.draw_image(temp,  0,0,0,0, 0.50f);

  temp = image.get_blur(radius / 9.0f);
  result.draw_image(temp,  0,0,0,0, 0.50f);

  if (radius / 18.0f > 0.5f)
  {  
    temp = image.get_blur(radius / 18.0f);
    result.draw_image(temp,  0,0,0,0, 0.25f);
  }

  if (radius / 36.0f > 0.5f)
  {
    temp = image.get_blur(radius / 36.0f);
    result.draw_image(temp,  0,0,0,0, 0.10f);
  }

  result.draw_image(image, 0,0,0,0, 0.10f);

  return result;
}

Image fadeSeamless(const Image& image, const Image& mask)
{
  DEBUG(fdeames);

  int sizex = image.dimx();
  int sizey = image.dimy();
  float radius = float (sizex+sizey) / 60.0f;

  limit (radius, 1.0f, 50.0f);

  int radint = round(radius);

  //DEBUG_VALUE("  radius for fadeseamless", radius);
  //DEBUG_SAVE_IMAGE(image, "fadeSeamless-inputImage.jpg");
  //DEBUG_SAVE_IMAGE(mask,  "fadeSeamless-intputmask.jpg" );

  Image repeatseamless = image;
  ImageTools::repeatN(repeatseamless, 1, radint, radint);

  Image repeatseamlessblur = blurMultiple(repeatseamless, radius); // generates +200 mb memory peak at 905Mb
  //DEBUG_SAVE_IMAGE(repeatseamless,     "fadeSeamless-seamless.jpg");
  //DEBUG_SAVE_IMAGE(repeatseamlessblur, "fadeSeamless-seamlessblur.jpg");

  Image maskseamless = repeatNInvertOdd(mask, 1, radint); // +20 mb, 3 channels
  Image invertmaskseamless = 255-maskseamless; // + 20 mb
  //DEBUG_SAVE_IMAGE(maskseamless,       "fadeSeamless-maskseamless.jpg");
  //DEBUG_SAVE_IMAGE(invertmaskseamless, "fadeSeamless-invertmaskseamless.jpg");

  Image even = Image(sizex+radint*2, sizey+radint*2, 1,3); even.fill(128); // gray 20 mb each
  Image odd  = Image(sizex+radint*2, sizey+radint*2, 1,3); odd.fill(128); // gray
  Image gray = Image(sizex+radint*2, sizey+radint*2, 1,3); gray.fill(128); // gray
  even.draw_image(repeatseamless, maskseamless, 0,0,0,0,255,1.0f);
  //DEBUG_SAVE_IMAGE(even, "fadeSeamless-even.jpg");
  odd.draw_image (repeatseamless, invertmaskseamless, 0,0,0,0,255,1.0f);
  //DEBUG_SAVE_IMAGE(odd,  "fadeSeamless-odd.jpg");

  Image evenblur = blurMultiple(even, radius); // peak at 946 mb
  //DEBUG_SAVE_IMAGE(evenblur, "fadeSeamless-evenblur.jpg");
  Image oddblur  = blurMultiple(odd , radius); // peak at 966 mb
  //DEBUG_SAVE_IMAGE(oddblur,  "fadeSeamless-oddblur.jpg");

  Image evenblurrepaint = evenblur.draw_image(gray, invertmaskseamless, 0,0,0,0,255,1.0f); // repaint the gray area as before blur
  //DEBUG_SAVE_IMAGE(evenblurrepaint, "fadeSeamless-evenblurrepaint.jpg");
  Image oddblurrepaint = oddblur. draw_image(gray, maskseamless, 0,0,0,0,255,1.0f);
  //DEBUG_SAVE_IMAGE(oddblurrepaint,  "fadeSeamless-oddblurrepaint.jpg");
  gray = maskseamless = invertmaskseamless = Image(); // free memory

  Image invertedevenblurrepaint = 0 - evenblurrepaint; // invert
  Image invertedoddblurrepaint  = 0 - oddblurrepaint;
  evenblurrepaint = oddblurrepaint = Image(); // free memory

  Image result = invertedevenblurrepaint;
  //DEBUG_SAVE_IMAGE(result, "fadeSeamless-result1.jpg");
  result = ImageTools::mixOverlay( result, invertedoddblurrepaint );
  //DEBUG_SAVE_IMAGE(result, "fadeSeamless-result2.jpg");
  result.draw_image(repeatseamlessblur,0,0,0,0, 0.50f);
  //DEBUG_SAVE_IMAGE(result, "fadeSeamless-result3.jpg");

  result = result.get_crop(radint, radint, radint+sizex-1, radint+sizey-1);
  Image finalresult = ImageTools::mixHardLight(image, result);
  finalresult       = ImageTools::mixHardLight(finalresult, result);
  finalresult       = ImageTools::mixHardLight(finalresult, result);
  //DEBUG_SAVE_IMAGE(finalresult, "fadeSeamless-resultfinal.jpg");
  //DEBUG_SAVE_IMAGE(image,       "fadeSeamless-resultfinalcompare.jpg");

  return finalresult; // 170 mb released
}

// computes and fills baseTexture_ and tiledTexture_
Image Result::computeTexture_()
{
  // Warning : this function is thread protected with a mutex, 
  // use   MUTEXRELEASE; before escaping with a "throw" 
  if (workMap_.isEmpty())
    {
      MUTEXRELEASE;
      THROW_INTERNAL_ERROR("Initialisation error(workMap_ is empty in computeTexture())");
    }

  Image texture;
  
  if (!cropper_.isSeamless())
    texture = computeBaseTexture_(modeNb_, workMap_, frontPhoto_);
  else
    texture = computeBaseTexture_(modeNb_, seamlessUncropedMap_, frontPhoto_);

  // calculates now imageSeamless
  if (isSeamless())
  {
    if (borderedMap_.isEmpty())
    {
      MUTEXRELEASE;
      THROW_INTERNAL_ERROR("Initialisation error(borderedMap_ is empty in computeTexture())");
    }

    if (modeNb_ == ACTION_MODE_TILE_MASK)
    {
      if (cropper_.isSeamless())
        texture =  Seamless::computeSeamlessMaskFromBordered(
          borderedMap_,
          int(round(float(texture.dimx()) * cropper_.getFrameSize())),
          int(round(float(texture.dimy()) * cropper_.getFrameSize())));
    }
    else 
    {
      if (cropper_.isSeamless())
      {
        texture =  Seamless::computeSeamlessImageFromBordered(
          borderedMap_, texture,
          round(float(texture.dimx()) * cropper_.getFrameSize()),
          round(float(texture.dimy()) * cropper_.getFrameSize()));

        if (modeNb_ != ACTION_MODE_SOURCES && 
            modeNb_ != ACTION_MODE_ALPHA   && 
            GlobalOptions.isSeamCorrectedDiffuse_) // additional processing
        {
          Image mask =  Seamless::computeSeamlessMaskFromBordered(
            borderedMap_,
            round(float(borderedMap_.getSizeX()) * cropper_.getFrameSize()),
            round(float(borderedMap_.getSizeY()) * cropper_.getFrameSize()));

          texture = fadeSeamless(texture, mask); // very slow > 60s
        }
      }
    }
  }

  // now that the texture is prepared, replace it by the 3d software view
  if (modeNb_ == ACTION_MODE_OLD_3D)
  {
    if (cropper_.isSeamless())
    {
      int framesizex = (texture.dimx()) * REPEATED_SEAM_PERCENT / 100 ;
      int framesizey = (texture.dimy()) * REPEATED_SEAM_PERCENT / 100 ;

      IntMap    ::repeatN(workMap_, 1, framesizex, framesizey);
      ImageTools::repeatN(texture,  1, framesizex, framesizey);
    }

    texture = View3D::generate3DImage(workMap_, texture, azimuth_,
                                    zenith_, zScale_/MULTIPLY/float(subdivision_));
  }

  return texture;
}

// Warning : computeSeamlessMap_ should be launched prior to any function
//           in order to use workMap_, flatMap_, borderedMap_, seamlessMap_ ...
//
//Image Result::computeBaseTexture_(int mode, bool isflat, bool istiled, 
//                                  const IntMap& workmap, const IntMap& flatmap, const IntMap& tiledmap) // should be const, but the mutex won't compile
Image Result::computeBaseTexture_(int mode, const IntMap& workmap, const Image& frontphoto) // should be const, but the mutex won't compile
{
  // Warning : this function is thread protected with a mutex, 
  // use   MUTEXRELEASE; before escaping with a "throw" 

  Image returned;
  
  if (workmap.isEmpty())
  {
    MUTEXRELEASE;
    THROW_INTERNAL_ERROR("Initialisation error (workMap_ is empty in computeBaseTexture_())");
  }
  
  if (frontphoto.is_empty())
  {
    frontPhoto_ = Image(1,1,1,3);
    frontPhoto_.fill (255);
  }

  switch(mode)
  {
  case ACTION_MODE_DISPLACE :
    returned = ImageGeneration::generateDepth(workmap);
    break;  
  case ACTION_MODE_BUMP :
    returned = ImageGeneration::generateBump(workmap);
    break;  
  case ACTION_MODE_LIMDISP :
    returned = ImageGeneration::generateDisplace(workmap);
    break;  
  case ACTION_MODE_NORMAL :
    returned = View3D::generateNormalMapImage(workmap, 
               zScale_/MULTIPLY/float(subdivision_), isInvertNormalX_ , isInvertNormalY_);
    break;  
  case ACTION_MODE_AMBOCC :
    returned = ImageGeneration::generateOcclusion(workmap,30,
      zScale_/float(subdivision_)); 

    break;
  case ACTION_MODE_SPECULAR : 
    returned = ImageGeneration::generateSpecular(workmap,frontPhoto_,30,
      zScale_/float(subdivision_)); 

    break;
  case ACTION_MODE_3D      :  case ACTION_MODE_DIFFUSE :
  case ACTION_MODE_SOURCES :
    returned = frontPhoto_.get_resize( workmap.getSizeX()  ,  workmap.getSizeY()  );
    Exposure::autoLevel(returned);

    break;
  case ACTION_MODE_TILE_MASK : // white page
    returned = Image( workmap.getSizeX(), workmap.getSizeY() , 1, 3);
    returned.fill(255);

    break;
  case ACTION_MODE_ALPHA :
    {
      if (!alpha_->is_empty())
      {
        returned = *alpha_;//->get_resize(frontPhoto_.dimx(),frontPhoto_.dimy());

        if (cropper_.isCrop())
          returned = cropper_.crop(returned);
      }
      else
        returned = Image( frontphoto.dimx(),frontphoto.dimy(),1,3). fill(255);

      returned = returned.get_resize(workmap.getSizeX(), workmap.getSizeY(), 1,3,3);

      returned = ImageTools::threshold(returned, 128);

      break;
    }
  case ACTION_MODE_OLD_3D :
    {
      Image texture;
      
      if (shader_ == ACTION_3D_TEXTURE)
        texture = frontPhoto_.get_resize( workmap.getSizeX(), workmap.getSizeY()  );
      else
        texture = Image( workmap.getSizeX(), workmap.getSizeY(), 1, 3) . fill( 255 ) ;

      returned = texture;
      break;    
    }    
  }

  return returned;
}

bool Result::isFlat() const
{
  return isFlatMap_;
}
bool Result::isSeamless() const
{
  return cropper_.isSeamless();
}

int  Result::getMode() const
{
  return modeNb_;
}
int  Result::getShader() const
{
  return shader_;
}

IntMap Result::getOriginalMap()
{
  MUTEX;

  IntMap tmp = originalMap_;

  MUTEXRELEASE;

  return tmp;
}

int Result::getOriginalSizeX()
{
  MUTEX;
  int tmp = originalMap_.getSizeX();
  MUTEXRELEASE;
  return tmp;
}

int Result::getOriginalSizeY()
{
  MUTEX;
  int tmp = originalMap_.getSizeY();
  MUTEXRELEASE;
  return tmp;
}

int Result::getModelSizeX()
{
  MUTEX;
  int tmp = finalMap_.getSizeX();
  MUTEXRELEASE;
  return tmp;
}
int Result::getModelSizeY()
{
  MUTEX;
  int tmp = finalMap_.getSizeY();
  MUTEXRELEASE;
  return tmp;
}


////////////////////////
// CROPPER CONTROL

void Result::defineCropArea(float x1, float y1, float x2, float y2)
{
  MUTEX;
  cropper_.defineArea( x1 , y1 , x2 , y2 );
  MUTEXRELEASE;
}

void Result::cancelCropArea()
{
  MUTEX;
  cropper_.cancel();
  MUTEXRELEASE;
}

bool Result::isCrop() const
{
  return cropper_.isCrop();
}

bool Result::isSquare  () const
{
  return cropper_.isSquare();
}

Cropper Result::getCropper()
{
  MUTEX;
  Cropper c = cropper_ ;
  MUTEXRELEASE;
  return c;
}

Cropper* Result::getCropperPointer()
{
  MUTEX;
  Cropper* c = &cropper_ ;
  MUTEXRELEASE;
  return c;
}

void Result::setCropper(Cropper c)
{
  MUTEX;
  cropper_ = c ;
  MUTEXRELEASE;
}

void Result::startAnimation()
{
  isAnimation_ = !isAnimation_ ;
}

void Result::setAlpha(PImage alpha)
{
  alpha_ = alpha ;
}
