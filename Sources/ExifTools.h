/*
 * ExifTools.h
 *
 *  Created on: 2nd dec 2010
  *      Author: Hippolyte Mounier, Copyright 2010 All rights reserved
 */

// read the file and get EXIF rotation data 
// results are -90, 0 or +90 degree
int getExifRotation(char* filename);

// read the file and get EXIF Focal length data (in 35 mm equivalent)
// returns 0 if cannot read
float getExifFocalLenth35mm(char* filename);