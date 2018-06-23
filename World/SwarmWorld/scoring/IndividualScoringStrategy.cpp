//
// Created by Le on 23.11.2017.
//

#include "IndividualScoringStrategy.h"

std::vector<double> IndividualScoringStrategy::getOrganismScores(const std::vector<shared_ptr<Agent>>& agents) {
    std::vector<double> scoreList;

    for (const auto &agent: agents) {
        scoreList.emplace_back(agent->getScore());
    }

    return scoreList;
}
