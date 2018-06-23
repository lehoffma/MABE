
//
// Created by Le on 15.11.2017.
//

#include "SwarmLevel.h"
#include "move/WithinBoundsStrategy.h"
#include "move/DebouncedScoringStrategy.h"
#include "move/PenaltyCollisionStrategy.h"
#include "move/SequenceScoringStrategy.h"

shared_ptr<ParameterLink<double>> SwarmLevel::rewardPL = Parameters::register_parameter(
        "WORLD_SWARM-scoringReward", 1.0,
        "the amount of score points the organism receives after walking through the goal"
);


SwarmLevel::SwarmLevel(const std::shared_ptr<ParametersTable> &_PT, const std::pair<int, int> &dimensions) : Level(
        dimensions) {
    //default values
    this->moveValidityStrategy = std::shared_ptr<MoveValidityStrategy<Field>>(new WithinBoundsStrategy<Field>());
    std::vector<FieldType> sequence{FLOOR, GOAL, FLOOR};
    this->scoringStrategies = {{
                                       std::shared_ptr<ScoringStrategy<Field>>(new DebouncedGoalStrategy()),
                                       std::shared_ptr<ScoringStrategy<Field>>(new SequenceScoringStrategy(sequence))
                               }};
    //no-op, no collision at all
    this->collisionStrategy = std::shared_ptr<CollisionStrategy<Field>>(new PenaltyCollisionStrategy<Field>(0));

    this->scoringReward = rewardPL->get(_PT);
}


SwarmLevel::SwarmLevel(const std::shared_ptr<ParametersTable> &_PT,
                       const std::pair<int, int> &dimensions,
                       const std::shared_ptr<MoveValidityStrategy<Field>> &moveValidityStrategy,
                       const std::vector<std::shared_ptr<ScoringStrategy<Field>>> &scoringStrategies,
                       const std::shared_ptr<CollisionStrategy<Field>> &collisionStrategy) :
        Level(dimensions, moveValidityStrategy, scoringStrategies, collisionStrategy) {
    this->scoringReward = rewardPL->get(_PT);
}


FieldType SwarmLevel::getFromValue(const std::shared_ptr<Field> &value) const {
    return value->fieldType;
}

FieldType getFieldTypeFromString(const std::string &value) {
    return value == "0" ? WALL
                        : value == "1" ? FLOOR
                                       : value == "3" ? START
                                                      : value == "4" ? GOAL
                                                                     : FLOOR;
}

std::shared_ptr<Field> SwarmLevel::getValueFromFile(const std::string &fileValue) {
    return make_shared<Field>(nullptr, getFieldTypeFromString(fileValue));
}

bool SwarmLevel::move(const std::pair<int, int> &from, const std::pair<int, int> &to) {
    if (!this->get(from)) {
        return false;
    }

    auto fromField = this->get(from);
    //there is an agent on this field
    if (fromField->agent) {
        fromField->agent->decrementWaitForGoal();

        //one of the fields is out of bounds
        if (!this->get(to)) {
            return false;
        }

        if (!this->moveValidityStrategy->isValid(this, from, to)) {
            return false;
        }

        auto toField = this->get(to);
        if (this->collisionStrategy->hasCollided(toField)) {
            this->collisionStrategy->collide(fromField);
            //collisions lead to an invalid move, i.e. the agent won't move at all
            return false;
        }

        //if all predicates are fulfilled
        if (std::all_of(this->scoringStrategies.begin(), this->scoringStrategies.end(),
                        [this, fromField, to](const std::shared_ptr<ScoringStrategy<Field>> strategy) {
                            return strategy->isValid(this, fromField, to);
                        })) {
            fromField->agent->setScore(fromField->agent->getScore() + scoringReward);
            for (const auto &strategy: this->scoringStrategies) {
                strategy->scoringSideEffect(fromField);
            }
        }

        toField->agent = fromField->agent;
        toField->agent->setLocation(to);

        return true;
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
    return false;
}

void SwarmLevel::reset() {
    for (int i = 0; i < this->dimensions.first; i++) {
        for (int j = 0; j < this->dimensions.second; j++) {
            if (this->map[i][j]->agent) {
                this->map[i][j]->agent.reset();
            }
            this->map[i][j]->agent = nullptr;
        }
    }
}
