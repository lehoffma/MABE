//
// Created by Le on 16.11.2017.
//

#ifndef MABE_MOVEVALIDITYCHECKER_H
#define MABE_MOVEVALIDITYCHECKER_H

#include <utility>
#include "../FieldType.h"
#include "../Level.h"

template<typename T>
class MoveValidityStrategy{
public:
    virtual bool isValid(Level<T>& level,
                         const std::pair<int, int> &from,
                         const std::pair<int, int>& to) = 0;
};

#endif //MABE_MOVEVALIDITYCHECKER_H
