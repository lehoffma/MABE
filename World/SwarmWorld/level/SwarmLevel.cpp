//
// Created by Le on 15.11.2017.
//

#include "SwarmLevel.h"

SwarmLevel::SwarmLevel(const std::pair<int, int> &dimensions) : Level(dimensions) {}

FieldType SwarmLevel::getFromValue(const Field &value) const {
    return value.fieldType;
}

FieldType getFieldTypeFromString(const std::string &value) {
    return value == "0" ? WALL
                        : value == "1" ? FLOOR
                                       : value == "3" ? START
                                                      : value == "4" ? GOAL
                                                                     : FLOOR;
}

Field SwarmLevel::getValueFromFile(const std::string &fileValue) {
    return *new Field(nullptr, getFieldTypeFromString(fileValue));
}

void SwarmLevel::move(const std::pair<int, int> &from, const std::pair<int, int> &to) {
    //todo movePredicate()
    if (isOutOfBounds(from) || isOutOfBounds(to)) {
        return;
    }

    Field field = this->get(from);
    //there is an agent on this field
    if (field.agent) {
        field.agent->setWaitForGoal(field.agent->getWaitForGoal() - 1);

        if (this->isFieldType(to, GOAL) && field.agent->getWaitForGoal() <= 0) {
            //todo scoring handler => {predicate: (const T& field) => boolean, sideEffect: (T& field) => void}
            field.agent->setScore(field.agent->getScore() + 1)
                    .setWaitForGoal(field.agent->getWaitForGoalInterval());
        }

        Field toField = this->get(to);
        if (toField.agent) {
            //todo collisionHandler, either none or if hasPenalty: init with "penaltyCollisionHandler"
            // => simple lambda probably or interface that implements "void collide(T field)"

            //score[organismIndex] -= penalty;
        }

        toField.agent = field.agent;
        toField.agent->setLocation(to);

        //todo moveSideEffects: (T& field) => void
        //
        //    agentMap[newloc.first][newloc.second] += 1;
        //    if (phero) {
        //        pheroMap[newloc.first][newloc.second] = 1;
        //    }
        //    if (previousLocation.first > 0 && previousLocation.second > 0) {
        //        agentMap[previousLocation.first][previousLocation.second] -= 1;
        //    }
    }
}

