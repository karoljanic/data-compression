#include "utils.hpp"

character_type findChar(calc_type scaled_value, const FileInfo& fileInfo) {
  for (size_t character = 0; character < ALPHABET_SIZE; character++)
    if (scaled_value < fileInfo.charactersPartialSums[character + 1]) {
      return character;
    }

  return 0;
}

FileInfo decode(std::ifstream& inputFile, std::ofstream& outputFile) {
  FileInfo fileInfo;
  initializeFileInfo(fileInfo);

  std::vector<bool> bitsBuffer{};
  bytes_number_type allBytesNumber{readFromFile<bytes_number_type>(inputFile)};

  for (bytes_number_type byteNum = 0;
       byteNum < std::min(BLOCK_SIZE, allBytesNumber); byteNum++) {
    auto character = readFromFile<character_type>(inputFile);
    fileInfo.readBytesCounter++;
    fileInfo.charactersCounter[character]++;

    writeToFile<character_type>(outputFile, character);
  }

  calc_type upperIntervalBound = MAX_CALC_VALUE;
  calc_type lowerIntervalBound = 0;
  calc_type value = 0;

  for (size_t i = 0; i < CALC_BITS_NUM; i++) {
    if (bitsBuffer.empty()) {
      readBitsFromFile(inputFile, bitsBuffer);
    }
    value <<= 1;
    value += bitsBuffer.front() ? 1 : 0;
    bitsBuffer.erase(bitsBuffer.begin());
  }

  while (fileInfo.readBytesCounter < allBytesNumber) {
    calc_type intervalRange = upperIntervalBound - lowerIntervalBound + 1;
    calc_type scaledValue = ((value - lowerIntervalBound + 1) *
                                 fileInfo.charactersPartialSums[ALPHABET_SIZE] -
                             1) /
                            intervalRange;
    character_type character = findChar(scaledValue, fileInfo);

    writeToFile<character_type>(outputFile, character);
    fileInfo.readBytesCounter++;
    fileInfo.charactersCounter[character]++;

    upperIntervalBound =
        lowerIntervalBound +
        (intervalRange * fileInfo.charactersPartialSums[character + 1]) /
            fileInfo.charactersPartialSums[ALPHABET_SIZE] -
        1;
    lowerIntervalBound =
        lowerIntervalBound +
        (intervalRange * fileInfo.charactersPartialSums[character]) /
            fileInfo.charactersPartialSums[ALPHABET_SIZE];

    while (true) {
      if (upperIntervalBound < ONE_HALF_MAX_CALC_VALUE) {

      } else if (lowerIntervalBound >= ONE_HALF_MAX_CALC_VALUE) {
        value -= ONE_HALF_MAX_CALC_VALUE;
        lowerIntervalBound -= ONE_HALF_MAX_CALC_VALUE;
        upperIntervalBound -= ONE_HALF_MAX_CALC_VALUE;
      } else if (lowerIntervalBound >= ONE_FOURTH_MAX_CALC_VALUE &&
                 upperIntervalBound < THREE_FOURTHS_MAX_CALC_VALUE) {
        value -= ONE_FOURTH_MAX_CALC_VALUE;
        lowerIntervalBound -= ONE_FOURTH_MAX_CALC_VALUE;
        upperIntervalBound -= ONE_FOURTH_MAX_CALC_VALUE;
      } else {
        break;
      }

      lowerIntervalBound <<= 1;
      upperIntervalBound <<= 1;
      upperIntervalBound++;

      if (bitsBuffer.empty()) {
        readBitsFromFile(inputFile, bitsBuffer);
      }

      value <<= 1;
      value += bitsBuffer.front() ? 1 : 0;
      bitsBuffer.erase(bitsBuffer.begin());
    }

    if (fileInfo.readBytesCounter % BLOCK_SIZE == 0) {
      calculatePartialSums(fileInfo);
    }
  }

  return fileInfo;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    std::cout << "Bad input!. Correct input is: ./decoder <encoded file> "
                 "<output file>\n";

    return -1;
  }

  const std::string inputFileName{argv[1]};
  const std::string outputFileName{argv[2]};

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

  decode(inputFile, outputFile);
}
