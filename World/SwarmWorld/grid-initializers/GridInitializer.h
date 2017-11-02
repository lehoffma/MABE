//
// Created by Le on 02.11.2017.
//

#ifndef MABE_GRIDINITIALIZER_H
#define MABE_GRIDINITIALIZER_H

#include <utility>
#include <vector>



class GridInitializer {
public:
    virtual std::pair<int, int> getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                                std::vector<std::pair<int, int>> availableSlots
    ) = 0;
};


#endif //MABE_GRIDINITIALIZER_H
