//
// Created by Le on 23.11.2017.
//

#include "PopulationAverageScoringStrategy.h"

unordered_map<int, vector<double>>
PopulationAverageScoringStrategy::getOrganismScores(const std::vector<shared_ptr<Agent>> &agents) {
    std::unordered_map<int, std::vector<double>> organismMap;

    const double sum = std::accumulate(agents.begin(), agents.end(), 0.0,
                                       [](const double accumulatedScore,
                                          const shared_ptr<Agent> &agent) -> double {
                                           return accumulatedScore + agent->getScore();
                                       });
    double populationAverage = sum / agents.size();

    for (const auto &agent: agents) {
//        organismMap[agent->getOrganism()->ID].push_back(agent->getScore() + populationAverage);
        organismMap[agent->getOrganism()->ID].push_back(agent->getScore() * 0.5 + populationAverage * 0.5);
    }

    return organismMap;
}
