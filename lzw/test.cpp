#include <iostream>
#include "universal-encoding.hpp"

int main() {

  unsigned char val = 190;
  char val2 = static_cast<char>(val);
  unsigned char val3 = static_cast<unsigned char>(val2);
  std::cout << val << std::endl << val2 << std::endl << val3 << std::endl;

  // size_t num1{137};
  // size_t num2{44};
  // std::vector<bool> numBits;

  // encode_ellias_omega(num1, numBits);
  // encode_ellias_omega(num2, numBits);
  // size_t res1 = decode_ellias_omega(numBits);
  // size_t res2 = decode_ellias_omega(numBits);

  // // std::vector<size_t> fibb1 = {1, 1};
  // // encode_fibonacci(num, numBits, fibb1);
  // // std::vector<size_t> fibb2 = {1, 1};
  // // size_t res = decode_fibonacci(numBits, fibb2);

  // std::cout << res1 << " " << res2 << std::endl;

  // for (const bool bit : numBits) {
  //   if (bit)
  //     std::cout << "1";
  //   else
  //     std::cout << "0";
  // }

  // std::cout << std::endl;

  return 0;
}