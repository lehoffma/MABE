//
// Created by Le on 13.11.2017.
//

#include <fstream>
#include "Level.h"
#include "../util/GridUtils.h"
#include "Field.h"

template<typename T>
T &Level<T>::get(std::pair<int, int> location) {
    if (!this->isOutOfBounds(location)) {
        return this->map[location.first][location.second];
    }
    //todo
    throw nullptr;
}

template<typename T>
bool Level<T>::isOutOfBounds(std::pair<int, int> location) const {
    return location.first < 0 || location.second < 0 ||
           location.first >= this->dimensions.first || location.second >= this->dimensions.second;
}

template<typename T>
Level<T> &Level<T>::loadFromFile(std::string fileName, char separator) {
    this->map = GridUtils::zeros<T>(this->dimensions.first, this->dimensions.second);

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
bool Level<T>::isFieldType(std::pair<int, int> location, FieldType fieldType) {
    if (this->isOutOfBounds(location)) {
        return false;
    }

    T value = this->get(location);

    return this->getFromValue(value) == fieldType;
}

template<typename T>
FieldType Level<T>::getFromValue(const T &value) const {
    //todo
    return WALL;
}

template<typename T>
std::pair<int, int> Level<T>::getRelative(std::pair<int, int> location, int facing, int direction) {
    //todo huh?
    int dir = ((facing + direction - 1) % 8) - 1;
    if (dir == -1) {
        dir = 7;
    }

    return {
            location.first + Level::RELPOS[dir][0],
            location.second + Level::RELPOS[dir][1]
    };
}

template<typename T>
Level<T> &Level<T>::setMoveValidityStrategy(std::shared_ptr<MoveValidityStrategy<T>> moveValidityStrategy) {
    this->moveValidityStrategy = moveValidityStrategy;
    return *this;
}

template<typename T>
Level<T> &Level<T>::setScoringStrategy(std::shared_ptr<ScoringStrategy<T>> scoringStrategy) {
    this->scoringStrategy = scoringStrategy;
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
                std::shared_ptr<ScoringStrategy<T>> scoringStrategy,
                std::shared_ptr<CollisionStrategy<T>> collisionStrategy) :
        dimensions(dimensions), moveValidityStrategy(moveValidityStrategy),
        scoringStrategy(scoringStrategy), collisionStrategy(collisionStrategy) {

}

template<>
int Level<int>::getValueFromFile(const std::string &fileValue) {
    return std::stoi(fileValue);
}

template<typename T>
T Level<T>::getValueFromFile(const std::string &fileValue) {
    throw fileValue;
}

template<>
FieldType Level<int>::getFromValue(const int &value) const {
    switch (value) {
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


template class Level<int>;
template class Level<Field>;