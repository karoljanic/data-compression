#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>
#include <vector>

int main(int argc, char* argv[]) {
  if (argc != 4) {
    std::cout << "Usage: " << argv[0]
              << " <probability> <input file> <output file>" << std::endl;
    return 1;
  }

  float probability = atof(argv[1]);
  std::ifstream input(argv[2], std::ios::binary);
  std::ofstream output(argv[3], std::ios::binary);

  if (!input.is_open()) {
    std::cout << "Error opening input file" << std::endl;
    return 1;
  }

  if (!output.is_open()) {
    std::cout << "Error opening output file" << std::endl;
    return 1;
  }

  std::mt19937 generator{std::random_device{}()};
  std::uniform_real_distribution<float> distribution(0.0, 1.0);

  while (!input.eof()) {
    uint8_t byteValue{0};
    input.read(reinterpret_cast<char*>(&byteValue), sizeof(byteValue));
    if (input.eof()) {
      break;
    }

    for (size_t i = 0; i < 8; i++) {
      if (distribution(generator) < probability) {
        byteValue ^= (1 << i);
      }
    }

    output.write(reinterpret_cast<char*>(&byteValue), sizeof(byteValue));
  }

  return 0;
}