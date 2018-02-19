//
// Created by Le on 18.02.2018.
//

#ifndef MABE_NEIGHBOURHOODOPTIMIZER_H
#define MABE_NEIGHBOURHOODOPTIMIZER_H


#include "../MultiObjectiveOptimizer.h"
#include "DistanceStrategy/DistanceStrategy.h"

class NeighbourhoodOptimizer : public MultiObjectiveOptimizer {
private:
    bool mutate = false;

    double maxNeighbourHoodDistance;

    std::unique_ptr<DistanceStrategy> distanceStrategy;

    double distance(int x1, int y1, int x2, int y2);

    /**
     * Returns the neighbourhoods of the population as indices of the given vector
     * @param population
     * @return
     */
    vector<vector<int>> getNeighbourHoods(std::vector<std::shared_ptr<Organism>> &population);

    /**
     *   1: [2,3,4]
     *   2: [1,3]
     *   3: [2,4]
     *   4: [1,3]
     *   5: [4]
     *
     *   example:       1: [2,3,4] -> 2 is the only org that dominates 1 -> 1 is assigned copy of 2
     *   other example: 1: [2,3,4] -> 2 and 3 dominate 1 --> 2 dominates 3 -> 1 is assigned copy of 2
     *                                                   |-> 2 and 3 don't dominate each other
     *                                                              -> random choice
     *                                                              -> 1 is assigned copy of math.random(2,3)
     *
     * @param population
     * @param neighbourhoods
     * @return
     */
    vector<int> assignCopy(std::vector<std::shared_ptr<Organism>> &population,
                           const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap,
                           std::vector<std::vector<int>> &neighbourhoods);

public:
    NeighbourhoodOptimizer(const shared_ptr<ParametersTable> &_PT);


    static shared_ptr<ParameterLink<double>> neighbourhoodDistancePL;
    static shared_ptr<ParameterLink<string>> distanceStrategyPL;

    void optimize(vector<shared_ptr<Organism>> &population) override;
};


#endif //MABE_NEIGHBOURHOODOPTIMIZER_H
