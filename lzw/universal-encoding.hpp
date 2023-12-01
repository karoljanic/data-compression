#ifndef UNIVERSAL_ENCODING_HPP
#define UNIVERSAL_ENCODING_HPP

#include <algorithm>
#include <vector>

#include "universal-encoding.hpp"
#include "utils.hpp"

inline void convert_number_to_bits(const size_t number,
                                   std::vector<bool>& bits) {
  size_t numberTmp{number};
  while (numberTmp > 0) {
    bits.push_back(numberTmp % 2);
    numberTmp >>= 1;
  }

  std::reverse(bits.begin(), bits.end());
}

inline size_t convert_bits_to_number(const size_t bitsNumber,
                                     std::vector<bool>& bits,
                                     std::ifstream& inputFile) {
  while (bits.size() < bitsNumber) {
    readBitsFromFile(inputFile, bits);
  }

  size_t number{0};
  for (size_t bitNum = 0; bitNum < bitsNumber; bitNum++) {
    number <<= 1;
    number += bits[bitNum];
  }

  bits.erase(bits.begin(), bits.begin() + bitsNumber);

  return number;
}

inline void encode_ellias_gamma(const size_t number,
                                std::vector<bool>& outputBits) {
  std::vector<bool> numberAsBits{};
  convert_number_to_bits(number, numberAsBits);

  std::vector<bool> zerosPrefix{};
  for (size_t i = 0; i < (numberAsBits.size() - 1); i++) {
    zerosPrefix.push_back(false);
  }

  outputBits.insert(outputBits.end(), zerosPrefix.begin(), zerosPrefix.end());
  outputBits.insert(outputBits.end(), numberAsBits.begin(), numberAsBits.end());
}

inline size_t decode_ellias_gamma(std::vector<bool>& inputBits,
                                  std::ifstream& inputFile) {
  if (inputBits.size() < 1) {
    readBitsFromFile(inputFile, inputBits);
  }

  size_t numberBitsCounter{0};
  while (inputBits[numberBitsCounter] == 0) {
    numberBitsCounter++;

    if (inputBits.size() <= numberBitsCounter) {
      readBitsFromFile(inputFile, inputBits);
    }
  }

  inputBits.erase(inputBits.begin(), inputBits.begin() + numberBitsCounter);

  return convert_bits_to_number(numberBitsCounter + 1, inputBits, inputFile);
}

inline void encode_ellias_delta(const size_t number,
                                std::vector<bool>& outputBits) {
  std::vector<bool> numberAsBits{};
  convert_number_to_bits(number, numberAsBits);

  encode_ellias_gamma(numberAsBits.size(), outputBits);
  outputBits.insert(outputBits.end(), numberAsBits.begin() + 1,
                    numberAsBits.end());
}

inline size_t decode_ellias_delta(std::vector<bool>& inputBits,
                                  std::ifstream& inputFile) {
  const size_t numberBits = decode_ellias_gamma(inputBits, inputFile);

  inputBits.insert(inputBits.begin(), true);
  return convert_bits_to_number(numberBits, inputBits, inputFile);
}

inline void encode_ellias_omega(const size_t number,
                                std::vector<bool>& outputBits) {

  std::vector<bool> resultBits = {false};

  size_t numberTmp{number};
  while (numberTmp > 1) {
    std::vector<bool> numberAsBits{};
    convert_number_to_bits(numberTmp, numberAsBits);
    numberTmp = numberAsBits.size() - 1;
    resultBits.insert(resultBits.begin(), numberAsBits.begin(),
                      numberAsBits.end());
  }

  outputBits.insert(outputBits.end(), resultBits.begin(), resultBits.end());
}

inline size_t decode_ellias_omega(std::vector<bool>& inputBits,
                                  std::ifstream& inputFile) {

  if (inputBits.size() < 1) {
    readBitsFromFile(inputFile, inputBits);
  }

  size_t number{1};
  while (inputBits[0] != false) {
    number = convert_bits_to_number(number + 1, inputBits, inputFile);

    if (inputBits.size() < 1) {
      readBitsFromFile(inputFile, inputBits);
    }
  }

  inputBits.erase(inputBits.begin(), inputBits.begin() + 1);

  return number;
}

inline void encode_fibonacci(const size_t number, std::vector<bool>& outputBits,
                             std::vector<size_t>& fibonacciSequence) {
  while (number > fibonacciSequence.back()) {
    const size_t currentFibSeqSize{fibonacciSequence.size()};
    fibonacciSequence.push_back(fibonacciSequence[currentFibSeqSize - 2] +
                                fibonacciSequence[currentFibSeqSize - 1]);
  }

  size_t maxFibIndex{0};
  for (maxFibIndex = 0; maxFibIndex < fibonacciSequence.size(); maxFibIndex++) {
    if (fibonacciSequence[maxFibIndex] > number) {
      break;
    }
  }

  size_t numberTmp{number};
  std::vector<bool> resultBits{};
  for (size_t fibIndex = maxFibIndex - 1; fibIndex > 0; fibIndex--) {
    if (numberTmp >= fibonacciSequence[fibIndex]) {
      resultBits.push_back(true);
      numberTmp -= fibonacciSequence[fibIndex];
    } else {
      resultBits.push_back(false);
    }
  }

  outputBits.insert(outputBits.end(), resultBits.rbegin(), resultBits.rend());
  outputBits.push_back(true);
}

inline size_t decode_fibonacci(std::vector<bool>& inputBits,
                               std::vector<size_t>& fibonacciSequence,
                               std::ifstream& inputFile) {
  bool lastBit{false};
  size_t inputBitCounter{0};
  size_t number{0};

  if (inputBits.size() < 1) {
    readBitsFromFile(inputFile, inputBits);
  }

  while (inputBits[inputBitCounter] != lastBit || lastBit == false) {
    const size_t currentFibSeqSize{fibonacciSequence.size()};
    if (inputBitCounter + 1 >= currentFibSeqSize) {
      fibonacciSequence.push_back(fibonacciSequence[currentFibSeqSize - 2] +
                                  fibonacciSequence[currentFibSeqSize - 1]);
    }

    if (inputBits[inputBitCounter]) {
      number += fibonacciSequence[inputBitCounter + 1];
    }

    lastBit = inputBits[inputBitCounter];
    inputBitCounter++;

    if (inputBits.size() <= inputBitCounter) {
      readBitsFromFile(inputFile, inputBits);
    }
  }

  if (inputBits.size() < (inputBitCounter + 1)) {
    readBitsFromFile(inputFile, inputBits);
  }

  inputBits.erase(inputBits.begin(), inputBits.begin() + inputBitCounter + 1);

  return number;
}

#endif  // UNIVERSAL_ENCODING_HPP
