//
// Created by Le on 22.01.2018.
//

#ifndef MABE_MULTIOBJECTIVEOPTIMIZER_H
#define MABE_MULTIOBJECTIVEOPTIMIZER_H


#include "../../Organism/Organism.h"
#include "MultiObjectiveSolution.h"
#include "../AbstractOptimizer.h"

class MultiObjectiveOptimizer : public AbstractOptimizer {

private:
    void crowdingDistanceAssignment(std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
                                    std::vector<int> &frontIndices,
                                    unordered_map<std::string, double> &bestValues,
                                    const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    /**
     * A dominates B if for all objectives M it holds true that A[m] is at least as good as B[m]
     * The objectiveMap controls which objectives should be minimized or maximized (true = minimize, false = maximize)
     * @param organismA
     * @param organismB
     * @param objectiveMap
     * @return
     */
    bool dominates(const shared_ptr<Organism> &organismA, const shared_ptr<Organism> &organismB,
                   const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);


    /**
     *
     * @param solutions
     * @param objectiveMap
     * @return
     */
    vector<vector<int>> fastNonDominatedSort(
            std::vector<std::shared_ptr<MultiObjectiveSolution>> &solutions,
            const unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

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
    MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT);

    static shared_ptr<ParameterLink<int>> numberParentsPL; // number of parents (default 1, asexual)
    static shared_ptr<ParameterLink<string>> minimizeObjectivesPL; //csv list of DM-Trees to minimize
    static shared_ptr<ParameterLink<string>> maximizeObjectivesPL; //csv list of DM-Trees to maximize

    static shared_ptr<ParameterLink<string>> elitismCountPL; // this number of organisms will reproduce asexualy (i.e. copy w/ mutation)

    static shared_ptr<ParameterLink<string>> nextPopSizePL;  // number of genomes in the population

    vector<shared_ptr<Organism>> previousPopulation{};

    int numberParents;
    vector<shared_ptr<Abstract_MTree>> minimizedObjectives, maximizedObjectives;
    shared_ptr<Abstract_MTree> elitismCountMT, nextPopSizeMT;

    virtual void optimize(vector<shared_ptr<Organism>> &population) override;

    void cleanup(vector<shared_ptr<Organism>> &population) override;
};


#endif //MABE_MULTIOBJECTIVEOPTIMIZER_H