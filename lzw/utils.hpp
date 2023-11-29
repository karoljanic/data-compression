#ifndef UTILS_HPP
#define UTILS_HPP

#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

constexpr size_t ALPHABET_SIZE = 256;

typedef struct FileInfo {
  size_t totalCharsCounter;
  size_t charactersCounter[ALPHABET_SIZE + 1];
} FileInfo;

inline void writeBitsToFile(std::ofstream& file, std::vector<bool>& bits,
                            bool fillWithZeros) {
  if (fillWithZeros && bits.size() % 8 != 0) {
    while (bits.size() % 8 != 0) {
      bits.push_back(false);
    }
  }

  while (bits.size() >= 8) {
    uint8_t byteValue{0};
    for (size_t bitIndex = 0; bitIndex < 8; bitIndex++) {
      byteValue <<= 1;
      byteValue |= bits[bitIndex];
    }

    bits.erase(bits.begin(), bits.begin() + 8);

    char byteChar{static_cast<char>(byteValue)};
    file.write(&byteChar, 1);
  }
}

inline void readBitsFromFile(std::ifstream& file, std::vector<bool>& bits) {
  char byteChar;
  file.read(&byteChar, 1);
  uint8_t byteValue{static_cast<uint8_t>(byteChar)};

  for (ssize_t bitIndex = 7; bitIndex >= 0; bitIndex--) {
    bool bit = (byteValue & (1 << bitIndex)) != 0;
    bits.push_back(bit);
  }
}

inline double calculateEntropy(const FileInfo& fileInfo) {
  double entropy = 0.0;

  for (size_t charCode = 0; charCode < ALPHABET_SIZE; charCode++) {
    if (fileInfo.charactersCounter[charCode] > 0) {
      entropy += -1.0 * fileInfo.charactersCounter[charCode] *
                 log2(fileInfo.charactersCounter[charCode]);
    }
  }

  entropy /= fileInfo.totalCharsCounter;
  return entropy + log2(fileInfo.totalCharsCounter);
}

#endif  // UTILS_HPP
