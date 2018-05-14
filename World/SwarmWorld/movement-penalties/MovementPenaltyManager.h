//
// Created by Le on 14.05.2018.
//

#ifndef MABE_MOVEMENTPENALTYMANAGER_H
#define MABE_MOVEMENTPENALTYMANAGER_H


#include "MovementPenalty.h"

class MovementPenaltyManager {
public:
    MovementPenaltyManager &add(std::shared_ptr<MovementPenalty> penalty);

    double getPenalty(const std::shared_ptr<Agent> &agent);

private:
    std::vector<std::shared_ptr<MovementPenalty>> penalties{};


};


#endif //MABE_MOVEMENTPENALTYMANAGER_H
