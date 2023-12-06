#include <iostream>

#include <functional>
#include "../tga/tga.hpp"

double numberListEntropy(const std::vector<uint8_t>& numbers) {
    std::vector<uint64_t> numbersCount(256, 0);
    for(uint8_t number : numbers) {
        numbersCount[number]++;
    }

    double entropy = 0;
    for(uint64_t numberCount : numbersCount) {
        if(numberCount != 0) {
            double probability = static_cast<double>(numberCount) / static_cast<double>(numbers.size());
            entropy -= probability * std::log2(probability);
        }
    }

    return entropy;
}

void compareSchemes(const tga::Image& image) {
    constexpr uint8_t predictorsNumber{8};

    // define predictors
    std::function<tga::Color(tga::Color nPixel, tga::Color wPixel, tga::Color nwPixel)> predictors[predictorsNumber] = {
            [](tga::Color /*n*/, tga::Color w, tga::Color /*nw*/) { return w; },
            [](tga::Color n, tga::Color /*w*/, tga::Color /*nw*/) { return n; },
            [](tga::Color /*n*/, tga::Color /*w*/, tga::Color nw) { return nw; },
            [](tga::Color n, tga::Color w, tga::Color nw) {
                int16_t nRed{n.red}, nGreen{n.green}, nBlue{n.blue};
                int16_t wRed{w.red}, wGreen{w.green}, wBlue{w.blue};
                int16_t nwRed{nw.red}, nwGreen{nw.green}, nwBlue{nw.blue};

                return tga::Color(static_cast<uint8_t>((nBlue + wBlue - nwBlue + 256) % 256),
                        static_cast<uint8_t>((nGreen + wGreen - nwGreen + 256) % 256),
                        static_cast<uint8_t>((nRed + wRed - nwRed + 256) % 256));
            },
            [](tga::Color n, tga::Color w, tga::Color nw) {
                int16_t nRed{n.red}, nGreen{n.green}, nBlue{n.blue};
                int16_t wRed{w.red}, wGreen{w.green}, wBlue{w.blue};
                int16_t nwRed{nw.red}, nwGreen{nw.green}, nwBlue{nw.blue};

                    return tga::Color(static_cast<uint8_t>((nBlue + (wBlue - nwBlue) / 2 + 256) % 256),
                            static_cast<uint8_t>((nGreen + (wGreen - nwGreen) / 2 + 256) % 256),
                            static_cast<uint8_t>((nRed + (wRed - nwRed) / 2 + 256) % 256));
            },
            [](tga::Color n, tga::Color w, tga::Color nw) {
                int16_t nRed{n.red}, nGreen{n.green}, nBlue{n.blue};
                int16_t wRed{w.red}, wGreen{w.green}, wBlue{w.blue};
                int16_t nwRed{nw.red}, nwGreen{nw.green}, nwBlue{nw.blue};

                return tga::Color(static_cast<uint8_t>((wBlue + (nBlue - nwBlue) / 2 + 256) % 256),
                        static_cast<uint8_t>((wGreen + (nGreen - nwGreen) / 2 + 256) % 256),
                        static_cast<uint8_t>((wRed + (nRed - nwRed) / 2 + 256) % 256));
            },
            [](tga::Color n, tga::Color w, tga::Color /*nw*/) {
                return tga::Color((n.blue + w.blue) / 2, (n.green + w.green) / 2, (n.red + w.red) / 2);
            },
            [](tga::Color n, tga::Color w, tga::Color nw) {
                int16_t nRed{n.red}, nGreen{n.green}, nBlue{n.blue};
                int16_t wRed{w.red}, wGreen{w.green}, wBlue{w.blue};
                int16_t nwRed{nw.red}, nwGreen{nw.green}, nwBlue{nw.blue};
                int16_t r, g, b;
                
                if(nwRed >= std::max(nRed, wRed)) { r = std::max(nRed, wRed); }
                else if(nwRed <= std::min(nRed, wRed)) { r = std::min(nRed, wRed); }
                else { r = nRed + wRed - nwRed; }

                if(nwGreen >= std::max(nGreen, wGreen)) { g = std::max(nGreen, wGreen); }
                else if(nwGreen <= std::min(nGreen, wGreen)) { g = std::min(nGreen, wGreen); }
                else { g = nGreen + wGreen - nwGreen; }

                if(nwBlue >= std::max(nBlue, wBlue)) { b = std::max(nBlue, wBlue); }
                else if(nwBlue <= std::min(nBlue, wBlue)) { b = std::min(nBlue, wBlue); }
                else { b = nBlue + wBlue - nwBlue; }

                return tga::Color(static_cast<uint8_t>((b + 256) % 256), static_cast<uint8_t>((g + 256) % 256), static_cast<uint8_t>((r + 256) % 256));
            }
    };
    
    // define predictors names
    std::string predictorsNames[predictorsNumber] = {
            "W",
            "N",
            "NW",
            "N+W-NW",
            "N+(W-NW)/2",
            "W+(N-NW)/2",
            "(N+W)/2",
            "New standard"
    };

    // define objects for predicted images and corrections
    std::vector<tga::Image> predictedImages(predictorsNumber);
    std::vector<std::vector<std::vector<uint8_t>>> corrections(predictorsNumber);
    for(uint8_t predictorIndex  = 0; predictorIndex < predictorsNumber; predictorIndex++) {
        tga::copyImage(image, predictedImages[predictorIndex]);
        corrections[predictorIndex] = {{}, {}, {}, {}};
    }

    // predict images
    tga::Color outColor = tga::Color(0, 0, 0);
    for(uint16_t row = 0; row < image.height; row++) {
        for(uint16_t col = 0; col < image.width; col++) {
            tga::Color n = (row == 0) ? outColor : image.colormap[row-1][col];
            tga::Color w = (col == 0) ? outColor : image.colormap[row][col - 1];
            tga::Color nw = (row == 0 || col == 0) ? outColor : image.colormap[row-1][col-1];

            for(uint8_t predictorIndex  = 0; predictorIndex < predictorsNumber; predictorIndex++) {
                predictedImages[predictorIndex].colormap[row][col] = predictors[predictorIndex](n, w, nw);
            }
        }
    }

    // calculate corrections
    for(uint8_t predictorIndex  = 0; predictorIndex < predictorsNumber; predictorIndex++) {
        for(uint16_t row = 0; row < image.height; row++) {
            for(uint16_t col = 0; col < image.width; col++) {
                corrections[predictorIndex][0].push_back((image.colormap[row][col].red - predictedImages[predictorIndex].colormap[row][col].red + 256) % 256);
                corrections[predictorIndex][0].push_back((image.colormap[row][col].green - predictedImages[predictorIndex].colormap[row][col].green) % 256);
                corrections[predictorIndex][0].push_back((image.colormap[row][col].blue - predictedImages[predictorIndex].colormap[row][col].blue) % 256);
                corrections[predictorIndex][1].push_back((image.colormap[row][col].red - predictedImages[predictorIndex].colormap[row][col].red + 256) % 256);
                corrections[predictorIndex][2].push_back((image.colormap[row][col].green - predictedImages[predictorIndex].colormap[row][col].green) % 256);
                corrections[predictorIndex][3].push_back((image.colormap[row][col].blue - predictedImages[predictorIndex].colormap[row][col].blue) % 256);
            }
        }
    }

    // calculate image entropy
    double totalEntropy, redEntropy, greenEntropy, blueEntropy;
    tga::calculateEntropy(image, totalEntropy, redEntropy, greenEntropy, blueEntropy);
    
    // print results and finding the best predictor(predictor which corrections entropy is minimal)
    std::cout << "Image - all channels entropy: " << totalEntropy << std::endl;
    std::cout << "Image - red channel entropy: " << redEntropy << std::endl;
    std::cout << "Image - green channel entropy: " << greenEntropy << std::endl;
    std::cout << "Image - blue channel entropy: " << blueEntropy << std::endl;
    std::cout << std::endl;

    double bestPredictorAllChannelsEntropy = std::numeric_limits<double>::max();
    double bestPredictorRedChannelEntropy = std::numeric_limits<double>::max();
    double bestPredictorGreenChannelEntropy = std::numeric_limits<double>::max();
    double bestPredictorBlueChannelEntropy = std::numeric_limits<double>::max();
    uint8_t bestPredictorAllChannelsIndex = 0;
    uint8_t bestPredictorRedChannelIndex = 0;
    uint8_t bestPredictorGreenChannelIndex = 0;
    uint8_t bestPredictorBlueChannelIndex = 0;

    for(uint8_t predictorIndex  = 0; predictorIndex < predictorsNumber; predictorIndex++) {
        double totalCorrectionsEntropy = numberListEntropy(corrections[predictorIndex][0]);
        double redCorrectionsEntropy = numberListEntropy(corrections[predictorIndex][1]);
        double greenCorrectionsEntropy = numberListEntropy(corrections[predictorIndex][2]);
        double blueCorrectionsEntropy = numberListEntropy(corrections[predictorIndex][3]);

        if(totalCorrectionsEntropy < bestPredictorAllChannelsEntropy) {
            bestPredictorAllChannelsEntropy = totalCorrectionsEntropy;
            bestPredictorAllChannelsIndex = predictorIndex;
        }
        if(redCorrectionsEntropy < bestPredictorRedChannelEntropy) {
            bestPredictorRedChannelEntropy = redCorrectionsEntropy;
            bestPredictorRedChannelIndex = predictorIndex;
        }
        if(greenCorrectionsEntropy < bestPredictorGreenChannelEntropy) {
            bestPredictorGreenChannelEntropy = greenCorrectionsEntropy;
            bestPredictorGreenChannelIndex = predictorIndex;
        }
        if(blueCorrectionsEntropy < bestPredictorBlueChannelEntropy) {
            bestPredictorBlueChannelEntropy = blueCorrectionsEntropy;
            bestPredictorBlueChannelIndex = predictorIndex;
        }

        std::cout << predictorsNames[predictorIndex] << " predictor - all channels corrections entropy: " << totalCorrectionsEntropy << std::endl;
        std::cout << predictorsNames[predictorIndex] << " predictor - red channel corrections entropy: " << redCorrectionsEntropy << std::endl;
        std::cout << predictorsNames[predictorIndex] << " predictor - green channel corrections entropy: " << greenCorrectionsEntropy << std::endl;
        std::cout << predictorsNames[predictorIndex] << " predictor - blue channel corrections entropy: " << blueCorrectionsEntropy << std::endl;
        std::cout << std::endl;
    }

    std::cout << "Best predictor for all channels: " << predictorsNames[bestPredictorAllChannelsIndex] << " - " << bestPredictorAllChannelsEntropy << std::endl;
    std::cout << "Best predictor for red channel: " << predictorsNames[bestPredictorRedChannelIndex] << " - " << bestPredictorRedChannelEntropy << std::endl;
    std::cout << "Best predictor for green channel: " << predictorsNames[bestPredictorGreenChannelIndex] << " - " << bestPredictorGreenChannelEntropy << std::endl;
    std::cout << "Best predictor for blue channel: " << predictorsNames[bestPredictorBlueChannelIndex] << " - " << bestPredictorBlueChannelEntropy << std::endl;
}

int main(int argc, char** argv) {
    if(argc < 2) {
        std::cout << "Incorrect input. Enter file name!\n";
        return -1;
    }

    std::string fileName{argv[1]};
    tga::Image image;
    if(tga::readImage(image, fileName) == tga::SUCCESS) {
        compareSchemes(image);
    }
    else {
        std::cout << "Error: cannot read image!" << std::endl;
    }

    return 0;
}
