#include "utils.hpp"
#include "universal-encoding.hpp"

FileInfo decode(std::ifstream &inputFile, std::ofstream &outputFile, const std::string& universalEncoding) {
    FileInfo fileInfo{};

    std::unordered_map<std::string, size_t> dictionary;
    size_t nextPrefixCode{0};
    for(size_t i = 0; i < ALPHABET_SIZE; i++) {
        dictionary[std::string(1, static_cast<char>(i))] = nextPrefixCode;
        nextPrefixCode++;
    }

}

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cout << "Bad input!. Correct input is: ./decoder <encoded file> <output file> --<universal-encoding-name>=ellias_omega\n";

        return -1;
    }

    const std::string inputFileName{argv[1]};
    const std::string outputFileName{argv[2]};
    const std::string universalEncoding = argc == 4 ? argv[3] : "ellias_omega";

    if(universalEncoding != "ellias_omega" && universalEncoding != "ellias_delta" && universalEncoding != "ellias_gamma" && universalEncoding != "fibonacci") {
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
