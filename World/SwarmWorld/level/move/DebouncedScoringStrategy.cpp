//
// Created by Le on 16.11.2017.
//

#include "DebouncedScoringStrategy.h"

bool DebouncedGoalStrategy::isValid(Level<Field> *level, const Field &field, const std::pair<int, int> &to) {
    return field.agent->getWaitForGoal() <= 0;
}

void DebouncedGoalStrategy::scoringSideEffect(Field &field) {
    field.agent->setWaitForGoal(field.agent->getWaitForGoalInterval());
}
