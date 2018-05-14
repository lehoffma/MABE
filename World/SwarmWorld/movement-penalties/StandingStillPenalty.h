//
// Created by Le on 14.05.2018.
//

#ifndef MABE_STANDINGSTILLPENALTY_H
#define MABE_STANDINGSTILLPENALTY_H


#include "MovementPenalty.h"

class StandingStillPenalty : public MovementPenalty {
public:
    StandingStillPenalty(double penalty);

    double get(const std::shared_ptr<Agent> &agent) override;
};


#endif //MABE_STANDINGSTILLPENALTY_H
