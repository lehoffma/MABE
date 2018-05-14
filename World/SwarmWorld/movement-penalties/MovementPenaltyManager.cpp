//
// Created by Le on 14.05.2018.
//

#include "MovementPenaltyManager.h"

double MovementPenaltyManager::getPenalty(const std::shared_ptr<Agent> &agent) {
    double combinedPenalty = 0;
    for (const auto &penalty: this->penalties) {
        combinedPenalty += penalty->get(agent);
    }
    return combinedPenalty;
}

MovementPenaltyManager &MovementPenaltyManager::add(std::shared_ptr<MovementPenalty> penalty) {
    this->penalties.emplace_back(penalty);
    return *this;
}
