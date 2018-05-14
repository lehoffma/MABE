//
// Created by Le on 13.05.2018.
//

#include "SpinningPenalty.h"

double SpinningPenalty::get(const std::shared_ptr<Agent> &agent) {
    return this->getBySequence(agent,
                               [](RelativeDirection direction) -> bool {
                                   return direction != RelativeDirection::LEFT && direction != RelativeDirection::RIGHT;
                               },
                               5);
}

SpinningPenalty::SpinningPenalty(double penalty) : MovementPenalty(penalty) {}

