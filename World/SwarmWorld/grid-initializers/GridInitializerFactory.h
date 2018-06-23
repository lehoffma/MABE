//
// Created by Le on 02.11.2017.
//

#ifndef MABE_GRIDINITIALIZERFACTORY_H
#define MABE_GRIDINITIALIZERFACTORY_H

#include <map>
#include <memory>
#include "GridInitializer.h"
#include "FirstAvailableInitializer.h"
#include "string"
#include "RandomInitializer.h"
#include "ChineseMilitaryInitializer.h"
#include "GreatestDistanceInitializer.h"

enum GridInitializerType {
    firstAvailable = 0,
    random = 1,
    chineseMilitary = 2,
    greatestDistance = 3
    //todo
};

class GridInitializerFactory {
private:
    static const std::map<std::string, GridInitializerType> enumMap;

public:
    static std::unique_ptr<GridInitializer> getFromEnum(GridInitializerType gridInitializerType) {
        switch (gridInitializerType) {
            case firstAvailable:
                return std::unique_ptr<GridInitializer>(new FirstAvailableInitializer());
            case random:
                return std::unique_ptr<GridInitializer>(new RandomInitializer());
            case chineseMilitary:
                return std::unique_ptr<GridInitializer>(new ChineseMilitaryInitializer());
            case greatestDistance:
                return std::unique_ptr<GridInitializer>(new GreatestDistanceInitializer());
        }
    }

    static std::unique_ptr<GridInitializer> getFromString(const std::string &gridTypeAsString) {
        //todo error handling if type is not available
        return getFromEnum(enumMap.at(gridTypeAsString));
    }
};

#endif //MABE_GRIDINITIALIZERFACTORY_H
