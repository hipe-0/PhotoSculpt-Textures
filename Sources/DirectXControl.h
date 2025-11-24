// 80 char per line standard :
/////////_/////////_/////////_/////////_/////////_/////////_/////////_/////////_
#pragma once


// include the basic windows header files and the Direct3D header file
#include <windows.h>
#include <windowsx.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <Commctrl.h>

// include the Direct3D Library files
#pragma comment (lib, "d3d9.lib")
#pragma comment (lib, "d3dx9.lib")

#include "Vector3.h"
#include "View3D.h"
#include "Obfuscate.h"
#include "Mutex.h"
#include "Mesh.h"
#include "FloatMap.h"

struct CUSTOMVERTEX 
{
  FLOAT X, Y, Z; 
  D3DVECTOR NORMAL; 
  FLOAT U, V;
};

#define CUSTOMFVF (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)

class DirectXControl
{
public:
  DirectXControl            ()              ;

  // function prototypes
  bool initD3D              (HWND hWnd, int sizex, int sizey, PFloatMap& inputmodel, 
                             PImage& inputtexture_, PImage& alpha, int shader,
                             bool isrepeat, float focallengthinmm);

  void changeShaderOnly(PImage& inputtexture, int shader, bool isrepeat);

  void render3DImageFrame   ()              ;
  void cleanD3D             ()              ;
  void setCamera            (float camerarx, float camerary, float camerarz, 
                             float cameratx, float cameraty, float cameratz, 
                             float camerascale);
  void forceDrawOnNextRender()              ;
  bool isOn                 ()              ;
  bool isRedrawNeeded       () const        ;
  int  getTriangleDisplayed ()              ;

  void setMaxTriangle       (int i)         ;

  void setSmooth            (bool value)    ;
  bool getSmooth            ()              ;
  void setLuminosity        (int luminosity);

  static bool testLoadDxDll();

// private functions
private:

  // creates 3d model ready for display
  void                 init_graphics             (const Mesh& model, const Image& image);
  void                 loadTexture_              (const Image& image);
  void                 initVertex_               (const Mesh& model, const Image& alpha);
  void                 initVertex_               (const FloatMap &model);
  void                 initIndex_                ();
  void                 initLight                 (float factor);

  static inline void   setCustomVertex(CUSTOMVERTEX& c, float x, float y, float z);
  static inline void   setNormal      (CUSTOMVERTEX& c, D3DVECTOR vect);
  static inline void   setTextureCoord(CUSTOMVERTEX& c, float u, float v);

  static inline D3DVECTOR convert  (Vector3 v);

  static CUSTOMVERTEX* getnewVerticesFromMap     (const Mesh &model, int& returnsize,
                                                  bool issmooth, const Image& alpha);
  static short *       getnewIndiceLineFromMap   (int sizex,         int& returnsize);

  void                 resizeTextureToHardWareCapabilities_ ( PImage inputtexture  , int div);
  void                 restrictModelSize_        ( PFloatMap model );
// private variables
private:

  LPDIRECT3D9             d3d_      ;
  LPDIRECT3DDEVICE9       d3ddev_   ;

  // buffers : they contain the 3d model description
  LPDIRECT3DVERTEXBUFFER9 vertexBufferTable_ ;
  LPDIRECT3DINDEXBUFFER9  indexBufferTable_  ;
  int                     modelSizeX_, modelSizeY_  ;

  LPDIRECT3DTEXTURE9      texture_  ;    // our only texture
  HWND                    hwnd_     ;

  int                     DXSizeX_, DXSizeY_              ;
  ofloat                  cameraRX_, cameraRY_, cameraRZ_ ; // rotations
  ofloat                  cameraTX_, cameraTY_, cameraTZ_ ; // translations
  ofloat                  cameraScale_                    ;

  static  HINSTANCE hInst_          ;
  obool   isForceRedraw_            ;

  Mutex   mutex_                    ;
  oint    triangleDisplayed_        ;
  oint    maxtriangle_              ;
  obool   isCompact_                ;
  obool   isSmooth_                 ;
  oint    luminosity_               ;
};



inline D3DVECTOR DirectXControl::convert(Vector3 v)
{
  D3DVECTOR ret;
  ret.x=v.x;
  ret.y=v.y;
  ret.z=v.z;

  return ret;
}

inline void DirectXControl::setNormal(CUSTOMVERTEX& c, D3DVECTOR vect)
{
  c.NORMAL = vect;
}

inline void DirectXControl::setTextureCoord(CUSTOMVERTEX& c, float u, float v)
{
  c.U=u;
  c.V=v;
}

inline void DirectXControl::setCustomVertex(CUSTOMVERTEX& c, float x, float y, float z)
{
  c.X=x;
  c.Y=y;
  c.Z=z;
  D3DVECTOR vect={-1,0,0};
  c.NORMAL = vect;
  c.U=0.0;
  c.V=0.0;
}
