//
// Created by Le on 02.11.2017.
//

#include <algorithm>
#include "FirstAvailableInitializer.h"

std::pair<int, int> FirstAvailableInitializer::getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                                               std::vector<std::pair<int, int>> availableSlots) {
    auto index = 0;
    auto iterator = std::find_if(agentPositions.begin(), agentPositions.end(),
                                 [](std::pair<int, int> position) -> bool {
                                     return position.first == -1 && position.second == -1;
                                 });
    // -> how many agents have been placed already
    index = std::distance(agentPositions.begin(), iterator);

    // modulo with the size of the available slots array so we don't accidentally go out of bounds
    return availableSlots[index % availableSlots.size()];
}
