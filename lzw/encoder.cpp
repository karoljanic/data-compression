#include "universal-encoding.hpp"
#include "utils.hpp"

std::vector<size_t> fibb = {1, 1};

void writeNumberToFile(const size_t num, const std::string& universalEncoding,
                       std::vector<bool>& bits, std::ofstream& outputFile,
                       bool flushData) {

  if (universalEncoding == "fibonacci") {
    encode_fibonacci(num + 1, bits, fibb);
  } else if (universalEncoding == "ellias_gamma") {
    encode_ellias_gamma(num + 1, bits);
  } else if (universalEncoding == "ellias_delta") {
    encode_ellias_delta(num + 1, bits);
  } else {
    encode_ellias_omega(num + 1, bits);
  }

  writeBitsToFile(outputFile, bits, flushData);
}

FileInfo encode(std::ifstream& inputFile, std::ofstream& outputFile,
                const std::string& universalEncoding) {
  FileInfo fileInfo{};
  std::vector<bool> bits{};

  std::unordered_map<std::string, size_t> dictionary;
  size_t nextPrefixCode{1};
  for (size_t i = 0; i < ALPHABET_SIZE; i++) {
    dictionary[std::string(1, static_cast<char>(i))] = nextPrefixCode;
    nextPrefixCode++;
  }

  std::string prefix{""};
  char currentChar;
  while (inputFile.get(currentChar)) {
    fileInfo.charactersCounter[static_cast<uint8_t>(currentChar)]++;
    fileInfo.totalCharsCounter++;

    const std::string newPrefix{prefix + currentChar};
    if (dictionary.find(newPrefix) != dictionary.end()) {
      prefix.push_back(currentChar);
    } else {
      writeNumberToFile(dictionary.at(prefix), universalEncoding, bits,
                        outputFile, false);
      dictionary[newPrefix] = nextPrefixCode;
      nextPrefixCode++;
      prefix = currentChar;
    }
  }

  writeNumberToFile(dictionary.at(prefix), universalEncoding, bits, outputFile,
                    false);

  writeNumberToFile(0, universalEncoding, bits, outputFile, true);

  return fileInfo;
}

int main(int argc, char* argv[]) {
  if (argc < 3) {
    std::cout << "Bad input!. Correct input is: ./encoder <file to encode> "
                 "<output file> <universal-encoding-name>=ellias_omega\n";
    return -1;
  }

  const std::string inputFileName{argv[1]};
  const std::string outputFileName{argv[2]};
  const std::string universalEncoding = argc == 4 ? argv[3] : "ellias_omega";

  if (universalEncoding != "ellias_omega" &&
      universalEncoding != "ellias_delta" &&
      universalEncoding != "ellias_gamma" && universalEncoding != "fibonacci") {
    std::cout << "Unsupported universal encoding " << universalEncoding << '\n';
    return -1;
  }

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

  const auto inputFileInfo = encode(inputFile, outputFile, universalEncoding);
  const auto inputFileEntropy = calculateEntropy(inputFileInfo);

  std::ifstream outputFileIn{outputFileName, std::ios::binary};
  FileInfo outputFileInfo{};
  char currentChar;
  while (outputFileIn.get(currentChar)) {
    outputFileInfo.charactersCounter[static_cast<uint8_t>(currentChar)]++;
    outputFileInfo.totalCharsCounter++;
  }
  const auto outputFileEntropy = calculateEntropy(outputFileInfo);

  std::cout << "Input file size: " << inputFileInfo.totalCharsCounter
            << std::endl
            << "Output file size: " << outputFileInfo.totalCharsCounter
            << std::endl
            << "Input File entropy: " << inputFileEntropy << std::endl
            << "Output File entropy: " << outputFileEntropy << std::endl
            << "Average code length: "
            << static_cast<double>(8 * outputFileInfo.totalCharsCounter) /
                   static_cast<double>(inputFileInfo.totalCharsCounter)
            << std::endl
            << "Compression ratio: "
            << static_cast<double>(inputFileInfo.totalCharsCounter) /
                   static_cast<double>(outputFileInfo.totalCharsCounter)
            << std::endl;
}
