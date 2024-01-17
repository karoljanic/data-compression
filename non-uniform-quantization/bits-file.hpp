#ifndef BITS_FILE_HPP
#define BITS_FILE_HPP

#include <fstream>
#include <vector>
#include <cstdint>
#include <bitset>

void writeBitsToFile(std::ofstream &outputFile, std::vector<bool> &bitsVector) {
  size_t residue = bitsVector.size() % 8;
  size_t bitsToRead = bitsVector.size() - residue;

  for (size_t i = 0; i < bitsToRead; i += 8) {
	uint8_t byteValue{0};
	for (ssize_t j = 7; j >= 0; j--) {
	  byteValue <<= 1;
	  byteValue |= bitsVector[j + i];
	}

	outputFile.write(reinterpret_cast<char *>(&byteValue), sizeof(byteValue));
  }

  bitsVector.erase(bitsVector.begin(), bitsVector.begin() + bitsToRead);
}

template<size_t N>
void writeNumberBitsToFile(std::bitset<N> number,
						   std::vector<bool> &buffer) {

  for (size_t i = 0; i < N; i++) {
	buffer.emplace_back(number[i]);
  }
}

void flushBuffer(std::ofstream &outputFile, std::vector<bool> &buffer) {
  if (!buffer.empty()) {
	while (buffer.size() % 8 != 0) {
	  buffer.push_back(false);
	}
	writeBitsToFile(outputFile, buffer);
  }
}

void readBitsFromFile(std::ifstream &inputFile, std::vector<bool> &bitsVector) {
  uint8_t byteValue{0};
  inputFile.read(reinterpret_cast<char *>(&byteValue), sizeof(byteValue));

  for (ssize_t i = 7; i >= 0; i--) {
	bitsVector.push_back((byteValue & (1 << (7 - i))) != 0);
  }
}

template<size_t N>
std::bitset<N> readNumberBitsFromFile(std::ifstream &inputFile,
									  std::vector<bool> &buffer) {
  std::bitset<N> result;

  while (buffer.size() < N) {
	readBitsFromFile(inputFile, buffer);
  }

  for (size_t i = 0; i < N; i++) {
	result[i] = buffer[i];
  }

  buffer.erase(buffer.begin(), buffer.begin() + N);

  return result;
}

template<typename T, size_t N>
T readNumberFromFile(std::ifstream &inputFile,
					 std::vector<bool> &buffer) {
  std::bitset<N> bits = readNumberBitsFromFile<N>(inputFile, buffer);

  return (T)bits.to_ulong();
}

#endif // BITS_FILE_HPP