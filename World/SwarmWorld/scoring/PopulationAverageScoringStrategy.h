//
// Created by Le on 23.11.2017.
//

#ifndef MABE_POPULATIONAVERAGESCORINGSTRATEGY_H
#define MABE_POPULATIONAVERAGESCORINGSTRATEGY_H


#include "OrganismScoringStrategy.h"

class PopulationAverageScoringStrategy: public OrganismScoringStrategy{
    std::vector<double> getOrganismScores(const std::vector<shared_ptr<Agent>>& agents) override;
};


#endif //MABE_POPULATIONAVERAGESCORINGSTRATEGY_H
