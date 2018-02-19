//
// Created by Le on 12.02.2018.
//

#ifndef MABE_MULTIOBJECTIVEUTILS_H
#define MABE_MULTIOBJECTIVEUTILS_H

#include "../../Utilities/MTree.h"
#include "../../Organism/Organism.h"

namespace MultiObjective {
    /**
     * A dominates B if for all objectives M it holds true that A[m] is at least as good as B[m]
     * The objectiveMap controls which objectives should be minimized or maximized (true = minimize, false = maximize)
     * @param organismA
     * @param organismB
     * @param PT
     * @param objectiveMap
     * @return
     */
    inline bool dominates(const shared_ptr<Organism> &organismA,
                          const shared_ptr<Organism> &organismB,
                          const std::shared_ptr<ParametersTable> &PT,
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

    namespace NsgaII {

        /**
         *
         * @param solutions
         * @param frontIndices
         * @param bestValues
         * @param PT
         * @param objectiveMap
         */
        inline void crowdingDistanceAssignment(std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
                                               std::vector<int> &frontIndices,
                                               unordered_map<std::string, double> &bestValues,
                                               const std::shared_ptr<ParametersTable> &PT,
                                               const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
            for (auto const &objectiveEntry: objectiveMap) {
                auto frontCopy{frontIndices};
                const auto &objective = objectiveEntry.first;
                const auto &minimize = objectiveEntry.second;

                std::unordered_map<int, double> objectiveValues{};
                for (auto index: frontCopy) {
                    objectiveValues[index] = objective->eval(solutions[index]->organism->dataMap, PT)[0];
                }

                //  sort list by objective M
                std::sort(frontCopy.begin(), frontCopy.end(),
                          [objective, &objectiveValues, solutions, minimize](int indexA, int indexB) -> bool {
                              auto valueA = objectiveValues[indexA];
                              auto valueB = objectiveValues[indexB];
                              if (minimize) {
                                  return valueA < valueB;
                              } else {
                                  return valueB > valueA;
                              }
                          });

                //boundary points should always be selected
                solutions[frontCopy[0]]->crowdingDistance = INFINITY;
                solutions[frontCopy[frontCopy.size() - 1]]->crowdingDistance = INFINITY;

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

                for (auto i = 1; i < ((int) frontCopy.size()) - 2; i++) {
                    const auto index = frontCopy[i];
                    const auto leftIndex = frontCopy[i - 1];
                    const auto rightIndex = frontCopy[i + 1];

                    const auto &leftValue = objective->eval(solutions[leftIndex]->organism->dataMap, PT)[0];
                    const auto &rightValue = objective->eval(solutions[rightIndex]->organism->dataMap, PT)[0];
                    solutions[index]->crowdingDistance += ((rightValue - leftValue) / normalizingFactor);
                }
            }
        }

        /**
         *
         * @param solutions
         * @param objectiveMap
         * @return
         */
        inline vector<vector<int>> fastNonDominatedSort(
                std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
                const std::shared_ptr<ParametersTable> &PT,
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
                    if (MultiObjective::dominates(solutions[p]->organism, solutions[q]->organism, PT, objectiveMap)) {
                        dominatedSet[p].push_back(q);
                        dominatedBy[q]++;
                    }
                        //q dominates p
                    else if (MultiObjective::dominates(solutions[q]->organism, solutions[p]->organism, PT,
                                                       objectiveMap)) {
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

            for (auto front = 0; front < frontIndices.size(); front++) {
                for (auto j = 0; j < frontIndices[front].size(); j++) {
                    solutions[j]->rank = front + 1;
                }
            }
            return frontIndices;
        }


        /**
         *
         * @param elites
         * @param elitismTargetSize
         * @param solutions
         * @param killList
         * @param frontIndicesList
         */
        inline void eliteAssignment(std::vector<int> &elites,
                                    int elitismTargetSize,
                                    const std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
                                    std::vector<std::shared_ptr<Organism>> &killList,
                                    vector<vector<int>> frontIndicesList) {
            for (auto &front: frontIndicesList) {
                auto toBeAdded = elitismTargetSize - elites.size();
                //we have already found all elitist organisms => kill the rest
                if (toBeAdded == 0) {
                    for (auto &index: front) {
                        //organism is not part of the elites list => just add it to kill list
                        killList.push_back(solutions[index]->organism);
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
                            killList.push_back(solutions[frontCopy[i]]->organism);
                        }
                    }
                }

            }
        }
    }
}


#endif //MABE_MULTIOBJECTIVEUTILS_H
