//
// Created by Le on 18.02.2018.
//

#ifndef MABE_MULTIOBJECTIVEOPTIMIZER_H
#define MABE_MULTIOBJECTIVEOPTIMIZER_H


#include "../AbstractOptimizer.h"

class MultiObjectiveOptimizer : public AbstractOptimizer {

protected:
    void initObjectiveMap(std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    void serializeToFile(const std::vector<std::shared_ptr<Organism>> &population,
                         const vector<vector<int>> &frontIndices,
                         const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    double getHyperVolume(const std::vector<std::shared_ptr<Organism>> &population,
                          const vector<int> &optimalFrontIndices,
                          const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    double frontSpread(const std::vector<std::shared_ptr<Organism>> &population,
                       const vector<int> &frontIndices,
                       const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    double frontUniformity(const std::vector<std::shared_ptr<Organism>> &population,
                           const vector<int> &frontIndices,
                           const std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap);

    std::string serializeDataMap(DataMap &dataMap);

    std::string serializeObjectiveScores(
            std::vector<std::shared_ptr<Organism>> &population,
            std::unordered_map<shared_ptr<Abstract_MTree>, bool> &objectiveMap
    );


public:
    static shared_ptr<ParameterLink<string>> minimizeObjectivesPL; //csv list of DM-Trees to minimize
    static shared_ptr<ParameterLink<string>> maximizeObjectivesPL; //csv list of DM-Trees to maximize
    static shared_ptr<ParameterLink<string>> optimizerFileNamePL;
    static shared_ptr<ParameterLink<string>> frontDataFileNamePL;
    static shared_ptr<ParameterLink<int>> serializationIntervalPL;
    vector<shared_ptr<Abstract_MTree>> minimizedObjectives, maximizedObjectives;
    std::string optimizerFileName, frontDataFileName;
    int serializationInterval;


    MultiObjectiveOptimizer(const shared_ptr<ParametersTable> &_PT);

};


#endif //MABE_MULTIOBJECTIVEOPTIMIZER_H
