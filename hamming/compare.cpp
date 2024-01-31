#include <cstdint>
#include <fstream>
#include <iostream>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cout << "Usage: " << argv[0] << " <input file 1> <input file 2>"
              << std::endl;
    return 1;
  }

  std::ifstream input1(argv[1], std::ios::binary);
  std::ifstream input2(argv[2], std::ios::binary);

  if (!input1.is_open()) {
    std::cout << "Error opening first input file" << std::endl;
    return 1;
  }

  if (!input2.is_open()) {
    std::cout << "Error opening second input file" << std::endl;
    return 1;
  }

  size_t blocksCounter{0};
  size_t errorsCounter{0};

  while (!input1.eof() && !input2.eof()) {
    uint8_t byteValue1{0};
    input1.read(reinterpret_cast<char*>(&byteValue1), sizeof(byteValue1));
    uint8_t byteValue2{0};
    input2.read(reinterpret_cast<char*>(&byteValue2), sizeof(byteValue2));

    if (input1.eof() || input2.eof()) {
      break;
    }

    blocksCounter += 2;
    for (size_t i = 0; i < 4; i++) {
      if ((byteValue1 & (1 << i)) != (byteValue2 & (1 << i))) {
        errorsCounter++;
        break;
      }
    }

    for (size_t i = 4; i < 8; i++) {
      if ((byteValue1 & (1 << i)) != (byteValue2 & (1 << i))) {
        errorsCounter++;
        break;
      }
    }
  }

  std::cout << static_cast<float>(errorsCounter) / blocksCounter * 100.0
            << std::endl;

  //   std::cout << errorsCounter << " errors in " << blocksCounter << " blocks: "
  //             << static_cast<float>(errorsCounter) / blocksCounter * 100.0 << "%"
  //             << std::endl;

  return 0;
}