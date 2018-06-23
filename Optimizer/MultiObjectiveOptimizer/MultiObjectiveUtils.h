//
// Created by Le on 12.02.2018.
//

#ifndef MABE_MULTIOBJECTIVEUTILS_H
#define MABE_MULTIOBJECTIVEUTILS_H

#include <deque>
#include "../../Utilities/MTree.h"
#include "../../Organism/Organism.h"
#include "../../World/SwarmWorld/util/CompareUtils.h"
#include "MultiObjectiveSolution.h"
#include "ObjectivePoint.h"
#include "ObjectiveUtil.h"
#include "MapUtil.h"

namespace MultiObjective {

    /**
     * A dominates B if for all objectives M it holds true that A[m] is at least as good as B[m]
     * The objectiveMap controls which objectives should be minimized or maximized (true = minimize, false = maximize)
     * @param pointA
     * @param pointB
     * @param PT
     * @param objectiveMap
     * @return
     */
    inline bool dominates(const ObjectivePoint &pointA,
                          const ObjectivePoint &pointB,
                          const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {


        bool atLeastOneGreaterThan = false;
        for (auto const &objectiveEntry : objectiveMap) {
            bool dominates;

            auto objective = objectiveEntry.first->getFormula();
            auto minimize = objectiveEntry.second;

            //second controls whether the objective should be minimized or maximized
            //true meaning minimized, whereas false means maximized
            auto valueA = Map::get(pointA, objective);
            auto valueB = Map::get(pointB, objective);

            bool valuesAreEqual = CompareUtils::almostEqual(valueA, valueB, 2);

            //minimize value
            if (minimize) {
                bool valueIsLessThan = !valuesAreEqual && valueA < valueB;
                atLeastOneGreaterThan = atLeastOneGreaterThan || valueIsLessThan;
                dominates = valueIsLessThan || valuesAreEqual;
            }
                //maximize value
            else {
                bool valueIsGreaterThan = !valuesAreEqual && valueA > valueB;
                atLeastOneGreaterThan = atLeastOneGreaterThan || valueIsGreaterThan;
                dominates = valueIsGreaterThan || valuesAreEqual;
            }

            //at least one of the objectives is not dominated => A doesn't dominate B
            if (!dominates) {
                return false;
            }
        }
        //A only dominates B if all objectives are dominated and the organism's objective value is at least once > (or <)
        return atLeastOneGreaterThan;
    }

    inline bool anyDominates(const std::vector<ObjectivePoint> &points,
                             const ObjectivePoint &otherPoint,
                             const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        for (const auto &point: points) {
            if (dominates(point, otherPoint, objectiveMap)) {
                return true;
            }
        }
        return false;
    }

    inline ObjectivePoint organismToPoint(const shared_ptr<Organism> &organism,
                                          const std::shared_ptr<ParametersTable> &PT,
                                          const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        ObjectivePoint point{};
        for (auto const &objectiveEntry : objectiveMap) {
            auto objective = objectiveEntry.first;
            point[objective->getFormula()] = objective->eval(organism->dataMap, PT)[0];
        }
        return point;
    }

    inline std::vector<ObjectivePoint> populationToPoints(const std::vector<std::shared_ptr<Organism>> &population,
                                                          const std::shared_ptr<ParametersTable> &PT,
                                                          const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        std::vector<ObjectivePoint> points{};

        for (auto const &organism : population) {
            points.emplace_back(organismToPoint(organism, PT, objectiveMap));
        }

        return points;
    }

    inline std::vector<ObjectivePoint> frontToPoints(const std::vector<std::shared_ptr<Organism>> &population,
                                                     const vector<int> &optimalFrontIndices,
                                                     const std::shared_ptr<ParametersTable> &PT,
                                                     const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        std::vector<ObjectivePoint> points{};

        for (auto const index: optimalFrontIndices) {
            points.emplace_back(organismToPoint(population[index], PT, objectiveMap));
        }
        return points;
    }

    /**
     * A dominates B if for all objectives M it holds true that A[m] is at least as good as B[m]
     * The objectiveMap controls which objectives should be minimized or maximized (true = minimize, false = maximize)
     * @param organismA
     * @param organismB
     * @param PT
     * @param objectiveMap
     * @return
     */
    inline bool dominatesOrganism(const shared_ptr<Organism> &organismA,
                                  const shared_ptr<Organism> &organismB,
                                  const std::shared_ptr<ParametersTable> &PT,
                                  const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
        return MultiObjective::dominates(
                MultiObjective::organismToPoint(organismA, PT, objectiveMap),
                MultiObjective::organismToPoint(organismB, PT, objectiveMap),
                objectiveMap
        );
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
                                               const std::vector<int> &frontIndices,
                                               const std::shared_ptr<ParametersTable> &PT,
                                               const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap,
                                               bool infiniteBoundaryDistance = true
        ) {

            for (auto const &objectiveEntry: objectiveMap) {
                auto frontCopy{frontIndices};
                const auto objective = objectiveEntry.first;

                std::unordered_map<int, double> objectiveValues{};
                for (auto index: frontCopy) {
                    objectiveValues[index] = objective->eval(solutions[index]->organism->dataMap, PT)[0];
                }

                //  sort list by objective M in ascending order
                std::sort(frontCopy.begin(), frontCopy.end(),
                          [objective, &objectiveValues](int indexA, int indexB) -> bool {
                              auto valueA = Map::get(objectiveValues, indexA);
                              auto valueB = Map::get(objectiveValues, indexB);
                              return valueB > valueA;
                          });


                double minValue = INFINITY;
                double maxValue = -INFINITY;
                for (auto index: frontCopy) {
                    auto value = Map::get(objectiveValues, index);
                    if (value < minValue) {
                        minValue = value;
                    }
                    if (value > maxValue) {
                        maxValue = value;
                    }
                }

                auto frontSize = (int) frontCopy.size();

                auto normalizingFactor = maxValue - minValue;
                if (normalizingFactor == 0) {
                    normalizingFactor = 1;
                }

                for (auto i = 0; i < frontSize - 1; i++) {
                    const auto index = frontCopy[i];
                    int leftIndex;
                    int rightIndex;

                    auto value = Map::get(objectiveValues, index);
                    if (i == 0) {
                        leftIndex = rightIndex = frontCopy[i + 1];
                    } else if (i == frontSize - 1) {
                        leftIndex = rightIndex = frontCopy[i - 1];
                    } else {
                        leftIndex = frontCopy[i - 1];
                        rightIndex = frontCopy[i + 1];
                    }

                    bool isMinimumBoundary = CompareUtils::almostEqual(value, minValue, 2);
                    bool isMaximumBoundary = CompareUtils::almostEqual(value, maxValue, 2);

                    if ((isMinimumBoundary || isMaximumBoundary) && infiniteBoundaryDistance) {
                        solutions[index]->crowdingDistance = INFINITY;
                    } else {
                        if (i == 0 || i == frontSize - 1) {
                            auto nearestNeighbourValue = Map::get(objectiveValues, rightIndex);
                            //the boundary points are assigned a crowding distance of twice the distance to the nearest neighbor
                            solutions[index]->crowdingDistance +=
                                    ((std::abs(nearestNeighbourValue - value)) / normalizingFactor) * 2;
                        } else {
                            const auto leftValue = Map::get(objectiveValues, leftIndex);
                            const auto rightValue = Map::get(objectiveValues, rightIndex);
                            solutions[index]->crowdingDistance += ((rightValue - leftValue) / normalizingFactor);
                        }
                    }
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
                    if (MultiObjective::dominatesOrganism(solutions[p]->organism, solutions[q]->organism, PT,
                                                          objectiveMap)) {
                        dominatedSet[p].push_back(q);
                        dominatedBy[q]++;
                    }
                        //q dominates p
                    else if (MultiObjective::dominatesOrganism(solutions[q]->organism, solutions[p]->organism, PT,
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
                    auto index = frontIndices[front][j];
                    solutions[index]->rank = front + 1;
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

    namespace HyperVolume {

        inline ObjectivePoint oppositeCorner(ObjectivePoint &point,
                                             const std::deque<ObjectivePoint> &stack,
                                             const ObjectivePoint &refPoint,
                                             const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
            //for each objective: return the next worst value for the point, if there is any
            //and construct the opposite corner from that list
            //example
            // point = [2, 0.5], Ref is [0, 0]
            // stack = {[1, 1], [0.5, 2]}
            // => oppositeCorner would be: [1, 0]
            //      first value is chosen because of the first stack value
            //      for the second value there is no dominated value in regards to the one we provided,
            //          so the ref value for that dim is chosen instead


            ObjectivePoint oppositeCorner{};

            for (auto const &objectiveEntry : objectiveMap) {
                auto objective = objectiveEntry.first;
                auto minimize = objectiveEntry.second;
                const string &objectiveKey = objective->getFormula();

                auto pointValue = Map::get(point, objectiveKey);
                std::vector<double> dominatedValues{};
                for (auto const &objectivePoint: stack) {
                    auto value = Map::get(objectivePoint, objectiveKey);
                    bool valuesAreEqual = CompareUtils::almostEqual(pointValue, value, 2);

                    if (valuesAreEqual) {
                        continue;
                    }

                    //value is better than the point's value
                    if ((minimize && value > pointValue) || (!minimize && value < pointValue)) {
                        continue;
                    }

                    dominatedValues.emplace_back(value);
                }

                if (dominatedValues.empty()) {
                    oppositeCorner[objectiveKey] = Map::get(refPoint, objectiveKey);
                } else {
                    //sort the dominated values and choose the "worst" value (i.e. the one nearest to the point's coordinates)
                    if (!minimize) {
                        std::sort(dominatedValues.begin(), dominatedValues.end(), CompareUtils::lesser());
                    } else {
                        std::sort(dominatedValues.begin(), dominatedValues.end(), CompareUtils::greater());
                    }

                    oppositeCorner[objectiveKey] = dominatedValues[0];
                }
            }

            return oppositeCorner;
        }


        /**
         * Calculates the volume between two points for the given objective space
         */
        inline double volumeBetween(ObjectivePoint &pointA,
                                    ObjectivePoint &pointB,
                                    const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
            if (objectiveMap.empty()) {
                return 0;
            }

            double volume = 1;

            for (auto const &objectiveEntry : objectiveMap) {
                auto objective = objectiveEntry.first;
                auto objectiveKey = objective->getFormula();
                auto valueA = Map::get(pointA, objectiveKey);
                auto valueB = Map::get(pointB, objectiveKey);

                auto length = std::abs(valueA - valueB);

                volume *= length;
            }

            return volume;
        }

        inline std::deque<ObjectivePoint> spawns(ObjectivePoint &point,
                                                 ObjectivePoint &oppositeCorner,
                                                 const std::deque<ObjectivePoint> &points,
                                                 const ObjectivePoint &refPoint,
                                                 const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
        ) {
            std::deque<ObjectivePoint> spawnPoints{};
            std::vector<ObjectivePoint> pointList{};
            copy(points.begin(), points.end(), inserter(pointList, pointList.end()));

            for (auto const &objectiveEntry : objectiveMap) {
                auto objective = objectiveEntry.first;
                auto objectiveKey = objective->getFormula();
                if (Map::get(oppositeCorner, objectiveKey) != Map::get(refPoint, objectiveKey)) {
                    ObjectivePoint spawnPoint{point};
                    spawnPoint[objectiveKey] = oppositeCorner[objectiveKey];

                    if (!MultiObjective::anyDominates(pointList, spawnPoint, objectiveMap)) {
                        spawnPoints.push_back(spawnPoint);
                    }
                }
            }

            return spawnPoints;
        }

        /**
         *
         */
        inline double lebMeasure(const std::vector<ObjectivePoint> &points,
                                 const ObjectivePoint &refPoint,
                                 const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap) {
            double hyperVolume = 0;

            std::deque<ObjectivePoint> stack{};
            copy(points.begin(), points.end(), inserter(stack, stack.end()));

            while (!stack.empty()) {
                //retrieve the first point
                auto &top = reinterpret_cast<ObjectivePoint &>(stack.front());

                ObjectivePoint oppositeCorner = HyperVolume::oppositeCorner(top, stack, refPoint, objectiveMap);

                hyperVolume += volumeBetween(top, oppositeCorner, objectiveMap);

                auto spawnPoints = spawns(top, oppositeCorner, stack, refPoint, objectiveMap);

                //remove the point from the stack
                stack.pop_front();

                auto size = spawnPoints.size();
                for (size_t i = 0; i < size; i++) {
                    auto &spawnPoint = reinterpret_cast<ObjectivePoint &>(spawnPoints.back());
                    stack.emplace_front(spawnPoint);
                    spawnPoints.pop_back();
                }
            }

            return hyperVolume;
        }
    }
}


#endif //MABE_MULTIOBJECTIVEUTILS_H
