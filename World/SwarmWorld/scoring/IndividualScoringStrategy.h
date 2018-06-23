//
// Created by Le on 23.11.2017.
//

#ifndef MABE_INDIVIDUALSCORINGSTRATEGY_H
#define MABE_INDIVIDUALSCORINGSTRATEGY_H


#include "OrganismScoringStrategy.h"

class IndividualScoringStrategy : public OrganismScoringStrategy {
public:
    std::vector<double> getOrganismScores(const std::vector<shared_ptr<Agent>>& agents) override;
};


#endif //MABE_INDIVIDUALSCORINGSTRATEGY_H
