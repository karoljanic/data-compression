#include <fstream>
#include <iostream>
#include <map>

#include "../tga/tga.hpp"
#include "bits-file.hpp"
#include "lgb.hpp"

void quantizeVector(const std::vector<int16_t>& input,
                    std::map<int16_t, int16_t>& output, uint8_t bits) {
  std::vector<double> inputDouble;
  for (const auto& value : input) {
    inputDouble.push_back(value);
  }

  std::vector<double> codebook;
  lgb(inputDouble, codebook, bits, 0.1, 10);

  for (const auto& value : input) {
    double minDistance = std::numeric_limits<double>::max();
    int16_t quantizedValue = 0;
    for (const auto& codebookValue : codebook) {
      int16_t castedCodebookValue = static_cast<int16_t>(codebookValue);
      double distance = elementsDistance(value, castedCodebookValue);
      if (distance < minDistance) {
        minDistance = distance;
        quantizedValue = castedCodebookValue;
      }
    }
    output[value] = quantizedValue;
  }
}

void processChannel(std::vector<int16_t>& input,
                    std::vector<int16_t>& outputLow,
                    std::vector<int16_t>& outputHigh, uint8_t bits) {

  // calculate low(average)
  std::vector<int16_t> low;
  for (size_t i = 0; i < input.size(); i += 2) {
    size_t j;
    if (i == 0) {
      j = 0;
    } else {
      j = i - 1;
    }
    low.push_back((input[i] + input[j]) / 2);
  }

  // calculate high(deviation)
  std::vector<int16_t> high;
  for (size_t i = 0; i < input.size(); i += 2) {
    size_t j;
    if (i == 0) {
      j = 0;
    } else {
      j = i - 1;
    }
    high.push_back((input[i] - input[j]) / 2);
  }

  // save low as difference
  std::vector<int16_t> lowDifference;
  lowDifference.emplace_back(low[0]);
  for (size_t i = 1; i < low.size(); i++) {
    lowDifference.push_back(low[i] - low[i - 1]);
  }

  // quantize low difference
  std::map<int16_t, int16_t> lowDifferenceQuantizationMap;
  quantizeVector(lowDifference, lowDifferenceQuantizationMap, bits);

  // save quantized low as difference
  std::vector<int16_t> lowDifference2;
  lowDifference2.emplace_back(low[0]);
  int16_t decodedLow = lowDifferenceQuantizationMap[low[0]];
  for (size_t i = 1; i < low.size(); i++) {
    int16_t diff = low[i] - decodedLow;
    lowDifference2.emplace_back(diff);
    decodedLow += lowDifferenceQuantizationMap[diff];
  }

  // save quantized low
  for (size_t i = 0; i < lowDifference2.size(); i++) {
    // outputLow.push_back(lowDifferenceQuantizationMap[lowDifference2[i]]);
    outputLow.push_back(lowDifferenceQuantizationMap[lowDifference[i]]);
  }

  // quantize high
  std::map<int16_t, int16_t> highQuantizationMap;
  quantizeVector(high, highQuantizationMap, bits);

  // save quantized high
  for (size_t i = 0; i < high.size(); i++) {
    outputHigh.push_back(highQuantizationMap[high[i]]);
  }
}

void encodeImage(tga::Image& inputImage, uint8_t bits,
                 std::ofstream& outputFile) {
  std::vector<int16_t> redChannel;
  std::vector<int16_t> greenChannel;
  std::vector<int16_t> blueChannel;

  for (size_t i = 0; i < inputImage.height; i++) {
    for (size_t j = 0; j < inputImage.width; j++) {
      redChannel.push_back(inputImage.colormap[i][j].red);
      greenChannel.push_back(inputImage.colormap[i][j].green);
      blueChannel.push_back(inputImage.colormap[i][j].blue);
    }
  }

  if (redChannel.size() % 2 != 0) {
    redChannel.push_back(0);
    greenChannel.push_back(0);
    blueChannel.push_back(0);
  }

  std::vector<int16_t> redChannelQuantizedLow;
  std::vector<int16_t> redChannelQuantizedHigh;
  std::vector<int16_t> greenChannelQuantizedLow;
  std::vector<int16_t> greenChannelQuantizedHigh;
  std::vector<int16_t> blueChannelQuantizedLow;
  std::vector<int16_t> blueChannelQuantizedHigh;

  processChannel(redChannel, redChannelQuantizedLow, redChannelQuantizedHigh,
                 bits);
  processChannel(greenChannel, greenChannelQuantizedLow,
                 greenChannelQuantizedHigh, bits);
  processChannel(blueChannel, blueChannelQuantizedLow, blueChannelQuantizedHigh,
                 bits);

  tga::writeHeader(inputImage, outputFile);

  std::vector<bool> bitsBuffer;

  writeNumberBitsToFile<8>(std::bitset<8>(bits), bitsBuffer);

  for (size_t i = 0; i < redChannelQuantizedLow.size(); i++) {
    writeNumberBitsToFile<16>(redChannelQuantizedLow[i], bitsBuffer);
  }

  for (size_t i = 0; i < redChannelQuantizedHigh.size(); i++) {
    writeNumberBitsToFile<16>(redChannelQuantizedHigh[i], bitsBuffer);
  }

  for (size_t i = 0; i < greenChannelQuantizedLow.size(); i++) {
    writeNumberBitsToFile<16>(greenChannelQuantizedLow[i], bitsBuffer);
  }

  for (size_t i = 0; i < greenChannelQuantizedHigh.size(); i++) {
    writeNumberBitsToFile<16>(greenChannelQuantizedHigh[i], bitsBuffer);
  }

  for (size_t i = 0; i < blueChannelQuantizedLow.size(); i++) {
    writeNumberBitsToFile<16>(blueChannelQuantizedLow[i], bitsBuffer);
  }

  for (size_t i = 0; i < blueChannelQuantizedHigh.size(); i++) {
    writeNumberBitsToFile<16>(blueChannelQuantizedHigh[i], bitsBuffer);
  }

  flushBuffer(outputFile, bitsBuffer);

  tga::writeFooter(inputImage, outputFile);
}

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Bad input!. Correct input is: ./encoder <file to encode> "
                 "<output file> <quantization bits number>\n";
    return -1;
  }

  const std::string inputFileName{argv[1]};
  const std::string outputFileName{argv[2]};
  const uint8_t quantizationBitsNumber{
      static_cast<uint8_t>(std::stoi(argv[3]))};

  std::ifstream inputFile{inputFileName, std::ios::binary};
  std::ofstream outputFile{outputFileName, std::ios::binary};

  if (inputFile.fail()) {
    std::cout << "Cannot open file " << inputFileName << '\n';
    return -1;
  }

  if (outputFile.fail()) {
    std::cout << "Cannot create file " << outputFileName << '\n';
    return -1;
  }

  tga::Image inputImage;
  if (tga::readImage(inputImage, inputFileName) == tga::SUCCESS) {
    encodeImage(inputImage, quantizationBitsNumber, outputFile);
  } else {
    std::cout << "Error: cannot read image!" << std::endl;
  }
}