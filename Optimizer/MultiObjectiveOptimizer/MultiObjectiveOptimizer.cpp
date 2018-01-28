//
// Created by Le on 22.01.2018.
//

#include "MultiObjectiveOptimizer.h"


shared_ptr<ParameterLink<int>> MultiObjectiveOptimizer::numberParentsPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-numberParents", 1, "number of parents used to produce offspring");

shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::minimizeObjectivesPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-minObjectives", (string) "",
        "Comma-separated list of MTree values that will be minimized by the optimizer");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::maximizeObjectivesPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-maxObjectives", (string) "",
        "Comma-separated list of MTree values that will be maximized by the optimizer");

shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::surviveRatePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-surviveRate", (string) "0",
        "value between 0 and 1, likelyhood that an organism will self (ignored if numberParents = 1) (MTree)");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::selfRatePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-selfRate", (string) "0",
        "value between 0 and 1, liklyhood that an organism will survive (MTree)");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::elitismCountPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-elitismCount", (string) "1",
        "number of mutated offspring added to next population for each elite organism (MTree)");
shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::elitismRangePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-elitismRange", (string) "0", "number of elite organisms (i.e. if 5, then best 5) (MTree)");

shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::nextPopSizePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-nextPopSize", (string) "0-1",
        "size of population after optimization(MTree). -1 indicates use current population size");


void
MultiObjectiveOptimizer::crowdingDistanceAssignment(std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
                                                    const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
    for (auto const &objectiveEntry: objectiveMap) {
        const auto &objective = objectiveEntry.first;
        const auto &minimize = objectiveEntry.second;
        //  sort list by objective M
        std::sort(solutions.begin(), solutions.end(),
                  [objective, minimize](MultiObjectiveSolution &solutionA, MultiObjectiveSolution &solutionB) -> bool {
                      const auto &valueA = objective->eval(solutionA.organism->dataMap, PT)[0];
                      const auto &valueB = objective->eval(solutionB.organism->dataMap, PT)[0];
                      if (minimize) {
                          return valueA < valueB;
                      } else {
                          return valueB > valueA;
                      }
                  });

        //boundary points should always be selected
        solutions[0]->crowdingDistance = INFINITY;
        solutions[solutions.size() - 1]->crowdingDistance = INFINITY;

        for (auto i = 1; i < solutions.size() - 2; i++) {
            const auto &leftValue = objective->eval(solutions[i - 1]->organism->dataMap, PT)[0];
            const auto &rightValue = objective->eval(solutions[i + 1]->organism->dataMap, PT)[0];
            solutions[i]->crowdingDistance += (rightValue - leftValue);
        }
    }
}

bool MultiObjectiveOptimizer::dominates(const shared_ptr<Organism> &organismA, const shared_ptr<Organism> &organismB,
                                        const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {

    for (auto const &objectiveEntry : objectiveMap) {
        auto dominates;
        auto objective = objectiveEntry.first;
        auto minimize = objectiveEntry.second;
        //second controls whether the objective should be minimized or maximized
        //true meaning minimized, whereas false means maximized
        //minimize value
        if (minimize) {
            dominates = objective->eval(organismA->dataMap, PT)[0] <= objective->eval(organismB->dataMap, PT)[0];
        }
            //maximize value
        else {
            dominates = objective->eval(organismA->dataMap, PT)[0] >= objective->eval(organismB->dataMap)[0];
        }
        //at least one of the objectives is not dominated => A doesn't dominate B
        if (!dominates) {
            return false;
        }
    }
    //A only dominates B if all objectives are dominated
    return true;
}


vector<vector<shared_ptr<MultiObjectiveSolution>>> MultiObjectiveOptimizer::fastNonDominatedSort(
        std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
        const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
) {
    std::unordered_map<int, std::vector<int>> dominatedSet{};
    std::unordered_map<int, int> dominatedBy{};
    std::vector<std::vector<int>> frontIndices{};

    const auto length = solutions.size();
    for (auto p = 0; p < length; p++) {
        for (auto q = 0; q < length; q++) {
            //ignore same organisms
            if (p == q) {
                continue;
            }
            //p dominates q
            if (this->dominates(solutions[p]->organism, solutions[q]->organism, objectiveMap)) {
                dominatedSet[p].push_back(q);
                dominatedBy[q]++;
            }
            //q dominates p
            if (this->dominates(solutions[q]->organism, solutions[p]->organism, objectiveMap)) {
                dominatedBy[p]++;
            }
        }
        //no solution dominates p => add it to the first front
        if (dominatedBy[p] == 0) {
            frontIndices[0].push_back(p);
        }
    }
    auto i = 0;
    //fill up the fronts
    while (!frontIndices[i].empty()) {
        std::vector<int> H{};

        for (auto p: frontIndices[i]) {
            for (auto q: dominatedSet[p]) {
                dominatedBy[q]--;
                if (dominatedBy[q] == 0) {
                    H.push_back(q);
                }
            }
        }

        i++;
        frontIndices[i] = H;
    }

    std::vector<std::vector<std::shared_ptr<MultiObjectiveSolution>>> fronts{};
    for (auto front = 0; front < frontIndices.size(); front++) {
        //safety measure to avoid nullpointers
        if (fronts.size() <= front) {
            fronts.emplace_back();
        }
        for (auto j = 0; j < frontIndices[front].size(); j++) {
            solutions[j]->rank = front + 1;
            fronts[front].push_back(make_shared(solutions[j]));
        }
    }
    return fronts;
}

MultiObjectiveOptimizer::MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT) : AbstractOptimizer(_PT) {
    numberParents = numberParentsPL->get(PT);

    surviveRateMT = stringToMTree(surviveRatePL->get(PT));
    selfRateMT = stringToMTree(selfRatePL->get(PT));
    elitismCountMT = stringToMTree(elitismCountPL->get(PT));
    elitismRangeMT = stringToMTree(elitismRangePL->get(PT));
    nextPopSizeMT = stringToMTree(nextPopSizePL->get(PT));

    convertCSVListToVector(minimizeObjectivesPL->get(PT), minimizeObjectives, ',');
    convertCSVListToVector(maximizeObjectivesPL->get(PT), maximizeObjectives, ',');
}

void MultiObjectiveOptimizer::optimize(vector<shared_ptr<Organism>> &population) {
    std::vector<std::shared_ptr<MultiObjectiveSolution>> solutions{};
    for (auto &organism: population) {
        solutions.push_back(make_shared(new MultiObjectiveSolution(organism, 0, 0)));
    }
    std::unordered_map<shared_ptr<Abstract_MTree>, bool> objectiveMap{};
    for (auto &minimizedObjective: minimizeObjectives) {
        objectiveMap[minimizedObjective] = true;
    }
    for (auto &maximizedObjective: maximizedObjective) {
        objectiveMap[maximizedObjective] = false;
    }


    //extract fronts from the solutions
    auto fronts = this->fastNonDominatedSort(solutions, objectiveMap);
    //assign crowding distance to solutions
    this->crowdingDistanceAssignment(solutions, objectiveMap);



    //todo wenn das paper wieder verfügbar is..
    //assign crowding distance to solutions
    //??
    //  select population based on <= until popSize is reached?
    //
}
