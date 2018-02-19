//
// Created by Le on 19.02.2018.
//

#ifndef MABE_DISTANCESTRATEGYFACTORY_H
#define MABE_DISTANCESTRATEGYFACTORY_H

#include <functional>
#include <memory>
#include <unordered_map>
#include "DistanceStrategy.h"
#include "EuclideanDistanceStrategy.h"
#include "ManhattanDistanceStrategy.h"
#include "ChebyshevDistanceStrategy.h"

class DistanceStrategyFactory {
private:
    static const std::unordered_map<std::string, std::function<DistanceStrategy *(void)>> map;

public:

    static std::unique_ptr<DistanceStrategy> getFromString(const std::string &strategy) {
        return std::unique_ptr<DistanceStrategy>(map.at(strategy)());
    }
};


const std::unordered_map<std::string, std::function<DistanceStrategy *(void)>>
        DistanceStrategyFactory::map = {
        {"euclidean", []() -> DistanceStrategy * {
            return new EuclideanDistanceStrategy();
        }},
        {"manhattan", []() -> DistanceStrategy * {
            return new ManhattanDistanceStrategy();
        }},
        {"chebyshev", []() -> DistanceStrategy * {
            return new ChebyshevDistanceStrategy();
        }}
};


#endif //MABE_DISTANCESTRATEGYFACTORY_H
