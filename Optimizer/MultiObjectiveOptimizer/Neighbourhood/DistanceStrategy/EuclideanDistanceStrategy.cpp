//
// Created by Le on 19.02.2018.
//

#include <cmath>
#include "EuclideanDistanceStrategy.h"

double EuclideanDistanceStrategy::distance(int x1, int y1, int x2, int y2) {
    return std::sqrt(std::pow(x1 - x2, 2) + std::pow(y1 - y2, 2));
}
