//
// Created by Le on 23.11.2017.
//

#include "IndividualScoringStrategy.h"

unordered_map<int, std::vector<double>> IndividualScoringStrategy::getOrganismScores(const std::vector<shared_ptr<Agent>>& agents) {
    std::unordered_map<int, std::vector<double>> organismMap;

    for (const auto &agent: agents) {
        organismMap[agent->getOrganism()->ID].push_back(agent->getScore());
    }

    return organismMap;
}
