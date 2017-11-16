//
// Created by Le on 16.11.2017.
//

#include "WithinBoundsStrategy.h"

template<typename T>
bool WithinBoundsStrategy::isValid(Level<T> &level,
                                   const std::pair<int, int> &from,
                                   const std::pair<int, int> &to) {
    return !level.isFieldType(to, WALL) && !level.isOutOfBounds(to);
}
