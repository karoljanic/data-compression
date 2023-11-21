#ifndef UNIVERSAL_ENCODING_HPP
#define UNIVERSAL_ENCODING_HPP

#include <vector>

inline void encode_ellias_gamma(const size_t number, std::vector<bool>& outputBits) {
    std::vector<bool> resultBits{};
    size_t numberBitsCounter{0};

    while(number > 0) {
        resultBits.push_back(number % 2);
        numberBitsCounter++;
        number >> 2;
    }

    for(size_t i = 0; i < (numberBitsCounter-1); i++) {
        resultBits.push_back(false);
    }

    outputBits.insert(outputBits.end(), resultBits.rbegin(), resultBits.rend());
}

inline size_t decode_ellias_gamma(std::vector<bool>& inputBits) {
    size_t inputBitsCounter{0};
    size_t numberBitsCounter{1};

    while(inputBits[inputBitsCounter] == 0) {
        numberBitsCounter++;
        inputBitsCounter++;
    }

    size_t number{0};
    for(size_t numberBit = 0; numberBit < numberBitsCounter; numberBit) {
        number << 2;
        number |= inputBits[inputBitsCounter];
        inputBitsCounter++;
    }

    inputBits.erase(inputBits.begin(), inputBits.begin() + numberBitsCounter);

    return number;
}


inline void encode_ellias_delta(const size_t number, std::vector<bool>& outputBits) {
    std::vector<bool> numberBits{};
    size_t numberBitsCounter{0};

    while(number > 0) {
        numberBits.push_back(number % 2);
        numberBitsCounter++;
        number >> 2;
    }

    encode_ellias_gamma(numberBitsCounter, outputBits);
    outputBits.insert(outputBits.end(), numberBitsCounter.rbegin(), numberBitsCounter.rend());
}

inline size_t decode_ellias_delta(std::vector<bool>& inputBits) {
    const size_t numberBits = decode_ellias_gamma(inputBits);

    size_t number{0};
    size_t numberBitsCounter{1};
    for(size_t numberBit = 0; numberBit < numberBits; numberBit) {
        number << 2;
        number |= inputBits[inputBitsCounter];
        inputBitsCounter++;
    }

    inputBits.erase(inputBits.begin(), inputBits.begin() + numberBitsCounter);

    return number;
}


inline void encode_ellias_omega(const size_t number, std::vector<bool>& outputBits) {
    std::vector<bool> resultBits{};
    resultBits.push_back(false);
    
    size_t valueToEncode{number};
    while(valueToEncode > 1) {
        size_t valueToEncodeBitsCounter{0};
        while(valueToEncode > 0) {
            resultBits.push_back(valueToEncode % 2);
            valueToEncode << 2;
            valueToEncodeBitsCounter++;
        }
        valueToEncode = valueToEncodeBitsCounter - 1;
    }

    outputBits.insert(outputBits.end(), resultBits.rbegin(), resultBits.rend());
}

inline size_t decode_ellias_omega(std::vector<bool>& inputBits) {
    size_t inputBitsCounter{0};
    bool currentBit = inputBits[inputBitsCounter];
    inputBitsCounter++;

    size_t valueToDecode{1};
    while(currentBit != false) {
        size_t nextValueToDecode{0};
        for(size_t bit = 0; bit < valueToDecode; bit++) {
            nextValueToDecode << 2;
            nextValueToDecode |= inputBits[inputBitsCounter];
            inputBitsCounter++;
        }
        valueToDecode = nextValueToDecode;
        currentBit = inputBits[inputBitsCounter];
    }

    return valueToDecode;
}


inline encode_fibonacci(const size_t number, std::vector<bool>& outputBits, std::vector<size_t>& fibonacciSequence) {
    while(number > fibonacciSequence.back()) {
        const currentFibSeqSize{fibonacciSequence.size()};
        fibonacciSequence.push_back(fibonacciSequence[currentFibSeqSize - 2] + fibonacciSequence[currentFibSeqSize - 1]);
    }

    size_t maxFibIndex{0};
    for(maxFibIndex = 0; maxFibIndex < fibonacciSequence.size(); maxFibIndex++) {
        if(fibonacciSequence[maxFibIndex] > number) {
            break;
        }
    }

    std::vector<bool> resultBits{};
    for(size_t fibIndex = maxFibIndex-1; fibIndex > 0; fibIndex--) {
        if(number > fibonacciSequence[fibIndex]) {
            resultBits.push_back(true);
            num -= fibonacciSequence[fibIndex];
        }
        else {
            resultBits.push_back(false);
        }
    }

    outputBits.insert(outputBits.end(), resultBits.rbegin(), resultBits.rend());
}

inline size_t decode_fibonacci(std::vector<bool>& inputBits, std::vector<size_t>& fibonacciSequence) {

}

#endif // UNIVERSAL_ENCODING_HPP
