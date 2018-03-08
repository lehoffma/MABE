//
// Created by Le on 18.02.2018.
//

#ifndef MABE_MULTIOBJECTIVEOPTIMIZER_H
#define MABE_MULTIOBJECTIVEOPTIMIZER_H


#include "../AbstractOptimizer.h"

class MultiObjectiveOptimizer: public AbstractOptimizer{

protected:
    void initObjectiveMap(std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    std::string serializeObjectiveScores(
            std::vector<std::shared_ptr<Organism>> &population,
            std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
    );
public:
    static shared_ptr<ParameterLink<string>> minimizeObjectivesPL; //csv list of DM-Trees to minimize
    static shared_ptr<ParameterLink<string>> maximizeObjectivesPL; //csv list of DM-Trees to maximize
    vector<shared_ptr<Abstract_MTree>> minimizedObjectives, maximizedObjectives;

    MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT);

};


#endif //MABE_MULTIOBJECTIVEOPTIMIZER_H
