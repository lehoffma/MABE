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
        "OPTIMIZER_MULTI-maxObjectives", (string) "DM_AVE[score]",
        "Comma-separated list of MTree values that will be maximized by the optimizer");

shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::elitismCountPL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-elitismCount", (string) "1",
        "number of elite organisms to be added (MTree)");

shared_ptr<ParameterLink<string>> MultiObjectiveOptimizer::nextPopSizePL = Parameters::register_parameter(
        "OPTIMIZER_MULTI-nextPopSize", (string) "0-1",
        "size of population after optimization(MTree). -1 indicates use current population size");


void
MultiObjectiveOptimizer::crowdingDistanceAssignment(std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
                                                    std::vector<int> &frontIndices,
                                                    unordered_map<std::string, double> &bestValues,
                                                    const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {


    for (auto const &objectiveEntry: objectiveMap) {
        const auto &objective = objectiveEntry.first;
        const auto &minimize = objectiveEntry.second;
        //  sort list by objective M
        std::sort(frontIndices.begin(), frontIndices.end(),
                  [objective, solutions, minimize, this](int indexA, int indexB) -> bool {
                      const auto &solutionA = solutions[indexA];
                      const auto &solutionB = solutions[indexB];
                      const auto &valueA = objective->eval(solutionA->organism->dataMap, PT)[0];
                      const auto &valueB = objective->eval(solutionB->organism->dataMap, PT)[0];
                      if (minimize) {
                          return valueA < valueB;
                      } else {
                          return valueB > valueA;
                      }
                  });

        //boundary points should always be selected
        solutions[frontIndices[0]]->crowdingDistance = INFINITY;
        solutions[frontIndices[frontIndices.size() - 1]]->crowdingDistance = INFINITY;

        auto minValue = INFINITY;
        auto maxValue = -INFINITY;
        for (auto &solution: solutions) {
            auto value = objective->eval(solution->organism->dataMap, PT)[0];
            if (value < minValue) {
                minValue = value;
            }
            if (value > maxValue) {
                maxValue = value;
            }
        }
        if (minimize) {
            if (minValue < bestValues[objective->getFormula()]) {
                bestValues[objective->getFormula()] = minValue;
            }
        } else {
            if (maxValue > bestValues[objective->getFormula()]) {
                bestValues[objective->getFormula()] = maxValue;
            }
        }

        auto normalizingFactor = maxValue - minValue;
        if (normalizingFactor == 0) {
            normalizingFactor = 1;
        }

        for (auto i = 1; i < ((int) frontIndices.size()) - 2; i++) {
            const auto index = frontIndices[i];
            const auto leftIndex = frontIndices[i - 1];
            const auto rightIndex = frontIndices[i + 1];

            const auto &leftValue = objective->eval(solutions[leftIndex]->organism->dataMap, PT)[0];
            const auto &rightValue = objective->eval(solutions[rightIndex]->organism->dataMap, PT)[0];
            solutions[index]->crowdingDistance += ((rightValue - leftValue) / normalizingFactor);
        }
    }
}

bool MultiObjectiveOptimizer::dominates(const shared_ptr<Organism> &organismA, const shared_ptr<Organism> &organismB,
                                        const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {

    bool atLeastOneGreaterThan = false;
    for (auto const &objectiveEntry : objectiveMap) {
        bool dominates;
        auto objective = objectiveEntry.first;
        auto minimize = objectiveEntry.second;
        //second controls whether the objective should be minimized or maximized
        //true meaning minimized, whereas false means maximized
        auto valueA = objective->eval(organismA->dataMap, PT)[0];
        auto valueB = objective->eval(organismB->dataMap, PT)[0];
        //minimize value
        if (minimize) {
            atLeastOneGreaterThan = atLeastOneGreaterThan || valueA < valueB;
            dominates = valueA <= valueB;
        }
            //maximize value
        else {
            atLeastOneGreaterThan = atLeastOneGreaterThan || valueA > valueB;
            dominates = valueA >= valueB;
        }
        //at least one of the objectives is not dominated => A doesn't dominate B
        if (!dominates) {
            return false;
        }
    }
    //A only dominates B if all objectives are dominated and the organism's objective value is at least once > (or <)
    return atLeastOneGreaterThan;
}


vector<vector<int>> MultiObjectiveOptimizer::fastNonDominatedSort(
        std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
        const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
) {
    std::unordered_map<int, std::vector<int>> dominatedSet{};
    std::unordered_map<int, int> dominatedBy{};
    std::vector<std::vector<int>> frontIndices{};
    frontIndices.emplace_back(std::vector<int>());

    const auto length = solutions.size();
    for (auto p = 0; p < length; p++) {
        for (auto q = p + 1; q < length; q++) {
            //p dominates q
            if (this->dominates(solutions[p]->organism, solutions[q]->organism, objectiveMap)) {
                dominatedSet[p].push_back(q);
                dominatedBy[q]++;
            }
                //q dominates p
            else if (this->dominates(solutions[q]->organism, solutions[p]->organism, objectiveMap)) {
                dominatedSet[q].push_back(p);
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
        frontIndices.push_back(H);
    }

    //remove the empty front at the end
    frontIndices.pop_back();

//    std::vector<std::vector<int>> fronts{};
    for (auto front = 0; front < frontIndices.size(); front++) {
//        //safety measure to avoid nullpointers
//        if (fronts.size() <= front) {
//            fronts.emplace_back();
//        }
        for (auto j = 0; j < frontIndices[front].size(); j++) {
            solutions[j]->rank = front + 1;
//            fronts[front].push_back(solutions[j]->organism->ID);
        }
    }
    return frontIndices;
}

MultiObjectiveOptimizer::MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT) : AbstractOptimizer(_PT) {
    numberParents = numberParentsPL->get(PT);

    elitismCountMT = stringToMTree(elitismCountPL->get(PT));
    nextPopSizeMT = stringToMTree(nextPopSizePL->get(PT));

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

void MultiObjectiveOptimizer::optimize(vector<shared_ptr<Organism>> &population) {
    std::unordered_map<std::string, double> bestValues{};
    std::vector<std::shared_ptr<MultiObjectiveSolution>> solutions{};
    for (auto &organism: population) {
        solutions.push_back(make_shared<MultiObjectiveSolution>(organism, 0, 0));
    }
    std::unordered_map<shared_ptr<Abstract_MTree>, bool> objectiveMap{};
    for (auto &minimizedObjective: minimizedObjectives) {
        objectiveMap[minimizedObjective] = true;
    }
    for (auto &maximizedObjective: maximizedObjectives) {
        objectiveMap[maximizedObjective] = false;
    }

    auto popTargetSize = (int) nextPopSizeMT->eval(PT)[0];
    if (popTargetSize == -1) {
        popTargetSize = static_cast<int>(population.size());
    }
    killList.clear();

    //extract fronts from the solutions
    auto frontIndicesList = this->fastNonDominatedSort(solutions, objectiveMap);
    for (auto &frontIndices: frontIndicesList) {
        //assign crowding distance to solutions
        this->crowdingDistanceAssignment(solutions, frontIndices, bestValues, objectiveMap);
    }

    auto elitismTargetSize = (int) elitismCountMT->eval(PT)[0];
    std::vector<int> elites{};

    for (auto &front: frontIndicesList) {
        auto toBeAdded = elitismTargetSize - elites.size();
        //we have already found all elitist organisms
        if (toBeAdded == 0) {
            for (auto &index: front) {
                killList.insert(solutions[index]->organism);
            }
        }
            //if all solutions of a front fit into the list, just add them
        else if (front.size() < toBeAdded) {
            for (auto &index: front) {
                elites.push_back(index);
            }
        }
            //if not every solutions fits, only add the first N solutions of the sorted list
        else {
            auto frontCopy{front};

            //sort list by >= ordering defined by Deb et al.
            std::sort(frontCopy.begin(), frontCopy.end(),
                      [solutions](int indexA,
                                  int indexB) -> bool {
                          const auto solutionA = solutions[indexA];
                          const auto solutionB = solutions[indexB];
                          return *solutionA >= *solutionB;
                      });

            for (auto i = 0; i < frontCopy.size(); i++) {
                //add elements until size is reached
                if (i < toBeAdded) {
                    elites.push_back(frontCopy[i]);
                }
                    //the rest of the front is put into the kill list
                else {
                    killList.insert(solutions[frontCopy[i]]->organism);
                }
            }
        }

    }
    std::vector<std::shared_ptr<Organism>> children{};
    while ((elites.size() + children.size()) < popTargetSize) {
        vector<shared_ptr<Organism>> parents{};
        while (parents.size() < numberParents) {
            //  binary tournament selection
            parents.push_back(this->binaryTournamentSelection(solutions));
        }
        //  recombination and mutation
        children.push_back(parents[0]->makeMutatedOffspringFromMany(parents));
    }

    //add generated children to the new population
    for (auto &child: children) {
        population.push_back(child);
    }

    //for every minimized and maximized objective: write best values to output stream
    std::stringstream ss{};
    for (auto &entry: bestValues) {
        ss << "best " << entry.first << " value: " << entry.second << " | ";
    }
    std::cout << ss.str() << std::endl;
}

std::shared_ptr<Organism> MultiObjectiveOptimizer::binaryTournamentSelection(
        std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions
) {
    auto popSize = static_cast<const int>(solutions.size());
    int first = Random::getIndex(popSize);
    int second = 0;
    //generate random values until a different one was found
    while ((second = Random::getIndex(popSize)) == first) {}
    auto valueA = *solutions[first];
    auto valueB = *solutions[second];

    return solutions[(valueA >= valueB ? first : second)]->organism;
}

