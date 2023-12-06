#include <iostream>
#include "../tga/tga.hpp"

double mse(const tga::Image& image1, const tga::Image& image2) {
    double mse{0.0};
    for(uint16_t row = 0; row < image1.height; row++) {
        for(uint16_t col = 0; col < image1.width; col++) {
            mse += std::pow(image1.colormap[row][col].blue - image2.colormap[row][col].blue, 2) +
                   std::pow(image1.colormap[row][col].green - image2.colormap[row][col].green, 2) +
                   std::pow(image1.colormap[row][col].red - image2.colormap[row][col].red, 2);
        }
    }

    return mse / static_cast<double>(image1.height * image1.width);
}

double snr(const tga::Image& image1, const tga::Image& image2) {
    double mseValue = mse(image1, image2);
    double snr{0.0};
    for(uint16_t row = 0; row < image1.height; row++) {
        for(uint16_t col = 0; col < image1.width; col++) {
            snr += std::pow(image1.colormap[row][col].blue, 2) +
                   std::pow(image1.colormap[row][col].green, 2) +
                   std::pow(image1.colormap[row][col].red, 2);
        }
    }

    return snr / mseValue / static_cast<double>(image1.height * image1.width);
}

uint16_t colorsDistance(const tga::Color& color1, const tga::Color& color2) {
//    return std::sqrt(std::pow(color1.blue - color2.blue, 2) +
//                     std::pow(color1.green - color2.green, 2) +
//                     std::pow(color1.red - color2.red, 2));
    return std::abs(color1.blue - color2.blue) +
           std::abs(color1.green - color2.green) +
           std::abs(color1.red - color2.red);
}

void flattenColormap(const std::vector<std::vector<tga::Color>>& colormap, std::vector<tga::Color>& flattenedColormap) {
    flattenedColormap.resize(colormap.size() * colormap[0].size());
    for(uint16_t row = 0; row < colormap.size(); row++) {
        for(uint16_t col = 0; col < colormap[0].size(); col++) {
            flattenedColormap[row * colormap[0].size() + col] = colormap[row][col];
        }
    }
}

tga::Color calculateAverageColor(const std::vector<tga::Color>& colors) {
    uint64_t blueSum{0};
    uint64_t greenSum{0};
    uint64_t redSum{0};
    uint64_t pixelsNumber{0};
    for(const auto& color : colors) {
        blueSum += color.blue;
        greenSum += color.green;
        redSum += color.red;
        pixelsNumber++;
    }

    return {static_cast<uint8_t>(static_cast<double>(blueSum)/ static_cast<double>(pixelsNumber)),
            static_cast<uint8_t>(static_cast<double>(greenSum)/ static_cast<double>(pixelsNumber)),
                    static_cast<uint8_t>(static_cast<double>(redSum)/ static_cast<double>(pixelsNumber))};
}

double calculateAverageDistortion(const std::vector<tga::Color>& colors, const tga::Color& averageColor) {
    double distortion{0.0};
    uint64_t pixelsNumber{0};
    for(const auto& color : colors) {
        distortion += colorsDistance(color, averageColor);
        pixelsNumber++;
    }

    return distortion / static_cast<double>(pixelsNumber);
}

double calculateAverageDistortion(const std::vector<tga::Color>& colors, const std::vector<tga::Color>& codebook) {
    double distortion{0.0};
    uint64_t pixelsNumber{0};
    for(size_t index = 0; index < colors.size(); index++) {
        distortion += colorsDistance(colors[index], codebook[index]);
        pixelsNumber++;
    }

    return distortion / static_cast<double>(pixelsNumber);
}

void splitCodebook(const std::vector<tga::Color> pixels, std::vector<tga::Color>& codebook, double& distortion, double epsilon) {
    std::vector<tga::Color> newCodebook;
    for(const tga::Color& color : codebook) {
        // divide each color into two near colors
        uint8_t newBlue1 = (color.blue == 255) ? color.blue : color.blue + 1;
        uint8_t newBlue2 = (color.blue == 0) ? color.blue : color.blue - 1;
        uint8_t newGreen1 = (color.green == 255) ? color.green : color.green + 1;
        uint8_t newGreen2 = (color.green == 0) ? color.green : color.green - 1;
        uint8_t newRed1 = (color.red == 255) ? color.red : color.red + 1;
        uint8_t newRed2 = (color.red == 0) ? color.red : color.red - 1;

        tga::Color newColor1{newBlue1, newGreen1, newRed1};
        tga::Color newColor2{newBlue2, newGreen2, newRed2};
        newCodebook.push_back(newColor1);
        newCodebook.push_back(newColor2);
    }

    double relativeError{1.0 + epsilon};
    double averageDistortion{0.0};
    while (relativeError > epsilon) {
        std::vector<size_t > nearestCentroidsIndexes(pixels.size(), 0);
        std::vector<std::vector<size_t>> pixelsForNearestCentroids(newCodebook.size(), std::vector<size_t>());

        // find nearest centroids for each pixel
        for(size_t pixelIndex = 0; pixelIndex < pixels.size(); pixelIndex++) {
            size_t nearestCentroidIndex{0};
            uint16_t nearestCentroidDistance{colorsDistance(pixels[pixelIndex], newCodebook[0])};

            for(size_t centroidIndex = 1; centroidIndex < newCodebook.size(); centroidIndex++) {
                uint16_t distance{colorsDistance(pixels[pixelIndex], newCodebook[centroidIndex])};
                if(distance < nearestCentroidDistance) {
                    nearestCentroidIndex = centroidIndex;
                    nearestCentroidDistance = distance;
                }
            }

            nearestCentroidsIndexes[pixelIndex] = nearestCentroidIndex;
            pixelsForNearestCentroids[nearestCentroidIndex].push_back(pixelIndex);
        }

        // update centroids
        for(uint32_t centroidIndex = 0; centroidIndex < newCodebook.size(); centroidIndex++) {
            if(pixelsForNearestCentroids[centroidIndex].size() != 0) {
                std::vector<tga::Color> nearestPixels{};
                for(const auto& pixelIndex : pixelsForNearestCentroids[centroidIndex]) {
                    nearestPixels.push_back(pixels[pixelIndex]);
                }
                newCodebook[centroidIndex] = calculateAverageColor(nearestPixels);
            }
        }

        std::vector<tga::Color> closestCentroids{};
        for(const auto& centroidIndex : nearestCentroidsIndexes) {
            closestCentroids.push_back(newCodebook[centroidIndex]);
        }

        double previousAverageDistortion = averageDistortion > 0 ? averageDistortion : distortion;
        averageDistortion = calculateAverageDistortion(pixels, closestCentroids);

        relativeError = std::abs((previousAverageDistortion - averageDistortion) / previousAverageDistortion);
    }

    codebook = newCodebook;
}

void generateCodebook(const std::vector<tga::Color>& colors, std::vector<tga::Color>& codebook, uint32_t colorsNumber, double epsilon) {
    // Linde-Buzo-Gray algorithm

    auto averageColor = calculateAverageColor(colors);
    auto distortion = calculateAverageDistortion(colors, averageColor);
    codebook.push_back(averageColor);

    while (codebook.size() < colorsNumber) {
        splitCodebook(colors, codebook, distortion, epsilon);
    }
}

void quantify(const tga::Image& image, tga::Image& quantizedImage, uint32_t colorsNumber) {
    // generate codebook
    std::vector<tga::Color> flattenedColormap;
    flattenColormap(image.colormap, flattenedColormap);
    std::vector<tga::Color> codebook;
    generateCodebook(flattenedColormap, codebook, colorsNumber, 0.0001);

    // result image initialization
    tga::copyImage(image, quantizedImage);

    std::cout << "Colors number: " << codebook.size() << std::endl;

    // quantize image
    for(uint16_t row = 0; row < quantizedImage.height; row++) {
        for(uint16_t col = 0; col < quantizedImage.width; col++) {
            uint16_t nearestCentroidDistance{colorsDistance(quantizedImage.colormap[row][col], codebook[0])};
            size_t nearestCentroidIndex{0};
            for(size_t centroidIndex = 1; centroidIndex < codebook.size(); centroidIndex++) {
                uint16_t distance{colorsDistance(quantizedImage.colormap[row][col], codebook[centroidIndex])};
                if(distance < nearestCentroidDistance) {
                    nearestCentroidIndex = centroidIndex;
                    nearestCentroidDistance = distance;
                }
            }
            quantizedImage.colormap[row][col] = codebook[nearestCentroidIndex];
        }
    }
}

int main(int argc, char** argv) {
    if(argc < 4) {
        std::cout << "Incorrect input. Enter input filename, output filename and colors number description!\n";
        return -1;
    }

    std::string inputFileName{argv[1]};
    std::string outputFileName{argv[2]};
    uint8_t colorsNumber{static_cast<uint8_t>(std::stoi(argv[3]))};

    tga::Image image;
    if(tga::readImage(image, inputFileName) == tga::SUCCESS) {
        std::cout << "Total colors number: " << tga::countColors(image) << std::endl;
        tga::Image quantizedImage;
        quantify(image, quantizedImage, 1 << colorsNumber);

        std::cout << "MSE: " << mse(image, quantizedImage) << std::endl;
        std::cout << "SNR: " << snr(image, quantizedImage) << std::endl;

        if(tga::writeImage(quantizedImage, outputFileName) == tga::SUCCESS) {
            std::cout << "Image was successfully quantized!\n";
        }
        else {
            std::cout << "Error: cannot write image!" << std::endl;
        }
    }
    else {
        std::cout << "Error: cannot read image!" << std::endl;
    }

    return 0;
}
