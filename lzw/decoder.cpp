#include "universal-encoding.hpp"
#include "utils.hpp"

std::vector<size_t> fibb = {1, 1};

size_t readNumberFromBits(const std::string& universalEncoding,
                          std::vector<bool>& bits, std::ifstream& inputFile) {
  size_t num;
  if (universalEncoding == "fibonacci") {
    num = decode_fibonacci(bits, fibb, inputFile);
  } else if (universalEncoding == "ellias_gamma") {
    num = decode_ellias_gamma(bits, inputFile);
  } else if (universalEncoding == "ellias_delta") {
    num = decode_ellias_delta(bits, inputFile);
  } else {
    num = decode_ellias_omega(bits, inputFile);
  }

  return num - 1;
}

void decode(std::ifstream& inputFile, std::ofstream& outputFile,
            const std::string& universalEncoding) {

  std::unordered_map<size_t, std::string> dictionary;
  size_t nextPrefixCode{1};
  for (size_t i = 0; i < ALPHABET_SIZE; i++) {
    dictionary[nextPrefixCode] = std::string(1, static_cast<char>(i));
    nextPrefixCode++;
  }

  std::vector<bool> bits{};

  size_t code = readNumberFromBits(universalEncoding, bits, inputFile);
  std::string currentString = dictionary[code];
  outputFile << currentString;

  while (code != 0) {
    size_t nextCode = readNumberFromBits(universalEncoding, bits, inputFile);
    if (nextCode != 0) {
      if (dictionary.find(nextCode) == dictionary.end()) {
        currentString = dictionary[code];
        currentString += currentString[0];
      } else {
        currentString = dictionary[nextCode];
      }

      outputFile << currentString;

      dictionary[nextPrefixCode] = dictionary[code] + currentString[0];
      nextPrefixCode++;
    }
    code = nextCode;
  }
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Bad input!. Correct input is: ./decoder <encoded file> "
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

  decode(inputFile, outputFile, universalEncoding);
}
