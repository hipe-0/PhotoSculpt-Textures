// basic file operations
#include "ExportPly.h"

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "Mesh.h"
#include "Convert.h"
#include "Includes.h"
#include "Options.h"
#include "Color.h"

// infos here
// http://paulbourke.net/dataformats/ply/

extern const char*  PROGRAM_NAME;

bool testAlpha(const Image& alphamap, const Mesh &model, int x, int y);

bool ExportPly::exportFile (char* filename, const Image& leftImage,
  const Image& alpha, const IntMap& map, 
  int skip, float xyScale, float zScale, float focallengthinmm)
{
  DEBUG(ExportP);

  std::ofstream myfile;

  int sizex=map.getSizeX(), sizey=map.getSizeY();

  // Open the file to export to.
  myfile.open (filename);
  if (!myfile.is_open()) return false;

  // Starts writing to file
  myfile << "ply\n";
  myfile << "format ascii 1.0\n";
  myfile << "comment created with\n";
  myfile << "comment " << PROGRAM_NAME << "\n";

  int numberofvertexes = sizex*sizey;
  myfile << "element vertex " << numberofvertexes << "\n";
  myfile << "property float x\n"     ;
  myfile << "property float y\n"     ;
  myfile << "property float z\n"     ;
  myfile << "property uchar red\n"   ;
  myfile << "property uchar green\n" ;
  myfile << "property uchar blue\n"  ;

  Mesh mesh;
  FloatMap floatmap = convertToFloat(map) * zScale;
  mesh.create(floatmap, focallengthinmm);  
  
  int numberoffaces = 0;
  for   (int y=0; y<sizey-skip ; y+=skip)
    for (int x=0; x<sizex-skip ; x+=skip)
      if ( testAlpha(alpha, mesh, x,y) )
        numberoffaces ++;

  myfile << "element face " << numberoffaces << "\n";

  myfile << "property list uchar int vertex_index\n";
  myfile << "end_header\n";

  myfile.precision(4);

  int count = 0;

  // 3D Vertex export: position of each vertex
  
  for   (int y=0; y<sizey ; y+=skip)
    for (int x=0; x<sizex ; x+=skip)
    {
      // Computes the 3d coordinates of the vertex
      float vx=  mesh.getVertexX(x,y) ;
      float vy= -mesh.getVertexY(x,y) + float(sizey-1) / float(sizex-1);
      float vz=  mesh.getVertexZ(x,y) ;

      float tx= mesh.getVertexU(x,y) ;
      float ty= mesh.getVertexV(x,y) ;
      Color c = getPixel(leftImage, tx, ty) ;

      // export the computed values to file
      myfile << vx << " " << vy  << " " << vz  << " " 
            << int(c.r) << " " << int(c.g) << " " << int(c.b) << "\n";

      count ++ ; // count is the current vertex number
    }
  
  // computes the exact number of vertex per line in x direction
  int line=0;
  for (int x=0; x<sizex ; x+=skip)
    line++;

  // Face declaration export (quads)
  //
  for (int y=0; y<sizey-skip ; y+=skip)
    for (int x=0; x<sizex-skip ; x+=skip)
    {
      bool show = testAlpha(alpha, mesh, x,y);

      if (show)
      {
        // get the coordinates in the grid
        int coordx = x / skip;
        int coordy = y / skip;

        // references of the 4 vertexes of this face
        long v1 = coordx +     line *  coordy;		// top left
        long v2 = coordx + 1 + line *  coordy;		// top right
        long v3 = coordx + 1 + line * (coordy + 1); 	// bottom right
        long v4 = coordx +     line * (coordy + 1);		// bottom left

        // export the computed values to file
        // vertex references and texture vertex references are same
        myfile << 4 << " " << v4 << " " << v3 << " " << v2 << " " << v1 << "\n";
      }
    }

  myfile.close();
  return true;
}

/* sample ply file

ply
format ascii 1.0           { ascii/binary, format version number }
comment made by Greg Turk  { comments keyword specified, like all lines }
comment this file is a cube
element vertex 8           { define "vertex" element, 8 of them in file }
property float x           { vertex contains float "x" coordinate }
property float y           { y coordinate is also a vertex property }
property float z           { z coordinate, too }
element face 6             { there are 6 "face" elements in the file }
property list uchar int vertex_index { "vertex_indices" is a list of ints }
end_header                 { delimits the end of the header }
0 0 0                      { start of vertex list }
0 0 1
0 1 1
0 1 0
1 0 0
1 0 1
1 1 1
1 1 0
4 0 1 2 3                  { start of face list }
4 7 6 5 4
4 0 4 5 1
4 1 5 6 2
4 2 6 7 3
4 3 7 4 0
*/