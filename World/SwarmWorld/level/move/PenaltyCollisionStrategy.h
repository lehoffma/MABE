//
// Created by Le on 16.11.2017.
//

#ifndef MABE_PENALTYCOLLISIONSTRATEGY_H
#define MABE_PENALTYCOLLISIONSTRATEGY_H


#include "CollisionStrategy.h"

template<typename T>
class PenaltyCollisionStrategy : public CollisionStrategy<T> {
private:
    double penalty;
public:
    explicit PenaltyCollisionStrategy(double penalty):penalty(penalty){

    }
    PenaltyCollisionStrategy():penalty(0){

    }

    void collide(T &field) override;

    bool hasCollided(T &to) override;

    PenaltyCollisionStrategy & setPenalty(double penalty);
};


#endif //MABE_PENALTYCOLLISIONSTRATEGY_H
