

#ifndef __BasicMarkovBrainTemplate__WorldSwarm__
#define __BasicMarkovBrainTemplate__WorldSwarm__

#include "../AbstractWorld.h"

#include "../../Brain/MarkovBrain/MarkovBrain.h"
#include "model/WorldLogEntry.h"
#include "model/WorldLog.h"
#include "model/OrganismState.h"
#include "serialize/Serializer.h"
#include "serialize/SwarmWorldSerializer.h"
#include "grid-initializers/GridInitializer.h"
#include "model/Agent.h"
#include "level/Level.h"
#include "level/Field.h"

#include <cstdlib>
#include <thread>
#include <vector>

using namespace std;

class SwarmWorld : public AbstractWorld {

protected:
    SwarmWorldSerializer serializer;
    std::unique_ptr<GridInitializer> gridInitializer;
    std::unique_ptr<Level<Field>> level;

    /**
     *
     * @param organismCount
     * @param previousStates
     * @param location
     * @param oldLocation
     * @param score
     * @param facing
     * @param waitForGoal
     * @param startSlots
     */
    virtual void initializeAgents(GridInitializer &gridInitializer, int organismCount,
                                  vector<vector<double>> &previousStates,
                                  vector<std::shared_ptr<Agent>> &organismInfos,
                                  const vector<pair<int, int>> &startSlots,
                                  std::vector<std::pair<int, int>> &alreadyUsedLocations);

    /**
     *
     * @param visualize
     * @param organismCount
     * @param org
     * @param previousStates
     * @param worldLog
     */
    virtual void initializeEvaluation(int visualize, int organismCount,
                                      const shared_ptr<Organism> &org,
                                      vector<std::shared_ptr<Agent>> &organismInfos,
                                      vector<vector<double>> &previousStates,
                                      vector<vector<double>> &pheroMap,
                                      WorldLog &worldLog);

    /**
     *
     * @param organismInfos - the scores of every individual
     * @return an aggregation of the given list of scores (i.e. the average)
     */
    virtual double getScore(const std::vector<std::shared_ptr<Agent>> &organismInfos);

    /**
     *
     * @param location
     * @param facing
     * @param senseSides
     * @param pheroMap
     * @param phero
     * @param senseAgents
     * @return
     */
    virtual vector<int> getInputs(std::pair<int, int> location, int facing, std::vector<int> senseSides,
                                  std::vector<std::vector<double>> &pheroMap, bool phero, bool senseAgents);

    const double DECAY_RATE = 0.9;

public:

    /**
     * Parameter link describing how often the world should be updated in the evaluation
     */
    static shared_ptr<ParameterLink<int>> worldUpdatesPL;
    static shared_ptr<ParameterLink<int>> gridXSizePL;
    static shared_ptr<ParameterLink<int>> gridYSizePL;
    static shared_ptr<ParameterLink<int>> hasPenaltyPL;
    static shared_ptr<ParameterLink<int>> senseAgentsPL;
    static shared_ptr<ParameterLink<int>> detectWaterPL;
    static shared_ptr<ParameterLink<double>> nAgentsPL;
    static shared_ptr<ParameterLink<string>> senseSidesPL;
    static shared_ptr<ParameterLink<int>> resetOutputsPL;
    static shared_ptr<ParameterLink<double>> penaltyPL;
    static shared_ptr<ParameterLink<int>> waitForGoalPL;
    static shared_ptr<ParameterLink<int>> pheroPL;
    static shared_ptr<ParameterLink<string>> gridInitializerPL;

    /**
     *
     */
    int generation;
    bool senseAgents;
    bool resetOutputs;
    bool hasPenalty;
    bool phero;
    double nAgents;
    vector<int> senseSides;


    int gridX;
    int gridY;
    int worldUpdates;
    double penalty;
    int waitForGoalInterval;

    vector<pair<int, int>> startSlots;


    explicit SwarmWorld(shared_ptr<ParametersTable> _PT = nullptr);

    ~SwarmWorld() override;

//    virtual void evaluate(map<string, shared_ptr<Group>>& groups, int analyse = 0, int visualize = 0, int debug = 0)override;
    void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;


    /**
     * Creates a vector of size <gridX,gridY> containing pairs of increasing values
     * @return
     */
    virtual vector<pair<int, int>> buildGrid();

    int requiredInputs() override;

    int requiredOutputs() override;

    vector<vector<double>> decay(vector<vector<double>> &pheroMap);

    int distance(pair<int, int> a, pair<int, int> b);
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSwarm__) */
