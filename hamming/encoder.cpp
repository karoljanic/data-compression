#include <bitset>
#include <cstdint>
#include <fstream>
#include <iostream>

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

  while (!input.eof()) {
    uint8_t byteValue{0};
    input.read(reinterpret_cast<char*>(&byteValue), sizeof(byteValue));
    if (input.eof()) {
      break;
    }

    for (size_t k = 0; k < 2; k++) {
      std::bitset<8> bits(byteValue);
      std::bitset<8> encodedBits;
      encodedBits[0] = bits[4 * k + 0];
      encodedBits[1] = bits[4 * k + 0] ^ bits[4 * k + 1];
      encodedBits[2] = bits[4 * k + 1] ^ bits[4 * k + 2];
      encodedBits[3] = bits[4 * k + 0] ^ bits[4 * k + 2] ^ bits[4 * k + 3];
      encodedBits[4] = bits[4 * k + 1] ^ bits[4 * k + 3];
      encodedBits[5] = bits[4 * k + 2];
      encodedBits[6] = bits[4 * k + 3];
      encodedBits[7] =
          bits[4 * k + 0] ^ bits[4 * k + 1] ^ bits[4 * k + 2] ^ bits[4 * k + 3];

      uint8_t result = static_cast<uint8_t>(encodedBits.to_ulong());
      output.write(reinterpret_cast<char*>(&result), sizeof(result));
    }
  }

  return 0;
}