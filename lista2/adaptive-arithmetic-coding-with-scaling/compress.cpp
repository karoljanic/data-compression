#include "utils.hpp"
#include <math.h>

FileInfo encode(std::ifstream& inputFile, std::ofstream& outputFile) {
    FileInfo fileInfo{};

    file_size_t bytesNum{getFileSize(inputFile)};
    writeToFile<file_size_t>(outputFile, bytesNum);

    calc_size_t intervalLowerBound{static_cast<calc_size_t>(0)};
    calc_size_t intervalUpperBound{static_cast<calc_size_t>(static_cast<calc_size_t>((1UL << CALC_BITS_NUM)) - 1)}; 
    calc_size_t underflowsCounter{0};

    calc_size_t fullRange = 1UL << CALC_BITS_NUM;
	calc_size_t halfRange = fullRange >> 1;  // Non-zero
	calc_size_t quarterRange = halfRange >> 1;  // Can be zero
	calc_size_t stateMask = (1UL << CALC_BITS_NUM) - 1;

    uint8_t character;
    for(file_size_t byteNum = 0; byteNum < bytesNum; byteNum++) {
        character = readFromFile<uint8_t>(inputFile);
        fileInfo.readBytesCounter++;
        fileInfo.charactersCounter[character]++;

        if(fileInfo.readBytesCounter % BLOCK_SIZE == 0) { // calculate new characters distribution
            updateCharactersPartialSums(fileInfo);
            std::cout << "Updated partial sums\n";
        }

        if(fileInfo.readBytesCounter < BLOCK_SIZE) { // do not encode
            writeToFile<uint8_t>(outputFile, character);
        }
        else { // encode
            calc_size_t currIntervalRange{intervalUpperBound - intervalLowerBound + 1};
            calc_size_t newIntervalLowerBound = intervalLowerBound + static_cast<uint32_t>(currIntervalRange) * static_cast<uint32_t>(fileInfo.charactersPartialSums[character]) / static_cast<uint32_t>(fileInfo.charactersPartialSums[ALPHABET_SIZE]);
            calc_size_t newIntervalUpperBound = intervalUpperBound + static_cast<uint32_t>(currIntervalRange) * static_cast<uint32_t>(fileInfo.charactersPartialSums[character + 1]) / static_cast<uint32_t>(fileInfo.charactersPartialSums[ALPHABET_SIZE]) - 1;
            intervalLowerBound = newIntervalLowerBound & stateMask;
            intervalUpperBound = newIntervalUpperBound & stateMask;
 
            // while intervalLowerBound and intervalUpperBound have the same MSB shift them
            // after this intervalLowerBound MSB must be '0' and intervalUpperBound MST must be '1'
            while(((intervalLowerBound ^ intervalUpperBound) & halfRange) == 0) {
                uint8_t bit{static_cast<uint8_t>(intervalLowerBound >> (CALC_BITS_NUM - 1))};
                writeToFile<uint8_t>(outputFile, bit);
                
                // write out underflow bits
                while(underflowsCounter > 0) {
                    writeToFile<uint8_t>(outputFile, bit ^ 1);
                    underflowsCounter--;
                }

                intervalLowerBound = ((intervalLowerBound  << 1) & stateMask);
                intervalUpperBound = ((intervalUpperBound << 1) & stateMask) | 1;
            }
            
            // while intervalLowerBound starts with '01' and intervalUpperBound starts with '10'
            // change '01' to '0' and '10' to '1'
            while((intervalLowerBound & ~intervalUpperBound & quarterRange) != 0) {
                underflowsCounter++;
                intervalLowerBound = (intervalLowerBound << 1) ^ halfRange;
                intervalUpperBound = ((intervalUpperBound ^ halfRange) << 1) | halfRange | 1;
            }
        }
    }

    writeToFile<uint8_t>(outputFile, 1);

    return fileInfo;
}


int main(int argc, char* argv[]) {
    if(argc != 3) {
        std::cout << "Bad input!. Correct input is: ./encoder <file to compress> <output file>\n";
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

    const FileInfo inputFileInfo = encode(inputFile, outputFile);

    file_size_t outputFileSize{getFileSize(outputFile)};
    std::cout << "Compression ratio: " << static_cast<double>(inputFileInfo.readBytesCounter) / static_cast<double>(outputFileSize) << std::endl;
}
