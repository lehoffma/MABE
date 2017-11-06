//
// Created by Le on 06.11.2017.
//

#include <algorithm>
#include "GreatestDistanceInitializer.h"


std::vector<std::pair<std::pair<int, int>, double>> GreatestDistanceInitializer::convertToDistanceList(
        std::vector<std::pair<int, int>> agentPositions,
        std::vector<std::pair<int, int>> availableSlots
) const {
    std::vector<std::pair<std::pair<int, int>, double>> distanceList(availableSlots.size());

    std::vector<std::pair<int, int>> nonZeroAgentPositions;
    std::copy_if(agentPositions.begin(), agentPositions.end(), std::back_inserter(nonZeroAgentPositions),
                 [](std::pair<int, int> slot) -> bool {
                     return slot.first != -1 && slot.second != -1;
                 });

    std::transform(availableSlots.begin(), availableSlots.end(), distanceList.begin(),
                   [nonZeroAgentPositions](std::pair<int, int> location) -> std::pair<std::pair<int, int>, double> {
                       std::pair<std::pair<int, int>, double> distancedLocation;
                       //because the = operator is deleted for some reason
                       distancedLocation.first.first = location.first;
                       distancedLocation.first.second = location.second;

                       //calculates the minimum distance to an agent
                       distancedLocation.second = std::accumulate(
                               nonZeroAgentPositions.begin(), nonZeroAgentPositions.end(), INFINITY,
                               [location](double minimumDistance, const std::pair<int, int> &position) -> double {
                                   double distance = std::sqrt(std::pow(location.first - position.first, 2) +
                                                     std::pow(location.second - position.second, 2));

                                   return std::min(minimumDistance, distance);
                               });

                       return distancedLocation;
                   }
    );

    return distanceList;
}


std::pair<int, int> GreatestDistanceInitializer::getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                                                 std::vector<std::pair<int, int>> availableSlots) {
    //todo better error handling
    if (availableSlots.empty()) {
        throw "No slot is available";
    }

    //calculate distances for each available slot
    auto distanceList = this->convertToDistanceList(agentPositions, availableSlots);

    //sort slots by their distance to the already placed agents (in descending order)
    std::sort(distanceList.begin(), distanceList.end(),
              [](std::pair<std::pair<int, int>, double> &distanceEntryA,
                 std::pair<std::pair<int, int>, double> &distanceEntryB) -> bool {
                  return distanceEntryA.second > distanceEntryB.second;
              }
    );

    //calculate how many entries have the same distance
    auto distance = distanceList[0].second;
    auto amountOfSameDistanceEntries = 0;
    //example: [3,3,3,2,0]
    //          -> amount = 3
    //another example: [4,3,2,1]
    //          -> amount = 1
    for (auto &distanceEntry : distanceList) {
        if (distance != distanceEntry.second) {
            break;
        }
        amountOfSameDistanceEntries++;
    }

    //choose a random entry of those with the lowest distance
    auto index = this->getRandomIndex(0, amountOfSameDistanceEntries - 1);

    return distanceList[index].first;
}
