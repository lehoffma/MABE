//
// Created by Le on 15.11.2017.
//

#ifndef MABE_SWARMLEVEL_H
#define MABE_SWARMLEVEL_H


#include "Level.h"
#include "Field.h"

class SwarmLevel : public Level<Field> {
public:
    explicit SwarmLevel(const std::pair<int, int> &dimensions);

    SwarmLevel(const std::pair<int, int> &dimensions,
               std::unique_ptr<MoveValidityStrategy<Field>> moveValidityStrategy,
               std::unique_ptr<ScoringStrategy<Field>> scoringStrategy,
               std::unique_ptr<CollisionStrategy<Field>> collisionStrategy
    );

    void move(const std::pair<int, int> &from, const std::pair<int, int> &to) override;

protected:
    FieldType getFromValue(const Field &value) const override;

    Field getValueFromFile(const std::string &fileValue) override;
};


#endif //MABE_SWARMLEVEL_H
