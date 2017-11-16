//
// Created by Le on 16.11.2017.
//

#include "WithinBoundsStrategy.h"
#include "../Field.h"

template<typename T>
bool WithinBoundsStrategy<T>::isValid(Level<T> *level,
                                      const std::pair<int, int> &from,
                                      const std::pair<int, int> &to) {
    return !level->isFieldType(to, WALL) && !level->isOutOfBounds(to);
}


template class WithinBoundsStrategy<Field>;