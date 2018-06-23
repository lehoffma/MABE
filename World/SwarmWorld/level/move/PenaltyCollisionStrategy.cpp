//
// Created by Le on 16.11.2017.
//

#include "PenaltyCollisionStrategy.h"
#include "../Field.h"

template<typename T>
void PenaltyCollisionStrategy<T>::collide(const std::shared_ptr<T> &field) {
    //score[organismIndex] -= penalty;
}

template<>
void PenaltyCollisionStrategy<Field>::collide(const std::shared_ptr<Field> &field) {
    field->agent->setScore(field->agent->getScore() - this->penalty);
    field->agent->setCollisions(field->agent->getCollisions() + 1);
}

template<typename T>
PenaltyCollisionStrategy<T> &PenaltyCollisionStrategy<T>::setPenalty(double penalty) {
    this->penalty = penalty;
    return *this;
}

template<typename T>
bool PenaltyCollisionStrategy<T>::hasCollided(const std::shared_ptr<T> &to) {
    return false;
}

template<>
bool PenaltyCollisionStrategy<Field>::hasCollided(const std::shared_ptr<Field> &to) {
    return !!to->agent || to->fieldType == WALL;
}
