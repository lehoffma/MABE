//
// Created by Le on 22.01.2018.
//

#ifndef MABE_NSGA2OPTIMIZER_H
#define MABE_NSGA2OPTIMIZER_H


#include "../../Organism/Organism.h"
#include "MultiObjectiveSolution.h"
#include "../AbstractOptimizer.h"
#include "MultiObjectiveOptimizer.h"

class Nsga2Optimizer : public MultiObjectiveOptimizer {

private:


    /**
     *
     * @param solutions
     * @return
     */
    shared_ptr<Organism> binaryTournamentSelection(
            const std::vector<int> &eliteIndices,
            std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions);

    std::vector<shared_ptr<Organism>> organismsToBeKilled{};
public:
    Nsga2Optimizer(const shared_ptr<ParametersTable> &_PT);

    static shared_ptr<ParameterLink<int>> numberParentsPL; // number of parents (default 1, asexual)
    static shared_ptr<ParameterLink<string>> minimizeObjectivesPL; //csv list of DM-Trees to minimize
    static shared_ptr<ParameterLink<string>> maximizeObjectivesPL; //csv list of DM-Trees to maximize

    static shared_ptr<ParameterLink<string>> elitismCountPL; // this number of organisms will reproduce asexualy (i.e. copy w/ mutation)

    static shared_ptr<ParameterLink<string>> nextPopSizePL;  // number of genomes in the population

    vector<shared_ptr<Organism>> previousPopulation{};

    int numberParents;
    vector<shared_ptr<Abstract_MTree>> minimizedObjectives, maximizedObjectives;
    shared_ptr<Abstract_MTree> nextPopSizeMT;

    virtual void optimize(vector<shared_ptr<Organism>> &population) override;

    void cleanup(vector<shared_ptr<Organism>> &population) override;
};


#endif //MABE_NSGA2OPTIMIZER_H
