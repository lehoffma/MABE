//
// Created by Le on 13.05.2018.
//

#ifndef MABE_SPINNINGPENALTY_H
#define MABE_SPINNINGPENALTY_H


#include "MovementPenalty.h"

class SpinningPenalty: public MovementPenalty{
public:

    SpinningPenalty(double penalty);

    double get(const std::shared_ptr<Agent> &agent) override;
};


#endif //MABE_SPINNINGPENALTY_H
