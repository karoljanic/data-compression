#ifndef TGA_HPP
#define TGA_HPP

#include <cinttypes>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace tga {
enum Result { SUCCESS, CANNOT_OPEN_FILE, INCORRECT_FILE_STRUCTURE };

struct Color {
  uint8_t blue;
  uint8_t green;
  uint8_t red;

  Color() : blue(0), green(0), red(0) {}
  Color(uint8_t blue, uint8_t green, uint8_t red)
      : blue(blue), green(green), red(red) {}
};

struct Image {
  uint8_t idLength;
  uint8_t colormapType;
  uint8_t imageType;
  uint16_t colormapOrigin;
  uint16_t colormapLength;
  uint8_t colormapDepth;
  uint16_t xOrigin;
  uint16_t yOrigin;
  uint16_t width;
  uint16_t height;
  uint8_t pixelDepth;
  uint8_t imageDescriptor;
  std::string imageId;
  std::vector<std::vector<Color>> colormap;
  std::string footer;
};

Result readImage(Image& image, const std::string& filename);
Result writeImage(Image& image, const std::string& filename);
Result readHeader(Image& image, std::ifstream& inputFile);
Result writeHeader(Image& image, std::ofstream& outputFile);
Result readFooter(Image& image, std::ifstream& inputFile);
Result writeFooter(Image& image, std::ofstream& outputFile);
void copyImage(const Image& source, Image& destination);
void calculateEntropy(const Image& image, double& total, double& red,
                      double& green, double& blue);
uint32_t countColors(const Image& image);
uint32_t compareImages(const Image& image1, const Image& image2);
}  // namespace tga

#endif  // TGA_HPP