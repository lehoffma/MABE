//
// Created by Le on 06.11.2017.
//

#ifndef MABE_GREATESTDISTANCEINITIALIZER_H
#define MABE_GREATESTDISTANCEINITIALIZER_H


#include "GridInitializer.h"
#include "RandomInitializer.h"

class GreatestDistanceInitializer : public RandomInitializer {
private:


public:
    std::pair<int, int> getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                        std::vector<std::pair<int, int>> availableSlots) override;
};


#endif //MABE_GREATESTDISTANCEINITIALIZER_H
