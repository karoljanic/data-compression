#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <fstream>
#include <cstdint> 

constexpr size_t ALPHABET_SIZE = 256;   
constexpr size_t BLOCK_SIZE = 256;      
constexpr size_t CALC_BITS_NUM = 32;    // [1, 63]

typedef uint32_t file_size_t;
typedef uint64_t calc_size_t;

typedef struct FileInfo {
    file_size_t readBytesCounter;
    file_size_t charactersCounter[ALPHABET_SIZE];
    // charactersPartialSums[k] = sum from i = 0 to (k-1) charactersCounter[i]
    file_size_t charactersPartialSums[ALPHABET_SIZE + 1];
} FileInfo;


inline file_size_t getFileSize(std::ofstream& file) {
    file.seekp(0, std::ios::end);
    size_t fileSize = static_cast<file_size_t>(file.tellp());
    file.seekp(0, std::ios::beg);

    return fileSize;
}

inline file_size_t getFileSize(std::ifstream& file) {
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<file_size_t>(file.tellg());
    file.seekg(0, std::ios::beg);

    return fileSize;
}

template<typename T>
inline void writeToFile(std::ofstream& file, T value) {
    constexpr size_t bytesNum{sizeof(T)};
    uint8_t bytes[bytesNum];

    for(size_t byteNum = 0; byteNum < bytesNum; byteNum++) {
        bytes[byteNum] = static_cast<uint8_t>(value >> (8 * (bytesNum - byteNum - 1)));
    }
    
    file.write((char*)bytes, bytesNum);
}

template<typename T>
inline T readFromFile(std::ifstream& file) {
    constexpr size_t bytesNum{sizeof(T)};
    uint8_t bytes[bytesNum];

    file.read((char*)bytes, bytesNum);

    T value{0};
    for(size_t byteNum = 0; byteNum < bytesNum; byteNum++) {
        value |= (bytes[byteNum] << (8 * (bytesNum - byteNum - 1)));
    }

    return value;
}

void updateCharactersPartialSums(FileInfo& fileInfo) {
    file_size_t total{0};
    for(size_t i = 0; i <= ALPHABET_SIZE; i++) {
        fileInfo.charactersPartialSums[i] = total;
        total += fileInfo.charactersCounter[i];
    }
}

#endif // UTILS_HPP