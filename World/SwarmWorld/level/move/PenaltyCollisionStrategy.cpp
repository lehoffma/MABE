//
// Created by Le on 16.11.2017.
//

#include "PenaltyCollisionStrategy.h"
#include "../Field.h"

template<typename T>
void PenaltyCollisionStrategy<T>::collide(T &field) {
    //score[organismIndex] -= penalty;
}

template<>
void PenaltyCollisionStrategy<Field>::collide(Field &field) {
    field.agent->setScore(field.agent->getScore() - this->penalty);
}

template<typename T>
PenaltyCollisionStrategy<T> &PenaltyCollisionStrategy<T>::setPenalty(double penalty) {
    this->penalty = penalty;
    return *this;
}

template<typename T>
bool PenaltyCollisionStrategy<T>::hasCollided(T &to) {
    return false;
}

template<>
bool PenaltyCollisionStrategy<Field>::hasCollided(Field &to) {
    return !!to.agent || to.fieldType == WALL;
}
