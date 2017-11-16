//
// Created by Le on 16.11.2017.
//

#include "DebouncedGoalStrategy.h"

bool DebouncedGoalStrategy::predicate(Level<Field> &level, const Field &field, const std::pair<int, int>& to) {
    return level.isFieldType(to, GOAL) && field.agent->getWaitForGoal() <= 0;
}

void DebouncedGoalStrategy::scoringSideEffect(Field &field) {
    field.agent->setScore(field.agent->getScore() + 1)
            .setWaitForGoal(field.agent->getWaitForGoalInterval());
}
