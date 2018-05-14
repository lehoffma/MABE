//
// Created by Le on 22.01.2018.
//

#include "Nsga2Optimizer.h"
#include "../../World/SwarmWorld/util/StringUtils.h"
#include "TournamentSelector.h"
#include "MultiObjectiveUtils.h"


shared_ptr<ParameterLink<int>> Nsga2Optimizer::numberParentsPL = Parameters::register_parameter(
        "OPTIMIZER_NSGA2-numberParents", 1, "number of parents used to produce offspring");

shared_ptr<ParameterLink<string>> Nsga2Optimizer::elitismCountPL = Parameters::register_parameter(
        "OPTIMIZER_NSGA2-elitismCount", (string) "1", "number of elite organisms to be added (MTree)");

shared_ptr<ParameterLink<string>> Nsga2Optimizer::nextPopSizePL = Parameters::register_parameter(
        "OPTIMIZER_NSGA2-nextPopSize", (string) "0-1",
        "size of population after optimization(MTree). -1 indicates use current population size");


Nsga2Optimizer::Nsga2Optimizer(const shared_ptr<ParametersTable> &_PT) : MultiObjectiveOptimizer(_PT) {
    numberParents = numberParentsPL->get(PT);

    stringToMTree(elitismCountPL->get(PT));
    nextPopSizeMT = stringToMTree(nextPopSizePL->get(PT));
}

void Nsga2Optimizer::optimize(vector<shared_ptr<Organism>> &population) {
    std::unordered_map<shared_ptr<Abstract_MTree>, bool> objectiveMap{};
    this->initObjectiveMap(objectiveMap);

    //for every objective: write best and average values to output stream
    std::cout << this->serializeObjectiveScores(population, objectiveMap);

    std::vector<std::shared_ptr<MultiObjectiveSolution>> solutions{};
    //combining this and the parent population for evaluation
    for (auto &organism: population) {
        solutions.push_back(make_shared<MultiObjectiveSolution>(organism, 0, 0));
    }
    for (auto &prevOrganism: previousPopulation) {
        solutions.push_back(make_shared<MultiObjectiveSolution>(prevOrganism, 0, 0));
    }

    auto popTargetSize = (int) nextPopSizeMT->eval(PT)[0];
    if (popTargetSize == -1) {
        popTargetSize = static_cast<int>(population.size());
    }
    killList.clear();

    //extract fronts from the solutions
    auto frontIndicesList = MultiObjective::NsgaII::fastNonDominatedSort(solutions, PT, objectiveMap);
    for (auto &frontIndices: frontIndicesList) {
        //assign crowding distance to solutions
        MultiObjective::NsgaII::crowdingDistanceAssignment(solutions, frontIndices, PT, objectiveMap);
    }

    auto elitismTargetSize = popTargetSize;
    std::vector<int> elites{};

    //extract N elites from the fronts
    MultiObjective::NsgaII::eliteAssignment(elites, elitismTargetSize, solutions, organismsToBeKilled,
                                            frontIndicesList);


    std::vector<std::shared_ptr<Organism>> children{};
    while (children.size() < popTargetSize) {
        vector<shared_ptr<Organism>> parents{};
        while (parents.size() < numberParents) {
            //  binary tournament selection of the elite organisms
            parents.push_back(this->binaryTournamentSelection(elites, solutions));
        }
        //  recombination and mutation
        children.push_back(parents[0]->makeMutatedOffspringFromMany(parents));
    }

    //add generated children to the new population
    for (auto &child: children) {
        population.push_back(child);
    }

    //update parent population for next iteration
    previousPopulation.clear();
    for (auto index: elites) {
        previousPopulation.push_back(solutions[index]->organism);
    }
}

std::shared_ptr<Organism> Nsga2Optimizer::binaryTournamentSelection(
        const std::vector<int> &eliteIndices,
        std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions
) {
    auto popSize = static_cast<const int>(eliteIndices.size());

    auto winner = TournamentSelector::select<std::shared_ptr<MultiObjectiveSolution>>
            (2, popSize,
             [solutions, eliteIndices]
                     (int index) -> std::shared_ptr<MultiObjectiveSolution> {
                 return solutions[eliteIndices[index]];
             },
             [](const std::shared_ptr<MultiObjectiveSolution> &solutionA,
                const std::shared_ptr<MultiObjectiveSolution> &solutionB) -> bool {
                 return *solutionA >=
                        *solutionB;
             }
            );
    return winner->organism;
}

void Nsga2Optimizer::cleanup(vector<shared_ptr<Organism>> &population) {
    vector<shared_ptr<Organism>> newPopulation;
    for (auto &org : population) {
        const auto parentIter = std::find(previousPopulation.begin(), previousPopulation.end(), org);
        const auto killListIter = std::find(organismsToBeKilled.begin(), organismsToBeKilled.end(), org);
        // if in kill list, call kill
        if (killListIter != organismsToBeKilled.end()) {
            org->kill();
        } else if (parentIter == previousPopulation.end()) { // if not in kill list and not part of the prev. population
            newPopulation.push_back(org); // move into new population
        }
    }

    population = newPopulation;
    killList.clear();
}

