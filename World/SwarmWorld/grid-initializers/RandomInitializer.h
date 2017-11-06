//
// Created by Le on 02.11.2017.
//

#ifndef MABE_RANDOMINITIALIZER_H
#define MABE_RANDOMINITIALIZER_H


#include "GridInitializer.h"
#include <random>
#include <chrono>

class RandomInitializer : public GridInitializer {
protected:
    std::vector<std::pair<int, int>> getFreePositions(std::vector<std::pair<int, int>> agentPositions,
                                                      std::vector<std::pair<int, int>> availableSlots);

    static std::mt19937 rng;

    int getRandomIndex(int min, int max);

public:

    std::pair<int, int> getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                        std::vector<std::pair<int, int>> availableSlots) override;
};


#endif //MABE_RANDOMINITIALIZER_H
