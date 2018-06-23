//
// Created by Le on 13.11.2017.
//

#include <fstream>
#include "Level.h"
#include "../util/GridUtils.h"
#include "Field.h"
#include "../../../Optimizer/MultiObjectiveOptimizer/MapUtil.h"

template<typename T>
shared_ptr<T> Level<T>::get(const std::pair<int, int> &location) {
    if (!this->isOutOfBounds(location)) {
        //todo

        auto iter = map.find(location.first);

        if (iter != map.end()) {
            auto innerMap = iter->second;
            auto innerIter = innerMap.find(location.second);

            if (innerIter != innerMap.end()) {
                return innerIter->second;
            }
        }
    }
    return nullptr;
}

template<typename T>
bool Level<T>::isOutOfBounds(const std::pair<int, int> &location) const {
    return location.first < 0 || location.second < 0 ||
           location.first >= this->dimensions.first || location.second >= this->dimensions.second;
}

template<typename T>
Level<T> &Level<T>::loadFromFile(std::string fileName, char separator) {
    GridUtils::assignZerosMap<T>(this->map, this->dimensions.first, this->dimensions.second);

    std::ifstream file("./" + fileName);

    for (int row = 0; row < this->dimensions.second; ++row) {
        std::string line;
        std::getline(file, line);
        if (!file.good())
            break;

        std::stringstream iss(line);

        for (int col = 0; col < this->dimensions.first; ++col) {
            std::string val;
            std::getline(iss, val, separator);
            if (!iss.good())
                break;

            std::stringstream converter(val);
            this->map[col][row] = this->getValueFromFile(converter.str());
        }
    }


    return *this;
}

template<typename T>
bool Level<T>::isFieldType(const std::pair<int, int> &location, FieldType fieldType) {
    if (this->isOutOfBounds(location)) {
        return false;
    }

    if (this->get(location)) {
        return this->getFromValue(this->get(location)) == fieldType;
    }
    return false;
}

template<typename T>
FieldType Level<T>::getFromValue(const std::shared_ptr<T> &value) const {
    //todo
    return WALL;
}


template<typename T>
Level<T> &Level<T>::setMoveValidityStrategy(std::shared_ptr<MoveValidityStrategy<T>> moveValidityStrategy) {
    this->moveValidityStrategy = moveValidityStrategy;
    return *this;
}

template<typename T>
Level<T> &Level<T>::setScoringStrategies(std::vector<std::shared_ptr<ScoringStrategy<T>>> scoringStrategies) {
    this->scoringStrategies = scoringStrategies;
    return *this;
}

template<typename T>
Level<T> &Level<T>::setCollisionStrategy(std::shared_ptr<CollisionStrategy<T>> collisionStrategy) {
    this->collisionStrategy = collisionStrategy;
    return *this;
}

template<typename T>
std::shared_ptr<MoveValidityStrategy<T>> Level<T>::getMoveValidityStrategy() {
    return this->moveValidityStrategy;
}

template<typename T>
Level<T>::Level(const std::pair<int, int> &dimensions,
                std::shared_ptr<MoveValidityStrategy<T>> moveValidityStrategy,
                std::vector<std::shared_ptr<ScoringStrategy<T>>> scoringStrategies,
                std::shared_ptr<CollisionStrategy<T>> collisionStrategy) :
        dimensions(dimensions), moveValidityStrategy(moveValidityStrategy),
        scoringStrategies(scoringStrategies), collisionStrategy(collisionStrategy) {

}

template<>
shared_ptr<int> Level<int>::getValueFromFile(const std::string &fileValue) {
    return std::make_shared<int>(std::stoi(fileValue));
}

template<typename T>
shared_ptr<T> Level<T>::getValueFromFile(const std::string &fileValue) {
    throw fileValue;
}

template<>
FieldType Level<int>::getFromValue(const std::shared_ptr<int> &value) const {
    switch (*value) {
        case 0:
            return WALL;
        case 1:
            return FLOOR;
        case 3:
            return START;
        case 4:
            return GOAL;
        default:
            return FLOOR;
    }
}


template
class Level<int>;

template
class Level<Field>;