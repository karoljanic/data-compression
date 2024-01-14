#include <iostream>
#include <random>
#include "../tga/tga.hpp"

double mse(const tga::Image &image1, const tga::Image &image2)
{
    double mse{0.0};
    for (uint16_t row = 0; row < image1.height; row++)
    {
        for (uint16_t col = 0; col < image1.width; col++)
        {
            mse += std::pow(image1.colormap[row][col].blue - image2.colormap[row][col].blue, 2) +
                   std::pow(image1.colormap[row][col].green - image2.colormap[row][col].green, 2) +
                   std::pow(image1.colormap[row][col].red - image2.colormap[row][col].red, 2);
        }
    }

    return mse / static_cast<double>(image1.height * image1.width);
}

double snr(const tga::Image &image1, const tga::Image &image2)
{
    double mseValue = mse(image1, image2);
    double snr{0.0};
    for (uint16_t row = 0; row < image1.height; row++)
    {
        for (uint16_t col = 0; col < image1.width; col++)
        {
            snr += std::pow(image1.colormap[row][col].blue, 2) +
                   std::pow(image1.colormap[row][col].green, 2) +
                   std::pow(image1.colormap[row][col].red, 2);
        }
    }

    return snr / mseValue / static_cast<double>(image1.height * image1.width);
}

double colorsDistance(const std::vector<double> &color1, const std::vector<double> &color2)
{
    return std::sqrt(std::pow(color1[0] - color2[0], 2) +
                     std::pow(color1[1] - color2[1], 2) +
                     std::pow(color1[2] - color2[2], 2));
    //     return std::abs(color1[0] - color2[0]) +
    //            std::abs(color1[1] - color2[1]) +
    //            std::abs(color1[2] - color2[2]);
}

void flattenColormap(const std::vector<std::vector<tga::Color>> &colormap, std::vector<std::vector<double>> &flattenedColormap)
{
    flattenedColormap.resize(colormap.size() * colormap[0].size());
    for (uint16_t row = 0; row < colormap.size(); row++)
    {
        for (uint16_t col = 0; col < colormap[0].size(); col++)
        {
            flattenedColormap[row * colormap[0].size() + col] = {static_cast<double>(colormap[row][col].blue),
                                                                 static_cast<double>(colormap[row][col].green),
                                                                 static_cast<double>(colormap[row][col].red)};
        }
    }
}

std::vector<double> calculateAverageColor(const std::vector<std::vector<double>> &colors)
{
    double blueSum{0};
    double greenSum{0};
    double redSum{0};
    uint64_t pixelsNumber{0};
    for (const auto &color : colors)
    {
        blueSum += color[0];
        greenSum += color[1];
        redSum += color[2];
        pixelsNumber++;
    }

    return {blueSum / static_cast<double>(pixelsNumber),
            greenSum / static_cast<double>(pixelsNumber),
            redSum / static_cast<double>(pixelsNumber)};
}

double calculateAverageDistortion(const std::vector<std::vector<double>> &colors, const std::vector<double> &averageColor)
{
    double distortion{0.0};
    uint64_t pixelsNumber{0};
    for (const auto &color : colors)
    {
        distortion += colorsDistance(color, averageColor);
        pixelsNumber++;
    }

    return distortion / static_cast<double>(pixelsNumber);
}

double calculateAverageDistortion(const std::vector<std::vector<double>> &colors, const std::vector<std::vector<double>> &codebook)
{
    double distortion{0.0};
    uint64_t pixelsNumber{0};
    for (size_t index = 0; index < colors.size(); index++)
    {
        distortion += colorsDistance(colors[index], codebook[index]);
        pixelsNumber++;
    }

    return distortion / static_cast<double>(pixelsNumber);
}

void splitCodebook(const std::vector<std::vector<double>> pixels, std::vector<std::vector<double>> &codebook, double &distortion, double epsilon)
{
    std::vector<std::vector<double>> newCodebook;
    for (const auto &color : codebook)
    {
        // divide each color into two near colors with random epsilon
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> distribution(0.0, 1.0);
        double randomFraction = distribution(gen);

        std::vector<double> newColor1{color[0] - randomFraction, color[1] + randomFraction, color[2] - randomFraction};
        std::vector<double> newColor2{color[0] + randomFraction, color[1] - randomFraction, color[2] + randomFraction};
        newCodebook.push_back(newColor1);
        newCodebook.push_back(newColor2);
    }

    double relativeError{1.0 + epsilon};
    double averageDistortion{0.0};
    while (relativeError > epsilon)
    {
        std::vector<size_t> nearestCentroidsIndexes(pixels.size(), 0);
        std::vector<std::vector<size_t>> pixelsForNearestCentroids(newCodebook.size(), std::vector<size_t>());

        // find nearest centroids for each pixel
        for (size_t pixelIndex = 0; pixelIndex < pixels.size(); pixelIndex++)
        {
            size_t nearestCentroidIndex{0};
            double nearestCentroidDistance{colorsDistance(pixels[pixelIndex], newCodebook[0])};

            for (size_t centroidIndex = 1; centroidIndex < newCodebook.size(); centroidIndex++)
            {
                double distance{colorsDistance(pixels[pixelIndex], newCodebook[centroidIndex])};
                if (distance < nearestCentroidDistance)
                {
                    nearestCentroidIndex = centroidIndex;
                    nearestCentroidDistance = distance;
                }
            }

            nearestCentroidsIndexes[pixelIndex] = nearestCentroidIndex;
            pixelsForNearestCentroids[nearestCentroidIndex].push_back(pixelIndex);
        }

        // update centroids
        for (size_t centroidIndex = 0; centroidIndex < newCodebook.size(); centroidIndex++)
        {
            if (pixelsForNearestCentroids[centroidIndex].size() != 0)
            {
                std::vector<std::vector<double>> nearestPixels{};
                for (const auto &pixelIndex : pixelsForNearestCentroids[centroidIndex])
                {
                    nearestPixels.push_back(pixels[pixelIndex]);
                }
                newCodebook[centroidIndex] = calculateAverageColor(nearestPixels);
            }
        }

        std::vector<std::vector<double>> closestCentroids{};
        for (const auto &centroidIndex : nearestCentroidsIndexes)
        {
            closestCentroids.push_back(newCodebook[centroidIndex]);
        }

        double previousAverageDistortion = averageDistortion > 0 ? averageDistortion : distortion;
        averageDistortion = calculateAverageDistortion(pixels, closestCentroids);

        relativeError = std::abs((previousAverageDistortion - averageDistortion) / previousAverageDistortion);
    }

    codebook = newCodebook;
}

void generateCodebook(const std::vector<std::vector<double>> &colors, std::vector<std::vector<double>> &codebook, uint32_t colorsNumber, double epsilon)
{
    // Linde-Buzo-Gray algorithm

    auto averageColor = calculateAverageColor(colors);
    auto distortion = calculateAverageDistortion(colors, averageColor);
    codebook.push_back(averageColor);

    while (codebook.size() < colorsNumber)
    {
        splitCodebook(colors, codebook, distortion, epsilon);
    }
}

void quantify(const tga::Image &image, tga::Image &quantizedImage, uint32_t colorsNumber)
{
    // generate codebook
    std::vector<std::vector<double>> flattenedColormap;
    flattenColormap(image.colormap, flattenedColormap);
    std::vector<std::vector<double>> codebook;
    generateCodebook(flattenedColormap, codebook, colorsNumber, 0.0001);

    // result image initialization
    tga::copyImage(image, quantizedImage);

    std::cout << "Colors number: " << codebook.size() << std::endl;

    // quantize image
    for (uint16_t row = 0; row < quantizedImage.height; row++)
    {
        for (uint16_t col = 0; col < quantizedImage.width; col++)
        {
            tga::Color pixel = quantizedImage.colormap[row][col];
            std::vector<double> pixelVector{static_cast<double>(pixel.blue), static_cast<double>(pixel.green), static_cast<double>(pixel.red)};
            double nearestCentroidDistance{colorsDistance(pixelVector, codebook[0])};
            size_t nearestCentroidIndex{0};
            for (size_t centroidIndex = 1; centroidIndex < codebook.size(); centroidIndex++)
            {
                double distance{colorsDistance(pixelVector, codebook[centroidIndex])};
                if (distance < nearestCentroidDistance)
                {
                    nearestCentroidIndex = centroidIndex;
                    nearestCentroidDistance = distance;
                }
            }
            quantizedImage.colormap[row][col] = {static_cast<uint8_t>(codebook[nearestCentroidIndex][0]),
                                                 static_cast<uint8_t>(codebook[nearestCentroidIndex][1]),
                                                 static_cast<uint8_t>(codebook[nearestCentroidIndex][2])};
        }
    }
}

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        std::cout << "Incorrect input. Enter input filename, output filename and colors number description!\n";
        return -1;
    }

    std::string inputFileName{argv[1]};
    std::string outputFileName{argv[2]};
    uint8_t colorsNumber{static_cast<uint8_t>(std::stoi(argv[3]))};

    tga::Image image;
    if (tga::readImage(image, inputFileName) == tga::SUCCESS)
    {
        std::cout << "Total colors number: " << tga::countColors(image) << std::endl;
        tga::Image quantizedImage;
        quantify(image, quantizedImage, 1 << colorsNumber);

        std::cout << "MSE: " << mse(image, quantizedImage) << std::endl;
        std::cout << "SNR: " << snr(image, quantizedImage) << std::endl;

        if (tga::writeImage(quantizedImage, outputFileName) == tga::SUCCESS)
        {
            std::cout << "Image was successfully quantized!\n";
        }
        else
        {
            std::cout << "Error: cannot write image!" << std::endl;
        }
    }
    else
    {
        std::cout << "Error: cannot read image!" << std::endl;
    }

    return 0;
}
