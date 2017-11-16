//
// Created by Le on 16.11.2017.
//

#ifndef MABE_SCORINGSTRATEGY_H
#define MABE_SCORINGSTRATEGY_H

#include "../Level.h"

template<typename T>
class ScoringStrategy{
public:
    virtual bool isValid(Level<T> &level, const T &field, const std::pair<int, int> &to) = 0;
    virtual void scoringSideEffect(T& field) = 0;
};

#endif //MABE_SCORINGSTRATEGY_H
