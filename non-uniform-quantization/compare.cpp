#include <iostream>

#include "../tga/tga.hpp"

struct Statistics {
  double red;
  double green;
  double blue;
  double total;
};

Statistics mse(const tga::Image &image1, const tga::Image &image2) {
  double rMse{0.0};
  double gMse{0.0};
  double bMse{0.0};
  for (uint16_t row = 0; row < image1.height; row++) {
	for (uint16_t col = 0; col < image1.width; col++) {
	  bMse += std::pow(image1.colormap[row][col].blue - image2.colormap[row][col].blue, 2);
	  gMse += std::pow(image1.colormap[row][col].green - image2.colormap[row][col].green, 2);
	  rMse += std::pow(image1.colormap[row][col].red - image2.colormap[row][col].red, 2);
	}
  }

  double totalMse = rMse + gMse + bMse;

  return Statistics{
	  rMse / static_cast<double>(image1.height * image1.width),
	  gMse / static_cast<double>(image1.height * image1.width),
	  bMse / static_cast<double>(image1.height * image1.width),
	  totalMse / static_cast<double>(image1.height * image1.width)
  };
}

Statistics snr(const tga::Image &image1, const tga::Image &image2) {
  double rSnr{0.0};
  double gSnr{0.0};
  double bSnr{0.0};
  for (uint16_t row = 0; row < image1.height; row++) {
	for (uint16_t col = 0; col < image1.width; col++) {
	  bSnr += std::pow(image1.colormap[row][col].blue, 2);
	  gSnr += std::pow(image1.colormap[row][col].green, 2);
	  rSnr += std::pow(image1.colormap[row][col].red, 2);
	}
  }

  double totalSnr = rSnr + gSnr + bSnr;

  Statistics mseValue = mse(image1, image2);

  return Statistics{
	  std::log10(rSnr / mseValue.red / static_cast<double>(image1.height * image1.width)),
	  std::log10(gSnr / mseValue.green / static_cast<double>(image1.height * image1.width)),
	  std::log10(bSnr / mseValue.blue / static_cast<double>(image1.height * image1.width)),
	  std::log10(totalSnr / mseValue.total / static_cast<double>(image1.height * image1.width))
  };
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
	std::cout << "Usage: " << argv[0] << " <original image> <decoded image>" << std::endl;
  }

  tga::Image originalImage;
  tga::readImage(originalImage, argv[1]);

  tga::Image decodedImage;
  tga::readImage(decodedImage, argv[2]);

  Statistics mseValue = mse(originalImage, decodedImage);
  Statistics snrValue = snr(originalImage, decodedImage);

  std::cout << "MSE:" << std::endl;
  std::cout << "  Red: " << mseValue.red << std::endl;
  std::cout << "  Green: " << mseValue.green << std::endl;
  std::cout << "  Blue: " << mseValue.blue << std::endl;
  std::cout << "  Total: " << mseValue.total << std::endl;

  std::cout << "SNR:" << std::endl;
  std::cout << "  Red: " << snrValue.red << std::endl;
  std::cout << "  Green: " << snrValue.green << std::endl;
  std::cout << "  Blue: " << snrValue.blue << std::endl;
  std::cout << "  Total: " << snrValue.total << std::endl;

  return 0;
}