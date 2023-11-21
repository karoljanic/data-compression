#include "utils.hpp"
#include "universal-encoding.hpp"

FileInfo encode(std::ifstream &inputFile, std::ofstream &outputFile, const std::string& universalEncoding) {
    FileInfo fileInfo{};

    std::unordered_map<std::string, size_t> dictionary;
    size_t nextPrefixCode{0};
    for(size_t i = 0; i < ALPHABET_SIZE; i++) {
        dictionary[std::string(1, static_cast<char>(i))] = nextPrefixCode;
        nextPrefixCode++;
    }

    std::string prefix{""};
    while (!inputFile.eof()) {
        const char currentChar = inputFile.get();
        fileInfo.charactersCounter[static_cast<size_t>(currentChar)]++;
        fileInfo.totalCharsCounter++;

        const std::string newPrefix{prefix + currentChar};
        if(dictionary.find(newPrefix) != dictionary.end()) {
            prefix.push_back(currentChar);
        }
        else {
            outputFile << dictionary.at(prefix) << std::endl;
            dictionary[newPrefix] = nextPrefixCode;
            nextPrefixCode++;
            prefix = currentChar;
        }
    }
    outputFile << dictionary.at(prefix) << std::endl;

    return fileInfo;
}


int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Bad input!. Correct input is: ./encoder <file to encode> <output file> --<universal-encoding-name>=ellias_omega\n";
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

    const auto inputFileInfo = encode(inputFile, outputFile, universalEncoding);
    // const auto outputFileSize = getFileSize(outputFile);
    // const auto entropy = calculateEntropy(inputFileInfo);

    // std::cout << "Entropy: " << entropy << std::endl << "Average code length: "
    //           << static_cast<double>(8 * outputFileSize) / static_cast<double>(inputFileInfo.readBytesCounter)
    //           << std::endl << "Compression ratio: " <<
    //           static_cast<double>(inputFileInfo.readBytesCounter) / static_cast<double>(outputFileSize) << std::endl;
}
