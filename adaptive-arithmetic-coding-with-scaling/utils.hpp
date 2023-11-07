#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <cmath>

typedef uint8_t character_type;
typedef uint32_t bytes_number_type;
typedef uint64_t calc_type;

constexpr size_t ALPHABET_SIZE = 256;
constexpr size_t CALC_BITS_NUM = 32;
constexpr bytes_number_type BLOCK_SIZE = 256;

calc_type MAX_CALC_VALUE = (static_cast<calc_type>(1) << CALC_BITS_NUM) - 1;
calc_type ONE_HALF_MAX_CALC_VALUE = (static_cast<calc_type>(1) << (CALC_BITS_NUM - 1));
calc_type ONE_FOURTH_MAX_CALC_VALUE = (static_cast<calc_type>(1) << (CALC_BITS_NUM - 2));
calc_type THREE_FOURTHS_MAX_CALC_VALUE = static_cast<calc_type>(3) * ONE_FOURTH_MAX_CALC_VALUE;

typedef struct FileInfo {
    bytes_number_type readBytesCounter;
    bytes_number_type charactersCounter[ALPHABET_SIZE + 1];
    // charactersPartialSums[k] = sum from i = 0 to (k-1) charactersCounter[i]
    bytes_number_type charactersPartialSums[ALPHABET_SIZE + 1];
} FileInfo;


inline bytes_number_type getFileSize(std::ofstream &file) {
    file.seekp(0, std::ios::end);
    size_t fileSize = static_cast<bytes_number_type>(file.tellp());
    file.seekp(0, std::ios::beg);

    return fileSize;
}

inline bytes_number_type getFileSize(std::ifstream &file) {
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<bytes_number_type>(file.tellg());
    file.seekg(0, std::ios::beg);

    return fileSize;
}

template<typename T>
inline void writeToFile(std::ofstream &file, T value) {
    constexpr size_t bytesNum{sizeof(T)};
    character_type bytes[bytesNum];

    for (size_t byteNum = 0; byteNum < bytesNum; byteNum++) {
        bytes[byteNum] = static_cast<character_type>(value >> (8 * (bytesNum - byteNum - 1)));
    }

    file.write((char *) bytes, bytesNum);
}

template<typename T>
inline T readFromFile(std::ifstream &file) {
    constexpr size_t bytesNum{sizeof(T)};
    character_type bytes[bytesNum];

    file.read((char *) bytes, bytesNum);

    T value{0};
    for (size_t byteNum = 0; byteNum < bytesNum; byteNum++) {
        value |= (bytes[byteNum] << (8 * (bytesNum - byteNum - 1)));
    }

    return value;
}

inline void writeBitsToFile(std::ofstream &file, std::vector<bool> &bits, bool fillWithZeros) {
    if (fillWithZeros && bits.size() % 8 != 0) {
        while (bits.size() % 8 != 0) {
            bits.push_back(false);
        }
    }

    while (bits.size() >= 8) {
        uint8_t byteValue{0};
        for (size_t bitIndex = 0; bitIndex < 8; bitIndex++) {
            byteValue <<= 1;
            byteValue |= bits[bitIndex];
        }

        bits.erase(bits.begin(), bits.begin() + 8);

        writeToFile<uint8_t>(file, byteValue);
    }
}

inline void readBitsFromFile(std::ifstream &file, std::vector<bool> &bits) {
    const uint8_t byteValue{readFromFile<uint8_t>(file)};

    for (ssize_t bitIndex = 7; bitIndex >= 0; bitIndex--) {
        bool bit = (byteValue & (1 << bitIndex)) != 0;
        bits.push_back(bit);
    }
}

inline void initializeFileInfo(FileInfo& fileInfo) {
    fileInfo.readBytesCounter = 0;
    for(size_t character = 0; character <= ALPHABET_SIZE; character++) {
        fileInfo.charactersCounter[character] = 1;
        fileInfo.charactersPartialSums[character] = character;
    }
}

inline void calculatePartialSums(FileInfo& fileInfo) {
    bytes_number_type total{0};

    for (size_t character = 0; character <= ALPHABET_SIZE; character++) {
        fileInfo.charactersPartialSums[character] = total;
        total += fileInfo.charactersCounter[character];
    }
}

inline double calculateEntropy(const FileInfo& fileInfo) {
    double entropy = 0.0;

    for(size_t charCode = 0; charCode < ALPHABET_SIZE; charCode++) {
        if(fileInfo.charactersCounter[charCode] > 0) {
            entropy += -1.0 * fileInfo.charactersCounter[charCode] * log2(fileInfo.charactersCounter[charCode]);
        }
    }

    entropy /= fileInfo.readBytesCounter;
    return entropy + log2(fileInfo.readBytesCounter);
}

#endif // UTILS_HPP