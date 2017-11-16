//
// Created by Le on 16.11.2017.
//

#ifndef MABE_WITHINBOUNDSSTRATEGY_H
#define MABE_WITHINBOUNDSSTRATEGY_H


#include "MoveValidityStrategy.h"

template<typename T>
class WithinBoundsStrategy: public MoveValidityStrategy<T>{
public:
    bool isValid(Level<T>& level,
                 const std::pair<int, int> &from,
                 const std::pair<int, int> &to) override;
};



#endif //MABE_WITHINBOUNDSSTRATEGY_H
