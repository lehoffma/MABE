//
// Created by Le on 13.11.2017.
//

#ifndef MABE_LEVEL_H
#define MABE_LEVEL_H


#include <utility>
#include <string>
#include <sstream>
#include "FieldType.h"

template<typename T>
class Level {
protected:
    std::pair<int, int> dimensions;


    FieldType getFromValue(const T& value) const;


    const int RELPOS[8][2] = {{1,  0},
                              {1,  -1},
                              {0,  -1},
                              {-1, -1},
                              {-1, 0},
                              {-1, 1},
                              {0,  1},
                              {1,  1}};
public:
    //todo
    explicit Level(std::pair<int, int> dimensions): dimensions(dimensions){

    }

    /**
     * Contains the structure of the level
     */
    T** map;

    /**
     * Fills the map with values from a given file, whose values are separated by "separator"
     * @param fileName
     * @param separator
     * @return
     */
    Level<T>& loadFromFile(std::string fileName, char separator);

    /**
     * Returns the value for the given location
     * @param location
     * @return
     */
    T& get(std::pair<int, int> location);


    /**
     * Converts a location and a relative facing/direction integer to its new absolute position
     * @param location
     * @param facing
     * @param direction
     * @return
     */
    std::pair<int, int> getRelative(std::pair<int, int> location, int facing, int direction);


    /**
     * Checks if the given location is contained inside the bounds of the internal map
     * @param location
     * @return
     */
    bool isOutOfBounds(std::pair<int, int> location) const;

    /**
     * Checks whether the field at the given position has the fieldType "fieldType"
     * @param location
     * @param fieldType
     * @return
     */
    bool isFieldType(std::pair<int, int> location, FieldType fieldType);
};



#endif //MABE_LEVEL_H
