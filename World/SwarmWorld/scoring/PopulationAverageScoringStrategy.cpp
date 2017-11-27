//
// Created by Le on 23.11.2017.
//

#include "PopulationAverageScoringStrategy.h"

unordered_map<int, vector<double>>
PopulationAverageScoringStrategy::getOrganismScores(const std::vector<shared_ptr<Agent>> &agents) {
    std::unordered_map<int, std::vector<double>> organismMap;

    double populationAverage = std::accumulate(agents.begin(), agents.end(), 0.0,
                                               [](const double accumulatedScore,
                                                  const shared_ptr<Agent> &agent) -> double {
                                                   return accumulatedScore + agent->getScore();
                                               }) / agents.size();

    for (const auto &agent: agents) {
        organismMap[agent->getOrganism()->ID].push_back(agent->getScore() + populationAverage);
    }

    return organismMap;
}
