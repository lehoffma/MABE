//
// Created by Le on 23.11.2017.
//

#ifndef MABE_ORGANISM_SCORINGSTRATEGY_H
#define MABE_ORGANISM_SCORINGSTRATEGY_H

#include <unordered_map>
#include "../../../Organism/Organism.h"
#include "../model/Agent.h"

class OrganismScoringStrategy {
public:
    virtual std::vector<double> getOrganismScores(const std::vector<shared_ptr<Agent>>& agents) = 0;
};

#endif //MABE_ORGANISM_SCORINGSTRATEGY_H
