#ifndef EXPOSURE_H_
#define EXPOSURE_H_

class Exposure
{
public:
  static void getMinMax(const Image& image, int channel, unsigned char& min, unsigned char& max);
  static void setMinMax(Image& image, int channel, unsigned char min, unsigned char max);
  static void autoLevel(Image& image);
  static float percentok(const Image& image1, const Image& image2, int threshold);
};


#endif