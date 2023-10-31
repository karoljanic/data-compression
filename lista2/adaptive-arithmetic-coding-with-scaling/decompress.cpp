#include "utils.hpp"

void decode(std::ifstream& inputFile, std::ofstream& outputFile) {
    FileInfo fileInfo{};
    
    file_size_t bytesNum = readFromFile<file_size_t>(inputFile);

    calc_size_t intervalLowerBound{static_cast<calc_size_t>(0)};
    calc_size_t intervalUpperBound{static_cast<calc_size_t>(static_cast<calc_size_t>((1UL << CALC_BITS_NUM)) - 1)}; 
    calc_size_t buffer{0};
    calc_size_t underflowsCounter{0};

    calc_size_t fullRange = 1UL << CALC_BITS_NUM;
	calc_size_t halfRange = fullRange >> 1;  // Non-zero
	calc_size_t quarterRange = halfRange >> 1;  // Can be zero
	calc_size_t stateMask = (1UL << CALC_BITS_NUM) - 1;

    uint8_t character;
    for(file_size_t byteNum = 0; byteNum < bytesNum; byteNum++) {
        character = readFromFile<uint8_t>(inputFile);
        fileInfo.readBytesCounter++;
        
        if(fileInfo.readBytesCounter % BLOCK_SIZE == 0) {
            updateCharactersPartialSums(fileInfo);
        }

        if(fileInfo.readBytesCounter < BLOCK_SIZE) { // do not decode
            writeToFile<uint8_t>(outputFile, character);
            fileInfo.charactersCounter[character]++;
        }
        else { // decode
            calc_size_t currIntervalRange{intervalUpperBound - intervalLowerBound + 1};
            calc_size_t offset{buffer - intervalLowerBound};
	        calc_size_t value{((offset + 1) * fileInfo.charactersPartialSums[ALPHABET_SIZE] - 1) / currIntervalRange};

            std::cout << currIntervalRange << " " << offset << " " << value << std::endl;

            uint32_t start{0};
            uint32_t end{ALPHABET_SIZE};
            while(end - start > 1) {
                uint32_t middle = (start + end) >> 1;
                if(fileInfo.charactersPartialSums[middle] > value) {
                    end = middle;
                }
                else {
                    start = middle;
                }
            }
            if (start + 1 != end) {
                std::cout << "XD\n";
            }
            
            uint32_t character = static_cast<uint8_t>(start);

            fileInfo.charactersCounter[character]++;
            writeToFile<uint8_t>(outputFile, character);
        }
    }
}


int main(int argc, char** argv) {
    if(argc != 3) {
        std::cout << "Bad input!. Correct input is: ./decoder <compressed file> <output file>\n";

        return -1;
    }

    const std::string inputFileName{argv[1]};
    const std::string outputFileName{argv[2]};

    std::ifstream inputFile{inputFileName, std::ios::binary};
    std::ofstream outputFile{outputFileName, std::ios::binary};

    if(inputFile.fail()) {
        std::cout << "Cannot open file " << inputFileName << '\n';
        return -1;
    }

    if(outputFile.fail()) {
        std::cout << "Cannot create file " << outputFileName << '\n';
        return -1;
    }

    decode(inputFile, outputFile);
}
