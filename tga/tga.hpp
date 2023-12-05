#ifndef TGA_HPP
#define TGA_HPP

#include <cinttypes>
#include <vector>
#include <string>
#include <fstream>
#include <cmath>
#include <iostream>

namespace tga {
    enum Result {
        SUCCESS,
        CANNOT_OPEN_FILE,
        INCORRECT_FILE_STRUCTURE
    };

    struct Color {
        uint8_t blue;
        uint8_t green;
        uint8_t red;

        Color(uint8_t blue, uint8_t green, uint8_t red) : blue(blue), green(green), red(red) { }

        Color operator+(const Color& other) const {
            int16_t  castedBlue{blue};
            int16_t castedOtherBlue{other.blue};
            int16_t  castedGreen{green};
            int16_t castedOtherGreen{other.green};
            int16_t  castedRed{red};
            int16_t castedOtherRed{other.red};

            return {static_cast<uint8_t>(static_cast<uint8_t>((castedBlue + castedOtherBlue) % 256)),
                         static_cast<uint8_t>(static_cast<uint8_t>((castedGreen + castedOtherGreen) % 256)),
                         static_cast<uint8_t>(static_cast<uint8_t>((castedRed + castedOtherRed) % 256))};
        }

        Color operator-(const Color& other) const {
            int16_t  castedBlue{blue};
            int16_t castedOtherBlue{other.blue};
            int16_t  castedGreen{green};
            int16_t castedOtherGreen{other.green};
            int16_t  castedRed{red};
            int16_t castedOtherRed{other.red};

            return {static_cast<uint8_t>(static_cast<uint8_t>((castedBlue - castedOtherBlue + 256) % 256)),
                    static_cast<uint8_t>(static_cast<uint8_t>((castedGreen - castedOtherGreen + 256) % 256)),
                    static_cast<uint8_t>(static_cast<uint8_t>((castedRed - castedOtherRed + 256) % 256))};
        }

        Color operator/(uint8_t divisor) const {
            return {static_cast<uint8_t>(blue / divisor),
                    static_cast<uint8_t>(green / divisor),
                    static_cast<uint8_t>(red / divisor)};
        }
    };

    struct Image {
        uint8_t  idLength;
        uint8_t  colormapType;
        uint8_t  imageType;
        uint16_t colormapOrigin;
        uint16_t colormapLength;
        uint8_t  colormapDepth;
        uint16_t xOrigin;
        uint16_t yOrigin;
        uint16_t width;
        uint16_t height;
        uint8_t  pixelDepth;
        uint8_t  imageDescriptor;
        std::string imageId;
        std::vector<std::vector<Color>> colormap;
        std::string footer;
    };

    Result readImage(Image& image, const std::string& filename);
    Result writeImage(Image& image, const std::string& filename);
    void copyImage(const Image& source, Image& destination);
    void calculateEntropy(const Image& image, double& total, double& red, double& green, double& blue);
}

#endif // TGA_HPP