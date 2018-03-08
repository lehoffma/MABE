//
// Created by Le on 18.02.2018.
//

#include "MultiObjectiveOptimizer.h"


shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::minimizeObjectivesPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-minObjectives", (string) "",
        "Comma-separated list of MTree values that will be minimized by the optimizer");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::maximizeObjectivesPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-maxObjectives", (string) "DM_AVE[score]",
        "Comma-separated list of MTree values that will be maximized by the optimizer");


MultiObjectiveOptimizer::MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT) : AbstractOptimizer(_PT) {
    std::vector<std::string> minimizeCsv{};
    std::vector<std::string> maximizeCsv{};
    convertCSVListToVector(minimizeObjectivesPL->get(PT), minimizeCsv, ',');
    convertCSVListToVector(maximizeObjectivesPL->get(PT), maximizeCsv, ',');
    for (const auto &minimize: minimizeCsv) {
        minimizedObjectives.push_back(stringToMTree(minimize));
    }
    for (const auto &maximize: maximizeCsv) {
        maximizedObjectives.push_back(stringToMTree(maximize));
    }
}

void MultiObjectiveOptimizer::initObjectiveMap(std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
    for (auto &minimizedObjective: minimizedObjectives) {
        objectiveMap[minimizedObjective] = true;
    }
    for (auto &maximizedObjective: maximizedObjectives) {
        objectiveMap[maximizedObjective] = false;
    }
}

std::string MultiObjectiveOptimizer::serializeObjectiveScores(
        std::vector<std::shared_ptr<Organism>> &population,
        std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
) {
    std::stringstream ss{};
    auto i = 0;
    for (auto &objectiveEntry: objectiveMap) {
        auto objective = objectiveEntry.first;
        auto minimize = objectiveEntry.second;
        double bestObjectiveValue = (minimize ? 1 : -1) * INFINITY;
        double valueSum = 0;
        for (auto &org: population) {
            auto value = objective->eval(org->dataMap, PT)[0];
            if (minimize) {
                bestObjectiveValue = std::min(bestObjectiveValue, value);
            } else {
                bestObjectiveValue = std::max(bestObjectiveValue, value);
            }
            valueSum += value;
        }
        auto average = valueSum / population.size();
        ss << "best " << objective->getFormula() << ": " << bestObjectiveValue
           << " | avg " << objective->getFormula() << ": " << average;

        if (i < objectiveMap.size()) {
            ss << " || ";
        }
        i++;
    }

    return ss.str();
}
