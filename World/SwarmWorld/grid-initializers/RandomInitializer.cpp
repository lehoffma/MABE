//
// Created by Le on 02.11.2017.
//

#include <algorithm>
#include <random>
#include "RandomInitializer.h"


std::vector<std::pair<int, int>> RandomInitializer::getFreePositions(std::vector<std::pair<int, int>> agentPositions,
                                                                     std::vector<std::pair<int, int>> availableSlots) {
    std::vector<std::pair<int, int>> nonBlockedPositions = std::vector<std::pair<int, int>>();

    std::copy_if(availableSlots.begin(), availableSlots.end(), std::back_inserter(nonBlockedPositions),
                 [agentPositions](std::pair<int, int> slot) -> bool {
                     auto iterator = std::find_if(agentPositions.begin(), agentPositions.end(),
                                                  [slot](const std::pair<int, int> &agentPosition) -> bool {
                                                      return agentPosition.first == slot.first &&
                                                             agentPosition.second == slot.second;
                                                  });
                     //no agent has the same position as the slot => keep them
                     return iterator == agentPositions.end();
                 });

    return nonBlockedPositions;
}

int RandomInitializer::getRandomIndex(int min, int max) {
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased
    return uni(rng);
}

std::pair<int, int> RandomInitializer::getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                                       std::vector<std::pair<int, int>> availableSlots) {

    std::vector<std::pair<int, int>> nonBlockedPositions = this->getFreePositions(agentPositions, availableSlots);
    auto randomIndex = this->getRandomIndex(0, nonBlockedPositions.size() - 1);

    return nonBlockedPositions[randomIndex];
}