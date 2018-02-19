//
// Created by Le on 19.02.2018.
//

#include <algorithm>
#include "ChebyshevDistanceStrategy.h"

double ChebyshevDistanceStrategy::distance(int x1, int y1, int x2, int y2) {
    return std::max(std::abs(x1 - x2), std::abs(y1 - y2));
}
