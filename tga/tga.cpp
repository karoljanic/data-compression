#include "tga.hpp"
#include <algorithm>

namespace tga {
Result readImage(Image& image, const std::string& filename) {
  std::ifstream inputFile;
  inputFile.open(filename);
  if (!inputFile.is_open()) {
    return Result::CANNOT_OPEN_FILE;
  }

  try {
    readHeader(image, inputFile);

    for (uint16_t row = 0; row < image.height; row++) {
      for (uint16_t col = 0; col < image.width; col++) {
        inputFile.read(reinterpret_cast<char*>(
                           &image.colormap[image.height - row - 1][col]),
                       sizeof(image.colormap[image.height - row - 1][col]));
      }
    }

    readFooter(image, inputFile);

  } catch (...) {
    return Result::INCORRECT_FILE_STRUCTURE;
  }

  return Result::SUCCESS;
}

Result writeImage(Image& image, const std::string& filename) {
  std::ofstream outputFile;
  outputFile.open(filename);
  if (!outputFile.is_open()) {
    return Result::CANNOT_OPEN_FILE;
  }

  try {
    writeHeader(image, outputFile);

    for (uint16_t row = 0; row < image.height; row++) {
      for (uint16_t col = 0; col < image.width; col++) {
        outputFile.write(reinterpret_cast<char*>(
                             &image.colormap[image.height - row - 1][col]),
                         sizeof(image.colormap[image.height - row - 1][col]));
      }
    }

    writeFooter(image, outputFile);

  } catch (...) {
    return Result::INCORRECT_FILE_STRUCTURE;
  }

  return Result::SUCCESS;
}

Result readHeader(Image& image, std::ifstream& inputFile) {
  if (!inputFile.is_open()) {
    return Result::CANNOT_OPEN_FILE;
  }

  inputFile.read(reinterpret_cast<char*>(&image.idLength),
                 sizeof(image.idLength));
  inputFile.read(reinterpret_cast<char*>(&image.colormapType),
                 sizeof(image.colormapType));
  inputFile.read(reinterpret_cast<char*>(&image.imageType),
                 sizeof(image.imageType));
  inputFile.read(reinterpret_cast<char*>(&image.colormapOrigin),
                 sizeof(image.colormapOrigin));
  inputFile.read(reinterpret_cast<char*>(&image.colormapLength),
                 sizeof(image.colormapLength));
  inputFile.read(reinterpret_cast<char*>(&image.colormapDepth),
                 sizeof(image.colormapDepth));
  inputFile.read(reinterpret_cast<char*>(&image.xOrigin),
                 sizeof(image.xOrigin));
  inputFile.read(reinterpret_cast<char*>(&image.yOrigin),
                 sizeof(image.yOrigin));
  inputFile.read(reinterpret_cast<char*>(&image.width), sizeof(image.width));
  inputFile.read(reinterpret_cast<char*>(&image.height), sizeof(image.height));
  inputFile.read(reinterpret_cast<char*>(&image.pixelDepth),
                 sizeof(image.pixelDepth));
  inputFile.read(reinterpret_cast<char*>(&image.imageDescriptor),
                 sizeof(image.imageDescriptor));

  char idByte;
  for (uint8_t idByteIndex = 0; idByteIndex < image.idLength; idByteIndex++) {
    inputFile.read(&idByte, sizeof(idByte));
    image.imageId.push_back(idByte);
  }

  image.colormap.resize(image.height);
  for (uint16_t row = 0; row < image.height; row++) {
    image.colormap[row].resize(image.width, Color(0, 0, 0));
  }

  return Result::SUCCESS;
}

Result writeHeader(Image& image, std::ofstream& outputFile) {
  if (!outputFile.is_open()) {
    return Result::CANNOT_OPEN_FILE;
  }

  outputFile.write(reinterpret_cast<char*>(&image.idLength),
                   sizeof(image.idLength));
  outputFile.write(reinterpret_cast<char*>(&image.colormapType),
                   sizeof(image.colormapType));
  outputFile.write(reinterpret_cast<char*>(&image.imageType),
                   sizeof(image.imageType));
  outputFile.write(reinterpret_cast<char*>(&image.colormapOrigin),
                   sizeof(image.colormapOrigin));
  outputFile.write(reinterpret_cast<char*>(&image.colormapLength),
                   sizeof(image.colormapLength));
  outputFile.write(reinterpret_cast<char*>(&image.colormapDepth),
                   sizeof(image.colormapDepth));
  outputFile.write(reinterpret_cast<char*>(&image.xOrigin),
                   sizeof(image.xOrigin));
  outputFile.write(reinterpret_cast<char*>(&image.yOrigin),
                   sizeof(image.yOrigin));
  outputFile.write(reinterpret_cast<char*>(&image.width), sizeof(image.width));
  outputFile.write(reinterpret_cast<char*>(&image.height),
                   sizeof(image.height));
  outputFile.write(reinterpret_cast<char*>(&image.pixelDepth),
                   sizeof(image.pixelDepth));
  outputFile.write(reinterpret_cast<char*>(&image.imageDescriptor),
                   sizeof(image.imageDescriptor));

  for (uint8_t idByteIndex = 0; idByteIndex < image.idLength; idByteIndex++) {
    outputFile.write(reinterpret_cast<char*>(&image.imageId[idByteIndex]),
                     sizeof(image.imageId[idByteIndex]));
  }

  return Result::SUCCESS;
}

Result readFooter(Image& image, std::ifstream& inputFile) {
  if (!inputFile.is_open()) {
    return Result::CANNOT_OPEN_FILE;
  }

  constexpr uint8_t footerSize{26};
  image.footer.reserve(footerSize);

  char idByte;
  for (uint8_t footerByteIndex = 0; footerByteIndex < footerSize;
       footerByteIndex++) {
    inputFile.read(&idByte, sizeof(idByte));
    image.footer.push_back(idByte);
  }

  return Result::SUCCESS;
}

Result writeFooter(Image& image, std::ofstream& outputFile) {
  if (!outputFile.is_open()) {
    return Result::CANNOT_OPEN_FILE;
  }

  constexpr uint8_t footerSize{26};
  for (uint8_t footerByteIndex = 0; footerByteIndex < footerSize;
       footerByteIndex++) {
    outputFile.write(reinterpret_cast<char*>(&image.footer[footerByteIndex]),
                     sizeof(image.footer[footerByteIndex]));
  }

  return Result::SUCCESS;
}

void copyImage(const Image& source, Image& destination) {
  destination.idLength = source.idLength;
  destination.colormapType = source.colormapType;
  destination.imageType = source.imageType;
  destination.colormapOrigin = source.colormapOrigin;
  destination.colormapLength = source.colormapLength;
  destination.colormapDepth = source.colormapDepth;
  destination.xOrigin = source.xOrigin;
  destination.yOrigin = source.yOrigin;
  destination.width = source.width;
  destination.height = source.height;
  destination.pixelDepth = source.pixelDepth;
  destination.imageDescriptor = source.imageDescriptor;
  destination.imageId = source.imageId;
  destination.footer = source.footer;

  destination.colormap.resize(destination.height);
  for (uint16_t row = 0; row < destination.height; row++) {
    destination.colormap[row].resize(destination.width, Color(0, 0, 0));
  }

  for (uint16_t row = 0; row < destination.height; row++) {
    for (uint16_t col = 0; col < destination.width; col++) {
      destination.colormap[row][col] = source.colormap[row][col];
    }
  }
}

void calculateEntropy(const Image& image, double& total, double& red,
                      double& green, double& blue) {
  constexpr size_t channelSize{256};
  constexpr size_t allChannelsSize{channelSize * channelSize * channelSize};

  std::vector<uint64_t> redCounter(channelSize, 0);
  std::vector<uint64_t> greenCounter(channelSize, 0);
  std::vector<uint64_t> blueCounter(channelSize, 0);
  std::vector<uint64_t> totalCounter(allChannelsSize, 0);

  for (uint16_t row = 0; row < image.height; row++) {
    for (uint16_t col = 0; col < image.width; col++) {
      totalCounter[image.colormap[row][col].blue * channelSize * channelSize +
                   image.colormap[row][col].green * channelSize +
                   image.colormap[row][col].red]++;
      redCounter[image.colormap[row][col].red]++;
      greenCounter[image.colormap[row][col].green]++;
      blueCounter[image.colormap[row][col].blue]++;
    }
  }

  total = 0.0;
  red = 0.0;
  green = 0.0;
  blue = 0.0;

  const auto pixelsNumber = static_cast<double>(image.width * image.height);
  for (size_t charIndex = 0; charIndex < 256; charIndex++) {
    if (redCounter[charIndex] != 0) {
      double redProbability =
          static_cast<double>(redCounter[charIndex]) / pixelsNumber;
      red -= redProbability * std::log2(redProbability);
    }

    if (greenCounter[charIndex] != 0) {
      double greenProbability =
          static_cast<double>(greenCounter[charIndex]) / pixelsNumber;
      green -= greenProbability * std::log2(greenProbability);
    }

    if (blueCounter[charIndex] != 0) {
      double blueProbability =
          static_cast<double>(blueCounter[charIndex]) / pixelsNumber;
      blue -= blueProbability * std::log2(blueProbability);
    }
  }

  for (size_t charIndex = 0; charIndex < allChannelsSize; charIndex++) {
    if (totalCounter[charIndex] != 0) {
      double totalProbability =
          static_cast<double>(totalCounter[charIndex]) / pixelsNumber;
      total -= totalProbability * std::log2(totalProbability);
    }
  }
}

uint32_t countColors(const Image& image) {
  uint32_t colorsNumber{0};
  std::vector<uint32_t> colors;
  for (uint16_t row = 0; row < image.height; row++) {
    for (uint16_t col = 0; col < image.width; col++) {
      uint32_t colorIndex = image.colormap[row][col].blue * 256 * 256 +
                            image.colormap[row][col].green * 256 +
                            image.colormap[row][col].red;
      if (std::find(colors.cbegin(), colors.cend(), colorIndex) ==
          colors.cend()) {
        colors.push_back(colorIndex);
        colorsNumber++;
      }
    }
  }

  return colorsNumber;
}

uint32_t compareImages(const Image& image1, const Image& image2) {
  uint32_t differentPixelsNumber{0};
  for (uint16_t row = 0; row < image1.height; row++) {
    for (uint16_t col = 0; col < image1.width; col++) {
      if (image1.colormap[row][col].blue != image2.colormap[row][col].blue ||
          image1.colormap[row][col].green != image2.colormap[row][col].green ||
          image1.colormap[row][col].red != image2.colormap[row][col].red) {
        differentPixelsNumber++;
      }
    }
  }

  return differentPixelsNumber;
}
}  // namespace tga
