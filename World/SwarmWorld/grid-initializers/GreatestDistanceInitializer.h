//
// Created by Le on 06.11.2017.
//

#ifndef MABE_GREATESTDISTANCEINITIALIZER_H
#define MABE_GREATESTDISTANCEINITIALIZER_H


#include "GridInitializer.h"
#include "RandomInitializer.h"

class GreatestDistanceInitializer : public RandomInitializer {
private:

    /**
     * Converts the given list of available slots to a list of pairs each containing the position and its
     * minimum distance to an agent on the grid
     * @param agentPositions 
     * @param availableSlots 
     * @return 
     */
    std::vector<std::pair<std::pair<int, int>, double>> convertToDistanceList(
            std::vector<std::pair<int, int>> agentPositions,
            std::vector<std::pair<int, int>> availableSlots
    ) const;

public:
    std::pair<int, int> getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                        std::vector<std::pair<int, int>> availableSlots) override;
};


#endif //MABE_GREATESTDISTANCEINITIALIZER_H
