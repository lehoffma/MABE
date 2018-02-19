//
// Created by Le on 19.02.2018.
//

#include <cmath>
#include "ManhattanDistanceStrategy.h"

double ManhattanDistanceStrategy::distance(int x1, int y1, int x2, int y2) {
    return std::abs(x1 - x2) + std::abs(y1 - y2);
}
