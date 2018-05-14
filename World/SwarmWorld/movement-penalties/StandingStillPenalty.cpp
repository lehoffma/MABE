//
// Created by Le on 14.05.2018.
//

#include "StandingStillPenalty.h"

StandingStillPenalty::StandingStillPenalty(double penalty) : MovementPenalty(penalty) {}

double StandingStillPenalty::get(const std::shared_ptr<Agent> &agent) {
    return this->getBySequence(agent,
                               [](RelativeDirection direction) -> bool {
                                   return direction != RelativeDirection::NONE;
                               },
                               3);
}
