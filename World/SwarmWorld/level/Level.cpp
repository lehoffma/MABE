//
// Created by Le on 13.11.2017.
//

#include <fstream>
#include "Level.h"
#include "../util/GridUtils.h"

template<typename T>
T &Level::get(std::pair<int, int> location) {
    if (!this->isOutOfBounds(location)) {
        return this->map[location.first][location.second];
    }
    //todo
    throw nullptr;
}

bool Level::isOutOfBounds(std::pair<int, int> location) const{
    return location.first < 0 || location.second < 0 ||
           location.first >= this->dimensions.first || location.second >= this->dimensions.second;
}

template<typename T>
Level<T> &Level::loadFromFile(std::string fileName, char separator) {
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
            converter >> this->map[col][row];
        }
    }


    return *this;
}

template<typename T>
bool Level::isFieldType(std::pair<int, int> location, FieldType fieldType){
    if(this->isOutOfBounds(location)){
        return false;
    }

    T value = this->get(location);

    return this->getFromValue(value) == fieldType;
}

template<typename T>
FieldType Level::getFromValue(const T &value) const {
    //todo
    return WALL;
}

std::pair<int, int> Level::getRelative(std::pair<int, int> location, int facing, int direction) {
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

template<>
FieldType Level::getFromValue(const int &value) const {
    switch(value){
        case 0: return WALL;
        case 1: return FLOOR;
        case 3: return START;
        case 4: return GOAL;
        default: return FLOOR;
    }
}
