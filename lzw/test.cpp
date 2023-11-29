#include <iostream>
#include "universal-encoding.hpp"

int main() {

  size_t num{137};
  std::vector<bool> numBits;

  //   encode_ellias_omega(num, numBits);
  //   size_t res = decode_ellias_omega(numBits);

  std::vector<size_t> fibb1 = {1, 1};
  encode_fibonacci(num, numBits, fibb1);
  std::vector<size_t> fibb2 = {1, 1};
  size_t res = decode_fibonacci(numBits, fibb2);

  std::cout << num << " " << res << std::endl;

  for (const bool bit : numBits) {
    if (bit)
      std::cout << "1";
    else
      std::cout << "0";
  }

  std::cout << std::endl;

  return 0;
}