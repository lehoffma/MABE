//
// Created by Le on 23.11.2017.
//

#ifndef MABE_ORGANISMSCORINGSTRATEGYFACTORY_H
#define MABE_ORGANISMSCORINGSTRATEGYFACTORY_H

#include <functional>
#include "OrganismScoringStrategy.h"
#include "IndividualScoringStrategy.h"
#include "PopulationAverageScoringStrategy.h"

class OrganismScoringStrategyFactory {
private:
    static const std::unordered_map<std::string, std::function<OrganismScoringStrategy *(void)>> map;
public:

    static std::unique_ptr<OrganismScoringStrategy> getFromString(const std::string &strategy) {
        return std::unique_ptr<OrganismScoringStrategy>(map.at(strategy)());
    }
};

#endif //MABE_ORGANISMSCORINGSTRATEGYFACTORY_H
