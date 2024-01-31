#include <bitset>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

const std::bitset<4> zero{0b0000};

const std::vector<std::bitset<4>> syndromes = {
    std::bitset<4>(0b1100), std::bitset<4>(0b1010), std::bitset<4>(0b1101),
    std::bitset<4>(0b1110), std::bitset<4>(0b1111), std::bitset<4>(0b1011),
    std::bitset<4>(0b1001), std::bitset<4>(0b1000),
};

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <input file> <output file>"
              << std::endl;
    return 1;
  }

  std::ifstream input(argv[1], std::ios::binary);
  std::ofstream output(argv[2], std::ios::binary);

  if (!input.is_open()) {
    std::cout << "Error opening input file" << std::endl;
    return 1;
  }

  if (!output.is_open()) {
    std::cout << "Error opening output file" << std::endl;
    return 1;
  }

  size_t blocksCounter{0};
  size_t parityErrors{0};

  while (!input.eof()) {
    std::bitset<8> decodedBits;
    for (size_t k = 0; k < 2; k++) {
      uint8_t byteValue{0};
      input.read(reinterpret_cast<char*>(&byteValue), sizeof(byteValue));
      if (input.eof()) {
        break;
      }

      std::bitset<8> bits(byteValue);
      std::bitset<4> syndrome;

      syndrome[0] = bits[2] ^ bits[4] ^ bits[5] ^ bits[6];
      syndrome[1] = bits[1] ^ bits[3] ^ bits[4] ^ bits[5];
      syndrome[2] = bits[0] ^ bits[2] ^ bits[3] ^ bits[4];
      syndrome[3] = bits[0] ^ bits[1] ^ bits[2] ^ bits[3] ^ bits[4] ^ bits[5] ^
                    bits[6] ^ bits[7];

      blocksCounter++;

      if (syndrome != zero) {
        bool foundSyndrome{false};
        for (size_t i = 0; i < syndromes.size(); i++) {
          if (syndrome == syndromes[i]) {
            bits.flip(i);
            foundSyndrome = true;
            break;
          }
        }

        if (!foundSyndrome) {
          parityErrors++;
        }
      }

      decodedBits[4 * k + 0] = bits[0];
      decodedBits[4 * k + 1] = bits[1] ^ bits[0];
      decodedBits[4 * k + 2] = bits[5];
      decodedBits[4 * k + 3] = bits[6];
    }

    uint8_t decodedByteValue = static_cast<uint8_t>(decodedBits.to_ulong());
    output.write(reinterpret_cast<char*>(&decodedByteValue),
                 sizeof(decodedByteValue));
  }

  std::cout << static_cast<float>(parityErrors) / blocksCounter * 100.0
            << std::endl;

  return 0;
}