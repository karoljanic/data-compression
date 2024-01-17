#ifndef LGB_HPP
#define LGB_HPP

#include <random>

double elementsDistance(double element1, double element2) {
  return std::pow(element1 - element2, 2);
}

double vectorsDistortion(const std::vector<double> &vector1,
						 const std::vector<double> &vector2) {
  double distortion = 0;
  for (size_t i = 0; i < vector1.size(); ++i) {
	distortion += elementsDistance(vector1[i], vector2[i]);
  }

  return distortion / vector1.size();
}

double vectorElementDistortion(const std::vector<double> &vector, double element) {
  double distortion = 0;
  for (const auto &value : vector) {
	distortion += elementsDistance(value, element);
  }

  return distortion / vector.size();
}

double vectorAverage(const std::vector<double> &vector) {
  double sum = 0;
  for (const auto &value : vector) {
	sum += value;
  }

  return sum / vector.size();
}

void lgb(std::vector<double> &input, std::vector<double> &codebook, uint8_t bits, double epsilon,
		 uint32_t maxIterationsNumber) {
  // Linde-Buzo-Gray algorithm
  const uint32_t codebookSize = std::pow(2, bits);
  const double average = vectorAverage(input);
  double distortion = vectorElementDistortion(input, average);

  codebook.push_back(average);

  while (codebook.size() < codebookSize) {
	std::vector<double> newCodebook;
	for (uint32_t i = 0; i < codebook.size(); i++) {
//	  std::random_device rd;
//	  std::mt19937 gen(rd());
//	  std::uniform_real_distribution<double> distribution(0.0, 1.0);
//	  double change = distribution(gen);
	  double change = 0.7;

	  newCodebook.push_back(codebook[i] + change);
	  newCodebook.push_back(codebook[i] - change);
	}

	double relativeError{1.0 + epsilon};
	double averageDistortion{0.0};
	double previousAverageDistortion{0.0};
	uint32_t iterationsNumber{0};
	while (relativeError > epsilon && iterationsNumber < maxIterationsNumber) {
	  std::vector<size_t> nearestCentroidsIndexes(input.size(), 0);
	  std::vector<std::vector<size_t>> elementsForNearestCentroids(newCodebook.size(), std::vector<size_t>());

	  // find nearest centroids for each element
	  for (size_t elementIndex = 0; elementIndex < input.size(); elementIndex++) {
		size_t nearestCentroidIndex{0};
		double nearestCentroidDistance = elementsDistance(input[elementIndex], newCodebook[0]);

		for (size_t centroidIndex = 1; centroidIndex < newCodebook.size(); centroidIndex++) {
		  double distance = elementsDistance(input[elementIndex], newCodebook[centroidIndex]);
		  if (distance < nearestCentroidDistance) {
			nearestCentroidIndex = centroidIndex;
			nearestCentroidDistance = distance;
		  }
		}

		nearestCentroidsIndexes[elementIndex] = nearestCentroidIndex;
		elementsForNearestCentroids[nearestCentroidIndex].push_back(elementIndex);
	  }

	  // update centroids
	  for (uint32_t centroidIndex = 0; centroidIndex < newCodebook.size(); centroidIndex++) {
		if (elementsForNearestCentroids[centroidIndex].size() != 0) {
		  std::vector<double> nearestElements{};
		  for (const auto &elementIndex : elementsForNearestCentroids[centroidIndex]) {
			nearestElements.push_back(input[elementIndex]);
		  }
		  newCodebook[centroidIndex] = vectorAverage(nearestElements);
		}
	  }

	  std::vector<double> closestCentroids{};
	  for (const auto &centroidIndex : nearestCentroidsIndexes) {
		closestCentroids.push_back(newCodebook[centroidIndex]);
	  }

	  previousAverageDistortion = averageDistortion > 0 ? averageDistortion : distortion;
	  averageDistortion = vectorsDistortion(input, closestCentroids);

	  relativeError = std::abs((averageDistortion - previousAverageDistortion) / averageDistortion);

	  iterationsNumber++;
	}

	codebook = newCodebook;
  }
}

#endif // LGB_HPP