#include "utils.hpp"

void flushPendingBitsAndClear(bool bit, bytes_number_type& pendingBitsCounter,
                              std::vector<bool>& bitsBuffer) {
  bitsBuffer.push_back(bit);
  for (bytes_number_type bitNum = 0; bitNum < pendingBitsCounter; bitNum++) {
    bitsBuffer.push_back(!bit);
  }

  pendingBitsCounter = 0;
}

FileInfo compress(std::ifstream& inputFile, std::ofstream& outputFile) {
  FileInfo fileInfo{};
  initializeFileInfo(fileInfo);

  std::vector<bool> bitsBuffer{};
  const bytes_number_type allBytesNumber{getFileSize(inputFile)};
  writeToFile<bytes_number_type>(outputFile, allBytesNumber);

  for (bytes_number_type byteNum = 0;
       byteNum < std::min(BLOCK_SIZE, allBytesNumber); byteNum++) {
    auto character = readFromFile<character_type>(inputFile);
    fileInfo.readBytesCounter++;
    fileInfo.charactersCounter[character]++;

    writeToFile<character_type>(outputFile, character);
  }

  bytes_number_type pendingBitsCounter{0};
  calc_type lowerIntervalBound{0};
  calc_type upperIntervalBound{MAX_CALC_VALUE};

  while (fileInfo.readBytesCounter < allBytesNumber) {
    auto character = readFromFile<character_type>(inputFile);
    fileInfo.readBytesCounter++;
    fileInfo.charactersCounter[character]++;

    calc_type intervalRange = upperIntervalBound - lowerIntervalBound + 1;
    upperIntervalBound =
        lowerIntervalBound +
        (intervalRange * fileInfo.charactersPartialSums[character + 1] /
         fileInfo.charactersPartialSums[ALPHABET_SIZE]) -
        1;
    lowerIntervalBound =
        lowerIntervalBound +
        (intervalRange * fileInfo.charactersPartialSums[character] /
         fileInfo.charactersPartialSums[ALPHABET_SIZE]);

    while (true) {
      if (upperIntervalBound < ONE_HALF_MAX_CALC_VALUE) {
        flushPendingBitsAndClear(false, pendingBitsCounter, bitsBuffer);
      } else if (lowerIntervalBound >= ONE_HALF_MAX_CALC_VALUE) {
        flushPendingBitsAndClear(true, pendingBitsCounter, bitsBuffer);
      } else if (lowerIntervalBound >= ONE_FOURTH_MAX_CALC_VALUE &&
                 upperIntervalBound < THREE_FOURTHS_MAX_CALC_VALUE) {
        pendingBitsCounter++;
        lowerIntervalBound -= ONE_FOURTH_MAX_CALC_VALUE;
        upperIntervalBound -= ONE_FOURTH_MAX_CALC_VALUE;
      } else {
        writeBitsToFile(outputFile, bitsBuffer, false);
        break;
      }

      upperIntervalBound <<= 1;
      upperIntervalBound++;
      upperIntervalBound &= MAX_CALC_VALUE;

      lowerIntervalBound <<= 1;
      lowerIntervalBound &= MAX_CALC_VALUE;
    }

    if (fileInfo.readBytesCounter % BLOCK_SIZE == 0) {
      calculatePartialSums(fileInfo);
    }
  }

  pendingBitsCounter++;
  if (lowerIntervalBound < ONE_FOURTH_MAX_CALC_VALUE) {
    flushPendingBitsAndClear(false, pendingBitsCounter, bitsBuffer);
  } else {
    flushPendingBitsAndClear(true, pendingBitsCounter, bitsBuffer);
  }

  writeBitsToFile(outputFile, bitsBuffer, true);

  return fileInfo;
}

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Bad input!. Correct input is: ./encoder <file to encode> "
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

  const auto inputFileInfo = compress(inputFile, outputFile);
  const auto outputFileSize = getFileSize(outputFile);
  const auto entropy = calculateEntropy(inputFileInfo);

  std::cout << "Entropy: " << entropy << std::endl
            << "Average code length: "
            << static_cast<double>(8 * outputFileSize) /
                   static_cast<double>(inputFileInfo.readBytesCounter)
            << std::endl
            << "Compression ratio: "
            << static_cast<double>(inputFileInfo.readBytesCounter) /
                   static_cast<double>(outputFileSize)
            << std::endl;
}
