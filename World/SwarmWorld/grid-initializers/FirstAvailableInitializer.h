//
// Created by Le on 02.11.2017.
//

#ifndef MABE_FIRSTAVAILABLEINITIALIZER_H
#define MABE_FIRSTAVAILABLEINITIALIZER_H


#include "GridInitializer.h"

class FirstAvailableInitializer: public GridInitializer{
public:
    std::pair<int, int> getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                        std::vector<std::pair<int, int>> availableSlots) override;
};


#endif //MABE_FIRSTAVAILABLEINITIALIZER_H
