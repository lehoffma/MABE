//
// Created by Le on 16.11.2017.
//

#include "PenaltyCollisionStrategy.h"
#include "../Field.h"

template<typename T>
void PenaltyCollisionStrategy::collide(T &field) {
    //score[organismIndex] -= penalty;
}

template<>
void PenaltyCollisionStrategy::collide(Field &field) {
    field.agent->setScore(field.agent->getScore() - this->penalty);
}

PenaltyCollisionStrategy & PenaltyCollisionStrategy::setPenalty(double penalty) {
    this->penalty = penalty;
    return *this;
}
