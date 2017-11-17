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
    explicit SwarmLevel(const std::pair<int, int> &dimensions);

    SwarmLevel(const std::pair<int, int> &dimensions,
               const std::shared_ptr<MoveValidityStrategy<Field>> &moveValidityStrategy,
               const std::shared_ptr<ScoringStrategy<Field>> &scoringStrategy,
               const std::shared_ptr<CollisionStrategy<Field>> &collisionStrategy
    );

    void move(const std::pair<int, int> &from, const std::pair<int, int> &to) override;

    void reset() override;

protected:
    FieldType getFromValue(const Field &value) const override;

    Field getValueFromFile(const std::string &fileValue) override;
};


#endif //MABE_SWARMLEVEL_H
