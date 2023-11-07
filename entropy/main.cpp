#include <iostream>
#include <fstream>
#include <vector>
#include <math.h>

constexpr size_t MAX_CHAR_CODE = 256;

double calculateEntropy(size_t totalCharsCount, const std::vector<size_t>& singleCharsCounter) {
    // let S = sum of |Ai| for i from 0 to N
    // entropy = sum for i from 0 to N: P(Ai) * I(Ai) = 
    // sum for i from 0 to N: P(Ai) * log2(1/P(Ai)) = 
    // sum for i from 0 to N: |Ai| / S * -1 * log2(|Ai| / S) = 
    // sum for i from 0 to N: |Ai| / S * -1 * (log2(|Ai|) - log2(S)) = 
    // (1 / S) * (sum from i = 0 to N: -1 * |Ai| * log2(|Ai|)) + (1 / S) * (sum from i = 0 to N: |Ai| * log2(S)) = 
    // (1 / S) * (sum from i = 0 to N: -1 * |Ai| * log2(|Ai|)) + (1 / S) * log2(S) * (sum from i = 0 to N: |Ai|) = 
    // (1 / S) * (sum from i = 0 to N: -1 * |Ai| * log2(|Ai|)) + (1 / S) * log2(S) * S = 
    // (1 / S) * (sum from i = 0 to N: -1 * |Ai| * log2(|Ai|)) + log2(S)

    double entropy = 0.0;

    for(size_t charCode = 0; charCode < MAX_CHAR_CODE; charCode++) {
      if(singleCharsCounter[charCode] > 0) {
        entropy += -1.0 * singleCharsCounter[charCode] * log2(singleCharsCounter[charCode]);
      }
    }

    entropy /= totalCharsCount;
    return entropy + log2(totalCharsCount);
}

double calculateConditionEntropy(size_t totalCharsCount, const std::vector<size_t>& singleCharsCounter, const std::vector<std::vector<size_t>>& doubleCharsCounter) {
    // let S = sum of |Ai| for i from 0 to N
    // conditionalEntropy = sum for i from 0 to N: P(Ai) * (sum for j from 0 to N: P(Aj|Ai) * I(Aj|Ai)) = 
    // sum for i from 0 to N: P(Ai) * (sum for j from 0 to N: P(Aj&Ai) / P(Ai) * log2(1/P(Ai))) = 
    // sum for i from 0 to N: P(Ai) / P(Ai) * (sum for j from 0 to N: -1 * P(Aj&Ai) * log2(P(Aj&Ai)))) = 
    // sum for i from 0 to N: (sum for j from 0 to N: -1 * |Aj&Ai| / S * log2(|Aj&Ai| / |Ai|)) = 
    // (1 / S) * sum for i from 0 to N: (sum for j from 0 to N: -1 * |Aj&Ai| * (log2(|Aj&Ai|) - log2(|Ai|)))
    
    double conditionalEntropy = 0.0;
    for(size_t firstCharCode = 0; firstCharCode < MAX_CHAR_CODE; firstCharCode++) {
      for(size_t secondCharCode = 0; secondCharCode < MAX_CHAR_CODE; secondCharCode++) {
        if(singleCharsCounter[firstCharCode] > 0 && doubleCharsCounter[firstCharCode][secondCharCode] > 0)
          conditionalEntropy += -1.0 * doubleCharsCounter[firstCharCode][secondCharCode] * (log2(doubleCharsCounter[firstCharCode][secondCharCode]) - log2(singleCharsCounter[firstCharCode]));
      }
    }

    conditionalEntropy /= totalCharsCount;
    return conditionalEntropy;
}

void analyze(const std::string& fileName) {
    std::ifstream file{fileName};
    
    size_t totalCharsCounter = 0;
    std::vector<size_t> singleCharsCounter(MAX_CHAR_CODE, 0);
    std::vector<std::vector<size_t>> doubleCharsCounter(MAX_CHAR_CODE, std::vector<size_t>(MAX_CHAR_CODE, 0));

    size_t currentChar = 0;
    size_t previousChar = 0;
    while (!file.eof()) {
      singleCharsCounter[currentChar]++;
      doubleCharsCounter[previousChar][currentChar]++;

      previousChar = currentChar;
      currentChar = file.get();
      totalCharsCounter++;
    }

    std::cout << "Entropy: " << calculateEntropy(totalCharsCounter, singleCharsCounter) << std::endl
        << "Conditional Entropy: " << calculateConditionEntropy(totalCharsCounter, singleCharsCounter, doubleCharsCounter) << std::endl;

    file.close();
}

int main(int argc, char** argv) {
    if(argc != 2) {
        std::cout << "Incorrect input. Enter file name!\n";

        return -1;
    }

    std::string fileName{argv[1]};
    analyze(fileName);

    return 0;
}   
