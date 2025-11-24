// basic file operations
#include "ExportObj.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "Mesh.h"
#include "Convert.h"
#include "Includes.h"
#include "Options.h"
#include "File.h"

const char*  PROGRAM_NAME         = ">Photosculpt Textures<" ;

bool ExportObj::exportAllObjFile (char* fileName, const Image& leftImage, const Image& alpha,
                                  const IntMap& map, 
                                  int skip, float xyScale, float zScale, int jpgquality,
                                  float focallengthinmm)
{
  DEBUG(exportAll);
  char nameWithoutExt[200]={0};
  strcpy(nameWithoutExt, fileName);

  char* pch=strrchr(nameWithoutExt,'.');
  // writes an end of string \0 char instead of a '.'
  if (pch!=0)
    *pch=0;

  // Constructs the image file name, ended with .bmp
  char imageFileName[200]={0};
  strcat(imageFileName, nameWithoutExt);
  strcat(imageFileName, ".jpg");

  // Constructs the image material name, ended with .mtl
  char materialName[200]={0};
  strcat(materialName, nameWithoutExt);
  strcat(materialName, ".mtl");


  // Export OBJ file first
  bool isSuccessObj=exportObjFileOnly(fileName, materialName, map, leftImage, 
                    alpha, skip, xyScale, zScale, focallengthinmm);

  // Export Mtl file
  bool isSuccessMtl=exportMtlFileOnly(materialName, imageFileName);

  // Export
  bool isSuccessImg=leftImage.save_jpeg(imageFileName, jpgquality);

  return (isSuccessObj && isSuccessMtl && isSuccessImg);
}

/*
# Blender3D MTL File: 2009.04.10-3dMix mur 4.blend
# Material Count: 1
newmtl Material_DSC03401_r.JPG
Ns 384.313725
Ka 0.000000 0.000000 0.000000
Kd 0.800000 0.800000 0.800000
Ks 0.113944 0.113944 0.113944
Ni 1.000000
d 1.000000
illum 2
map_Kd DSC03401 r.JPG
 */

bool ExportObj::exportMtlFileOnly (char* fileName, char* imageFileName)
{
  DEBUG(exportMt);

  std::ofstream myfile;

  printf("file : %s\n",fileName);

  // Open the file to export to.
  myfile.open (fileName);
  if (!myfile.is_open()) 
    return false;

  myfile << "# Exported with " << PROGRAM_NAME << "\n" ;

  myfile << "newmtl material\n";

  // Some parameters (shading)
  myfile << "Ns 384.313725\n";
  myfile << "Ka 0.000000 0.000000 0.000000\n";
  myfile << "Kd 0.800000 0.800000 0.800000\n";
  myfile << "Ks 0.113944 0.113944 0.113944\n";
  myfile << "Ni 1.000000\n";
  myfile << "d 1.000000\n";
  myfile << "illum 2\n";

  // extracts only the file name, no path.
  char * fileNameNoPath = pointToFileNameOnly(imageFileName);

  // Image file name
  myfile << "map_Kd " << fileNameNoPath << "\n";

  myfile.close();
  return true;
}


/*
mtllib 2009.04.10-3dMix mur 4.mtl
o Cube
v 3.163885 1.928659 3.105435
v 3.161165 1.923046 3.117526
v 3.159695 1.917400 3.123849
v 3.158929 1.911737 3.126924
vt 0.916471 0.080277 0.0
vt 0.913797 0.080277 0.0
vt 0.913797 0.077612 0.0
vt 0.916471 0.077612 0.0
usemtl Material_DSC03401_r.JPG
s 1
f 1/1 47/2 62/3 2/4
f 2/5 62/6 63/7 3/8
f 3/9 63/10 64/11 4/12
f 4/13 64/14 65/15 5/16
f 5/17 65/18 66/19 6/20
 */
#include "POV.h"
extern POV GlobalPOV;

bool testAlpha(const Image& alphamap, const Mesh &model, int x, int y);

bool ExportObj::exportObjFileOnly (char* filename, char* material, const IntMap& imap, 
                                   const Image& leftImage, const Image& alpha, int skip, 
                                   float xyScale, float zScale, float focallengthinmm)
{
  DEBUG(exportO);
  
  DEBUG_VALUE("  file", filename);

  std::ofstream myfile;

  int sizex=imap.getSizeX(), sizey=imap.getSizeY();

  // Open the file to export to.
  myfile.open (filename);
  if (!myfile.is_open()) return false;

  // Starts writing to file
  myfile << "# Exported with Photosculpt\n\n";

  // extracts only the file name, no path.
  // strrchr searches for the last '/' char of the string
  char * materialNoPath = strrchr(material,'\\') + 1 ;

  // special case : cannot find the character /  --> there is already no path in this name
  if (materialNoPath==0)
    materialNoPath=material;

  // header : definition of the material library
  myfile << "mtllib " << materialNoPath << "\n";

  // header : definition of an object name
  myfile << "o photosculpt\n";

  Mesh mesh;
  FloatMap map = convertToFloat(imap) * zScale;
  mesh.create(map, focallengthinmm);

  myfile.precision(4);

  int count = 0;
  int * left, * right, * top, * bottom;
  int firstleftvertex, firstrightvertex, firsttopvertex, firstbottomvertex, lastbottomvertex;

  bool isclosed = GlobalOptions.isExport3DClosed_;

  if (isclosed)
  {
    left   = new int[sizey];
    right  = new int[sizey];
    top    = new int[sizex];
    bottom = new int[sizex];
  }

  // 3D Vertex export: position of each vertex
  //
  for (int y=0; y<sizey ; y+=skip)
    for (int x=0; x<sizex ; x+=skip)
    {
      // Computes the 3d coordinates of the vertex
      float vx=  mesh.getVertexX(x,sizey-1-y);
      float vy= -mesh.getVertexY(x,sizey-1-y) + float(sizey-1) / float(sizex-1);
      float vz=  mesh.getVertexZ(x,sizey-1-y);

      // export the computed values to file
      myfile << "v " << vx << " " << vy << " " << vz << "\n";
      count ++ ; // count is the current vertex number

      if (isclosed)
      {
        // stores the vertex index in 4 tables for future use
        if (x == 0)         left   [y] = count;
        if (x == sizex - 1) right  [y] = count;
        if (y == 0)         top    [x] = count;
        if (y == sizey - 1) bottom [x] = count;
      }
    }
  
  if (isclosed)
  {
    float back_z = -0.3f;

    float leftx   = mesh.getVertexX(0,0);
    float rightx  = mesh.getVertexX(sizex-1,0);
    float topy    = -mesh.getVertexY(0,sizey-1) + float(sizey-1) / float(sizex-1);
    float bottomy = -mesh.getVertexY(0,0)       + float(sizey-1) / float(sizex-1);

    // left wall
    firstleftvertex = count + 1 ;
    for (int y=0; y<sizey ; y+=skip)
    {
      int x=0;
      // Computes the 3d coordinates of the vertex
      float vx =  leftx;
      float vy =  topy + (bottomy-topy) * float(y) / float(sizey-1);
      float vz =  back_z;

      // export the computed values to file
      myfile << "v " << vx << " " << vy << " " << vz << "\n";
      count ++ ; // count is the current vertex number
      }


    // right wall
    firstrightvertex = count + 1 ;
    for (int y=0; y<sizey ; y+=skip)
    {
      int x = sizex-1 ;
      // Computes the 3d coordinates of the vertex
      float vx =  rightx;
      float vy =  topy + (bottomy-topy) * float(y) / float(sizey-1) ;
      float vz =  back_z ;

      // export the computed values to file
      myfile << "v " << vx << " " << vy << " " << vz << "\n";
      count ++ ; // count is the current vertex number
    }
    // top wall
    firsttopvertex = count + 1 ;
    for (int x=0; x<sizex ; x+=skip)
    {
      int y=0;
      // Computes the 3d coordinates of the vertex
      float vx =  leftx + (rightx-leftx) * float(x) / float(sizex-1);
      float vy =  topy;
      float vz =  back_z;

      // export the computed values to file
      myfile << "v " << vx << " " << vy << " " << vz << "\n";
      count ++ ; // count is the current vertex number
      }

    // bottom wall
    firstbottomvertex = count + 1 ;
    for (int x=0; x<sizex ; x+=skip)
    {
      int y = sizey-1 ;
      // Computes the 3d coordinates of the vertex
      float vx=  leftx + (rightx-leftx) * float(x) / float(sizex-1);
      float vy=  bottomy;
      float vz=  back_z ;

      // export the computed values to file
      myfile << "v " << vx << " " << vy << " " << vz << "\n";
      count ++ ; // count is the current vertex number
    }
    lastbottomvertex = count ;
  }

  // write 2 vertexes that symbolize camera position
  Vector3 cameraleft  = GlobalPOV.computeCameraPositionLeft(imap.getSizeX(), imap.getSizeY());
  myfile << "v " << cameraleft.x << " " << cameraleft.y << " " << cameraleft.z << "\n";
  Vector3 cameraright = GlobalPOV.computeCameraPositionRight(imap.getSizeX(), imap.getSizeY());
  myfile << "v " << cameraright.x << " " << cameraright.y << " " << cameraright.z << "\n";
  
  float const yratio = float(sizex-1) / float(sizey-1);

  // 2D texture UV map export
  for   (int y=0; y<sizey ; y+=skip)
    for (int x=0; x<sizex ; x+=skip)
    {
      // Computes the 3d coordinates of the vertex
      //  values are between 0.0 and 1.0

      float tx=        mesh.getVertexU(x,sizey-1-y) ;
      float ty= 1.0f - mesh.getVertexV(x,sizey-1-y) ;

      // export the computed values to file
      // texture coordinates have zero depth (0.0)
      myfile << "vt " << tx << " " << ty << " " << 0.0 << "\n";
    }

  // Material definition
  myfile << "usemtl material\n";

  // Smoothness (smoothness = 1)
  myfile << "s 1\n";

  // computes the exact number of vertex per line in x direction
  int line=0;
  for (int x=0; x<sizex ; x+=skip)
    line++;

  // Face declaration export (quads)
  //
  for (int y=0; y<sizey-skip ; y+=skip)
    for (int x=0; x<sizex-skip ; x+=skip)
    {
      bool show = testAlpha(alpha, mesh, x,sizey-1-y);

      if (show)
      {
        // get the coordinates in the grid
        int coordx = x / skip;
        int coordy = y / skip;

        // references of the 4 vertexes of this face
        long v1 = 1 + coordx +     line *  coordy;		// top left
        long v2 = 1 + coordx + 1 + line *  coordy;		// top right
        long v3 = 1 + coordx + 1 + line * (coordy + 1); 	// bottom right
        long v4 = 1 + coordx +     line * (coordy + 1);		// bottom left

        // export the computed values to file
        // vertex references and texture vertex references are same
        myfile << "f " ;
        myfile << v1 << "/" << v1 << " ";
        myfile << v2 << "/" << v2 << " ";
        myfile << v3 << "/" << v3 << " ";
        myfile << v4 << "/" << v4 << "\n";
      }
    }
    
  if (isclosed)
  {
    // left wall
    for (int y=0; y<sizey-skip ; y+=skip)
    {
      long v1 = left[y];
      long v2 = left[y+1];
      long v3 = firstleftvertex + y + 1;
      long v4 = firstleftvertex + y ;

      myfile << "f " ;
      myfile << v1 << "/" << 1 << " ";
      myfile << v2 << "/" << 1 << " ";
      myfile << v3 << "/" << 1 << " ";
      myfile << v4 << "/" << 1 << "\n";
    }

    // right wall
    for (int y=0; y<sizey-skip ; y+=skip)
    {
      long v1 = right[y];
      long v2 = right[y+1];
      long v3 = firstrightvertex + y + 1;
      long v4 = firstrightvertex + y ;

      myfile << "f " ;
      myfile << v4 << "/" << 1 << " ";
      myfile << v3 << "/" << 1 << " ";
      myfile << v2 << "/" << 1 << " ";
      myfile << v1 << "/" << 1 << "\n";
    }

    // top wall
    for (int x=0; x<sizex-skip ; x+=skip)
    {
      long v1 = top[x];
      long v2 = top[x+1];
      long v3 = firsttopvertex + x + 1;
      long v4 = firsttopvertex + x ;

      myfile << "f " ;
      myfile << v4 << "/" << 1 << " ";
      myfile << v3 << "/" << 1 << " ";
      myfile << v2 << "/" << 1 << " ";
      myfile << v1 << "/" << 1 << "\n";    
    }

    // bottom wall
    for (int x=0; x<sizex-skip ; x+=skip)
    {
      long v1 = bottom[x];
      long v2 = bottom[x+1];
      long v3 = firstbottomvertex + x + 1;
      long v4 = firstbottomvertex + x ;

      myfile << "f " ;
      myfile << v1 << "/" << 1 << " ";
      myfile << v2 << "/" << 1 << " ";
      myfile << v3 << "/" << 1 << " ";
      myfile << v4 << "/" << 1 << "\n";
    }

    // back wall
    myfile << "f " ;
    myfile << firstbottomvertex << "/" << 1 << " ";
    myfile << lastbottomvertex  << "/" << 1 << " ";
    myfile << firstrightvertex  << "/" << 1 << " ";
    myfile << firstleftvertex   << "/" << 1 << "\n";
  
    delete left, right, top, bottom;
  }

  myfile.close();
  return true;
}
