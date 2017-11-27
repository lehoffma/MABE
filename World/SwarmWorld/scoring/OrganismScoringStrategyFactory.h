//
// Created by Le on 23.11.2017.
//

#ifndef MABE_ORGANISMSCORINGSTRATEGYFACTORY_H
#define MABE_ORGANISMSCORINGSTRATEGYFACTORY_H

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

const std::unordered_map<std::string, std::function<OrganismScoringStrategy *(void)>>
        OrganismScoringStrategyFactory::map = {
        {"individual",        []() -> OrganismScoringStrategy * {
            return new IndividualScoringStrategy();
        }},
        {"populationAverage", []() -> OrganismScoringStrategy * {
            return new PopulationAverageScoringStrategy();
        }}
};


#endif //MABE_ORGANISMSCORINGSTRATEGYFACTORY_H
