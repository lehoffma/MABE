//
// Created by Le on 23.11.2017.
//

#include "PopulationAverageScoringStrategy.h"

std::vector<double> PopulationAverageScoringStrategy::getOrganismScores(const std::vector<shared_ptr<Agent>> &agents) {
    std::vector<double> scoreList;

    const double sum = std::accumulate(agents.begin(), agents.end(), 0.0,
                                       [](const double accumulatedScore,
                                          const shared_ptr<Agent> &agent) -> double {
                                           return accumulatedScore + agent->getScore();
                                       });
    double populationAverage = sum / agents.size();

    for (const auto &agent: agents) {
//        organismMap[agent->getOrganism()->ID].push_back(agent->getScore() + populationAverage);
        scoreList.emplace_back(agent->getScore() * 0.5 + populationAverage * 0.5);
    }

    return scoreList;
}
