//
// Created by Le on 13.11.2017.
//

#ifndef MABE_LEVEL_H
#define MABE_LEVEL_H


#include <utility>
#include <string>
#include <sstream>
#include <memory>
#include <vector>
#include <unordered_map>
#include "FieldType.h"
#include "move/MoveValidityStrategy.h"
#include "move/ScoringStrategy.h"
#include "move/CollisionStrategy.h"
#include "../model/Direction.h"

template<typename T>
class MoveValidityStrategy;

template<typename T>
class ScoringStrategy;

template<typename T>
class Level {
protected:
    std::pair<int, int> dimensions = {0, 0};

    std::shared_ptr<MoveValidityStrategy<T>> moveValidityStrategy;
    std::vector<std::shared_ptr<ScoringStrategy<T>>> scoringStrategies;
    std::shared_ptr<CollisionStrategy<T>> collisionStrategy;

    virtual FieldType getFromValue(const std::shared_ptr<T> &value) const;


    virtual std::shared_ptr<T> getValueFromFile(const std::string &fileValue);

    /**
     * Contains the structure of the level
     */
    std::unordered_map<int, std::unordered_map<int, std::shared_ptr<T>>> map;
public:
    Level() {
        this->dimensions.first = 0;
        this->dimensions.second = 0;
    }

    //todo
    explicit Level(std::pair<int, int> dimensions) : dimensions(dimensions) {

    }

    Level(const std::pair<int, int> &dimensions,
          std::shared_ptr<MoveValidityStrategy<T>> moveValidityStrategy,
          std::vector<std::shared_ptr<ScoringStrategy<T>>> scoringStrategies,
          std::shared_ptr<CollisionStrategy<T>> collisionStrategy
    );

    /**
     * Reset the map
     */
    virtual void reset() = 0;

    /**
     * Fills the map with values from a given file, whose values are separated by "separator"
     * @param fileName
     * @param separator
     * @return
     */
    Level<T> &loadFromFile(std::string fileName, char separator);

    /**
     * Returns the value for the given location
     * @param location
     * @return
     */
    std::shared_ptr<T> get(const std::pair<int, int> &location);

    /**
     * Moves the map values from one place to another. (for example: an agent)
     * @param from
     * @param to
     * @return true if the move succeeded, false otherwise (either because it was invalid or out of bounds)
     */
    virtual bool move(const std::pair<int, int> &from, const std::pair<int, int> &to) = 0;


    /**
     * Checks if the given location is contained inside the bounds of the internal map
     * @param location
     * @return
     */
    bool isOutOfBounds(const std::pair<int, int> &location) const;

    /**
     * Checks whether the field at the given position has the fieldType "fieldType"
     * @param location
     * @param fieldType
     * @return
     */
    bool isFieldType(const std::pair<int, int> &location, FieldType fieldType);


    /**
     *
     * @param moveValidityStrategy
     * @return
     */
    Level &setMoveValidityStrategy(std::shared_ptr<MoveValidityStrategy<T>> moveValidityStrategy);

    Level &setScoringStrategies(std::vector<std::shared_ptr<ScoringStrategy<T>>> scoringStrategies);

    Level &setCollisionStrategy(std::shared_ptr<CollisionStrategy<T>> collisionStrategy);

    std::shared_ptr<MoveValidityStrategy<T>> getMoveValidityStrategy();
};


#endif //MABE_LEVEL_H
