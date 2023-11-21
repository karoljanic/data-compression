#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <cmath>
#include <unordered_map>

constexpr size_t ALPHABET_SIZE = 256;

typedef struct FileInfo {
    size_t totalCharsCounter;
    size_t charactersCounter[ALPHABET_SIZE + 1];
} FileInfo;


inline double calculateEntropy(const FileInfo& fileInfo) {
    double entropy = 0.0;

    for(size_t charCode = 0; charCode < ALPHABET_SIZE; charCode++) {
        if(fileInfo.charactersCounter[charCode] > 0) {
            entropy += -1.0 * fileInfo.charactersCounter[charCode] * log2(fileInfo.charactersCounter[charCode]);
        }
    }

    entropy /= fileInfo.totalCharsCounter;
    return entropy + log2(fileInfo.totalCharsCounter);
}

#endif // UTILS_HPP
