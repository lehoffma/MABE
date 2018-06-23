//
// Created by Le on 11.06.2018.
//

#include "OrganismScoringStrategyFactory.h"

const std::unordered_map<std::string, std::function<OrganismScoringStrategy *(void)>>
        OrganismScoringStrategyFactory::map = {
        {"individual",        []() -> OrganismScoringStrategy * {
            return new IndividualScoringStrategy();
        }},
        {"populationAverage", []() -> OrganismScoringStrategy * {
            return new PopulationAverageScoringStrategy();
        }}
};

