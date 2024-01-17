#include <fstream>
#include <iostream>

#include "../tga/tga.hpp"
#include "bits-file.hpp"

void decodeImage(std::ifstream &inputFile, std::string outputFilename) {
  tga::Image image;
  tga::readHeader(image, inputFile);

  std::vector<bool> buffer;

  size_t bitsNumber = (image.height * image.width) / 2;

  uint8_t significantBits = readNumberFromFile<uint8_t, 8>(inputFile, buffer);
  (void)significantBits;

  std::vector<int16_t> redChannelLow;
  std::vector<int16_t> redChannelHigh;
  std::vector<int16_t> greenChannelLow;
  std::vector<int16_t> greenChannelHigh;
  std::vector<int16_t> blueChannelLow;
  std::vector<int16_t> blueChannelHigh;

  for (size_t i = 0; i < bitsNumber; i++) {
	redChannelLow.push_back(readNumberFromFile<int16_t, 16>(inputFile, buffer));
  }

  for (size_t i = 0; i < bitsNumber; i++) {
	redChannelHigh.push_back(readNumberFromFile<int16_t, 16>(inputFile, buffer));
  }

  for (size_t i = 0; i < bitsNumber; i++) {
	greenChannelLow.push_back(readNumberFromFile<int16_t, 16>(inputFile, buffer));
  }

  for (size_t i = 0; i < bitsNumber; i++) {
	greenChannelHigh.push_back(readNumberFromFile<int16_t, 16>(inputFile, buffer));
  }

  for (size_t i = 0; i < bitsNumber; i++) {
	blueChannelLow.push_back(readNumberFromFile<int16_t, 16>(inputFile, buffer));
  }

  for (size_t i = 0; i < bitsNumber; i++) {
	blueChannelHigh.push_back(readNumberFromFile<int16_t, 16>(inputFile, buffer));
  }

  std::vector<int16_t> redChannel(2 * redChannelLow.size(), 0);
  int16_t redChannelLowAccumulator = redChannelLow[0];
  redChannel[0] = redChannelLowAccumulator;
  for (size_t i = 1; i < redChannelLow.size(); i++) {
	redChannelLowAccumulator += redChannelLow[i];
	redChannel[2 * i] = redChannelLowAccumulator + redChannelHigh[i];
	redChannel[2 * i - 1] = redChannelLowAccumulator - redChannelHigh[i];
  }

  std::vector<int16_t> blueChannel(2 * blueChannelLow.size(), 0);
  int16_t blueChannelLowAccumulator = blueChannelLow[0];
  blueChannel[0] = blueChannelLowAccumulator;
  for (size_t i = 1; i < blueChannelLow.size(); i++) {
	blueChannelLowAccumulator += blueChannelLow[i];
	blueChannel[2 * i] = blueChannelLowAccumulator + blueChannelHigh[i];
	blueChannel[2 * i - 1] = blueChannelLowAccumulator - blueChannelHigh[i];
  }

  std::vector<int16_t> greenChannel(2 * greenChannelLow.size(), 0);
  int16_t greenChannelLowAccumulator = greenChannelLow[0];
  greenChannel[0] = greenChannelLowAccumulator;
  for (size_t i = 1; i < greenChannelLow.size(); i++) {
	greenChannelLowAccumulator += greenChannelLow[i];
	greenChannel[2 * i] = greenChannelLowAccumulator + greenChannelHigh[i];
	greenChannel[2 * i - 1] = greenChannelLowAccumulator - greenChannelHigh[i];
  }

  for (size_t i = 0; i < image.height; i++) {
	for (size_t j = 0; j < image.width; j++) {
	  image.colormap[i][j].red = redChannel[i * image.width + j];
	  image.colormap[i][j].green = greenChannel[i * image.width + j];
	  image.colormap[i][j].blue = blueChannel[i * image.width + j];
	}
  }

  tga::readFooter(image, inputFile);

  tga::writeImage(image, outputFilename);
}

int main(int argc, char **argv) {
  if (argc != 3) {
	std::cout << "Bad input!. Correct input is: ./decoder <encoded file> "
				 "<output file>\n";

	return -1;
  }

  const std::string inputFileName{argv[1]};
  const std::string outputFileName{argv[2]};

  std::ifstream inputFile{inputFileName, std::ios::binary};

  if (inputFile.fail()) {
	std::cout << "Cannot open file " << inputFileName << '\n';
	return -1;
  }

  decodeImage(inputFile, outputFileName);
}