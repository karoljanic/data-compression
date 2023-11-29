#include "universal-encoding.hpp"
#include "utils.hpp"

std::vector<size_t> fibb = {1, 1};

size_t readNumberFromBits(const std::string& universalEncoding,
                          std::vector<bool>& bits, std::ofstream& outputFile) {
  size_t num;
  if (universalEncoding == "fibonacci") {
    num = decode_fibonacci(bits, fibb);
  } else if (universalEncoding == "ellias_gamma") {
    num = decode_ellias_gamma(bits);
  } else if (universalEncoding == "ellias_delta") {
    num = decode_ellias_delta(bits);
  } else {
    num = decode_ellias_omega(bits);
  }

  std::cout << "Odczytaj " << num << std::endl;
}

FileInfo decode(std::ifstream& inputFile, std::ofstream& outputFile,
                const std::string& universalEncoding) {
  //   FileInfo fileInfo{};

  //   //   std::unordered_map<std::string, size_t> dictionary;
  //   //   size_t nextPrefixCode{0};
  //   //   for (size_t i = 0; i < ALPHABET_SIZE; i++) {
  //   //     dictionary[std::string(1, static_cast<char>(i))] = nextPrefixCode;
  //   //     nextPrefixCode++;
  //   //   }

  //   std::vector<bool> bits{};
  //   for (int i = 0; i < 9; i++) {
  //     //readBitsFromFile(inputFile, bits);
  //   }

  //   std::cout << "Wczytane " << bits.size() << std::endl;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    std::cout << "Bad input!. Correct input is: ./decoder <encoded file> "
                 "<output file> --<universal-encoding-name>=ellias_omega\n";

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

  const auto outputFileInfo = decode(inputFile, outputFile, universalEncoding);
}
