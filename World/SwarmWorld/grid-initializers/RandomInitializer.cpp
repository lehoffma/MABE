//
// Created by Le on 02.11.2017.
//

#include <algorithm>
#include <random>
#include <iostream>
#include <chrono>
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
    //usually, the random_device would be used to seed the rng, but it's kinda buggy on windows (always returning the same number)
//    std::random_device rd;     // only used once to initialise (seed) engine
    // random-number engine used (Mersenne-Twister in this case)

    std::uniform_int_distribution<int> uni(min, max); // guaranteed unbiased
    return uni(rng);
}

std::pair<int, int> RandomInitializer::getNextPosition(std::vector<std::pair<int, int>> agentPositions,
                                                       std::vector<std::pair<int, int>> availableSlots) {

    std::vector<std::pair<int, int>> nonBlockedPositions = this->getFreePositions(agentPositions, availableSlots);
    auto randomIndex = this->getRandomIndex(0, nonBlockedPositions.size() - 1);

    return nonBlockedPositions[randomIndex];
}

std::mt19937 RandomInitializer::rng(
        static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));