//
// Created by Le on 18.02.2018.
//

#include "NeighbourhoodOptimizer.h"
#include "../MultiObjectiveSolution.h"
#include "../MultiObjectiveUtils.h"
#include "DistanceStrategy/DistanceStrategyFactory.h"


shared_ptr<ParameterLink<double>> NeighbourhoodOptimizer::neighbourhoodDistancePL = Parameters::register_parameter(
        "OPTIMIZER_NEIGHBOURHOOD-neighbourhoodDistance", 0.0,
        "distance limit to be used for neighbourhood tournaments. If the distances between organisms are within this"
                "limit, they are considered neighbours");
shared_ptr<ParameterLink<string>> NeighbourhoodOptimizer::distanceStrategyPL = Parameters::register_parameter(
        "OPTIMIZER_NEIGHBOURHOOD-distanceMeasurement", (string) "euclidean",
        "Which distance measurement is used in the neighbourhood calculation. Can be either euclidean, manhattan or chebyshev");


NeighbourhoodOptimizer::NeighbourhoodOptimizer(const shared_ptr<ParametersTable> &_PT) : MultiObjectiveOptimizer(_PT) {
    this->maxNeighbourHoodDistance = neighbourhoodDistancePL->get(PT);
    this->distanceStrategy = DistanceStrategyFactory::getFromString(distanceStrategyPL->get(PT));
}

double NeighbourhoodOptimizer::distance(int x1, int y1, int x2, int y2) {
    return this->distanceStrategy->distance(x1, y1, x2, y2);
}


vector<vector<int>> NeighbourhoodOptimizer::getNeighbourHoods(std::vector<std::shared_ptr<Organism>> &population) {
    std::unordered_map<int, std::unordered_map<int, double>> distances{};
    //calculate distances between every organism at the end of the simulation
    const auto size = population.size();
    for (auto i = 0; i < size; i++) {
        //todo vllt doch lieber mit average falls copies erstellt wurden?
        const auto xPositions1 = population[i]->dataMap.getIntVector("xPos");
        const auto yPositions1 = population[i]->dataMap.getIntVector("yPos");
        const auto x1 = xPositions1[xPositions1.size() - 1];
        const auto y1 = yPositions1[yPositions1.size() - 1];
        for (auto j = i + 1; j < size; j++) {
            const auto xPositions2 = population[j]->dataMap.getIntVector("xPos");
            const auto yPositions2 = population[j]->dataMap.getIntVector("yPos");
            const auto x2 = xPositions2[xPositions2.size() - 1];
            const auto y2 = yPositions2[yPositions2.size() - 1];
            distances[i][j] = this->distance(x1, y1, x2, y2);
            //the distance matrix is symmetrical
            distances[j][i] = distances[i][j];
        }
    }

    std::vector<std::vector<int>> neighbourHoodIndices{};
    for (auto i = 0; i < size; i++) {
        neighbourHoodIndices.emplace_back(std::vector<int>{});
        for (auto j = 0; j < size; j++) {
            if (i == j) {
                continue;
            }
            if (distances[i][j] <= maxNeighbourHoodDistance) {
                neighbourHoodIndices[i].push_back(j);
            }
        }
    }

    return neighbourHoodIndices;
}


vector<int> NeighbourhoodOptimizer::assignCopy(std::vector<std::shared_ptr<Organism>> &population,
                                               const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap,
                                               std::vector<std::vector<int>> &neighbourhoods) {
    std::vector<int> copyAssignments{};

    const auto size = population.size();
    for (auto i = 0; i < size; i++) {
        std::vector<int> dominatingIndices{};
        const auto &neighbourhood = neighbourhoods[i];
        for (const auto neighbour: neighbourhood) {
            if (MultiObjective::dominates(population[neighbour], population[i], PT, objectiveMap)) {
                dominatingIndices.push_back(neighbour);
            }
        }
        //nothing dominates the organism, either because the neighbourhood is empty or because the neighbours are not
        //good enough
        if (dominatingIndices.empty()) {
            // => make a copy of itself (or do nothing)
            copyAssignments.push_back(i);
        } else if (dominatingIndices.size() == 1) {
            //only one org dominates, which means we can directly pass it to the assignment vector
            copyAssignments.push_back(dominatingIndices[0]);
        } else {
            //more than one org dominates -> sort them
            std::vector<std::shared_ptr<MultiObjectiveSolution>> solutions{};
            for (auto index: dominatingIndices) {
                solutions.push_back(make_shared<MultiObjectiveSolution>(population[index], 0, 0));
            }

            auto frontIndicesList = MultiObjective::NsgaII::fastNonDominatedSort(solutions, PT, objectiveMap);
            for (auto &frontIndices: frontIndicesList) {
                //assign crowding distance to solutions
                MultiObjective::NsgaII::crowdingDistanceAssignment(solutions, frontIndices, PT, objectiveMap);
            }

            std::vector<int> elites{};
            std::vector<std::shared_ptr<Organism>> dummyKillList{};
            MultiObjective::NsgaII::eliteAssignment(elites, 1, solutions, dummyKillList, frontIndicesList);

            //the first (and only) element of the elites list is the best organism based on rank + crowd. distance
            copyAssignments.push_back(elites[0]);
        }
    }

    return copyAssignments;
}


void NeighbourhoodOptimizer::optimize(vector<shared_ptr<Organism>> &population) {
    const auto size = population.size();
    std::unordered_map<shared_ptr<Abstract_MTree>, bool> objectiveMap{};
    this->initObjectiveMap(objectiveMap);

    //for every objective: write best and average values to output stream
    std::cout << this->serializeObjectiveScores(population, objectiveMap) << std::endl;

    //switch between mutating and copying every other run
    if (mutate) {
        for (auto i = 0; i < size; i++) {
            vector<int> xPositions{population[i]->dataMap.getIntVector("xPos")};
            vector<int> yPositions{population[i]->dataMap.getIntVector("yPos")};
            population[i] = population[i]->makeMutatedOffspringFrom(population[i]);
            //keep the previously stored positions intact
            //the rest of the datamap is scrapped though, because we want to evaluate the mutated organism, not their ancestors
            population[i]->dataMap.set("xPos", xPositions);
            population[i]->dataMap.set("yPos", yPositions);
        }
    } else {
        auto neighbourHoodIndices = this->getNeighbourHoods(population);

        //perform tournament inside neighbourhoods => loser is replaced by copy of winner
        auto copyAssignments = this->assignCopy(population, objectiveMap, neighbourHoodIndices);


        for (auto i = 0; i < size; i++) {
            vector<int> xPositions{population[i]->dataMap.getIntVector("xPos")};
            vector<int> yPositions{population[i]->dataMap.getIntVector("yPos")};
            population[i] = population[copyAssignments[i]]->makeCopy(PT);
            //reset the datamap
            //but keep the previously stored positions intact
            population[i]->dataMap.set("xPos", xPositions);
            population[i]->dataMap.set("yPos", yPositions);

            if (i != copyAssignments[i]) {
                std::cout << std::endl;
                std::cout << i << " replaced by " << copyAssignments[i];
                if (i == (size - 1)) std::cout << std::endl;
            }
        }
    }

    mutate = !mutate;
}
