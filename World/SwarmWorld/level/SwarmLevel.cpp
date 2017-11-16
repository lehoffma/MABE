//
// Created by Le on 15.11.2017.
//

#include "SwarmLevel.h"
#include "move/WithinBoundsStrategy.h"
#include "move/DebouncedGoalStrategy.h"
#include "move/PenaltyCollisionStrategy.h"

SwarmLevel::SwarmLevel(const std::pair<int, int> &dimensions) : Level(dimensions) {
    //default values
    this->moveValidityStrategy = std::unique_ptr<MoveValidityStrategy<Field>>(new WithinBoundsStrategy<Field>());
    this->scoringStrategy = std::unique_ptr<ScoringStrategy<Field>>(new DebouncedGoalStrategy());
    //no-op, no collision at all
    this->collisionStrategy = std::unique_ptr<CollisionStrategy<Field>>(new PenaltyCollisionStrategy<Field>(0));
}


SwarmLevel::SwarmLevel(const std::pair<int, int> &dimensions,
                       const std::shared_ptr<MoveValidityStrategy<Field>> &moveValidityStrategy,
                       const std::shared_ptr<ScoringStrategy<Field>> &scoringStrategy,
                       const std::shared_ptr<CollisionStrategy<Field>> &collisionStrategy) :
        Level(dimensions, moveValidityStrategy, scoringStrategy, collisionStrategy) {

}


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
    if (!this->moveValidityStrategy->isValid(this, from, to)) {
        return;
    }

    Field field = this->get(from);
    //there is an agent on this field
    if (field.agent) {
        field.agent->setWaitForGoal(field.agent->getWaitForGoal() - 1);

        if (this->scoringStrategy->isValid(this, field, to)) {
            this->scoringStrategy->scoringSideEffect(field);
        }

        Field toField = this->get(to);
        if (toField.agent) {
            this->collisionStrategy->collide(field);
        }

        toField.agent = field.agent;
        toField.agent->setLocation(to);

        //todo moveSideEffects: (T& field) => void
        //todo phero level
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
