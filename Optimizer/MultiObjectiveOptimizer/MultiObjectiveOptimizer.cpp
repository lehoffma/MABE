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
