//
// Created by Le on 15.11.2017.
//

#ifndef MABE_SWARMLEVEL_H
#define MABE_SWARMLEVEL_H


#include "Field.h"
#include "Level.h"
#include <memory>

class SwarmLevel : public Level<Field> {
public:
    static std::shared_ptr<ParameterLink<double>> rewardPL;
    double scoringReward;

    explicit SwarmLevel(const std::shared_ptr<ParametersTable> &_PT,
            const std::pair<int, int> &dimensions);

    SwarmLevel(const std::shared_ptr<ParametersTable> &_PT,
               const std::pair<int, int> &dimensions,
               const std::shared_ptr<MoveValidityStrategy<Field>> &moveValidityStrategy,
               const std::vector<std::shared_ptr<ScoringStrategy<Field>>>& scoringStrategies,
               const std::shared_ptr<CollisionStrategy<Field>> &collisionStrategy
    );

    bool move(const std::pair<int, int> &from, const std::pair<int, int> &to) override;

    void reset() override;

protected:
    FieldType getFromValue(const Field &value) const override;

    Field getValueFromFile(const std::string &fileValue) override;
};


#endif //MABE_SWARMLEVEL_H
