

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
#include "scoring/OrganismScoringStrategy.h"
#include "model/OrganismStateContainer.h"

#include <cstdlib>
#include <thread>
#include <vector>

using namespace std;

class SwarmWorld : public AbstractWorld {

protected:
    SwarmWorldSerializer serializer;
    std::unique_ptr<GridInitializer> gridInitializer;
    std::unique_ptr<Level<Field>> level;
    std::unique_ptr<OrganismScoringStrategy> organismScoringStrategy;

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
    virtual void initializeAgents(const shared_ptr<Organism> &org, GridInitializer &gridInitializer, int organismCount,
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
    virtual void initializeEvaluation(const shared_ptr<Organism> &org, int visualize, int organismCount,
                                      vector<std::shared_ptr<Agent>> &organismInfos,
                                      vector<vector<double>> &pheroMap,
                                      WorldLog &worldLog);

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
    virtual vector<int> getInputs(std::pair<int, int> location, AbsoluteDirection facing, std::vector<int> senseSides,
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
    static shared_ptr<ParameterLink<double>> invalidMovePenaltyPL;
    static shared_ptr<ParameterLink<int>> senseAgentsPL;
    static shared_ptr<ParameterLink<int>> detectWaterPL;
    static shared_ptr<ParameterLink<double>> nAgentsPL;
    static shared_ptr<ParameterLink<string>> senseSidesPL;
    static shared_ptr<ParameterLink<int>> resetOutputsPL;
    static shared_ptr<ParameterLink<double>> penaltyPL;
    static shared_ptr<ParameterLink<int>> waitForGoalPL;
    static shared_ptr<ParameterLink<int>> pheroPL;
    static shared_ptr<ParameterLink<string>> gridInitializerPL;
    static shared_ptr<ParameterLink<string>> simulationModePL;
    static shared_ptr<ParameterLink<string>> scoringStrategyPL;
    static shared_ptr<ParameterLink<int>> repeatsPL;
    static shared_ptr<ParameterLink<int>> resetPositionsPL;

    unordered_map<string, unordered_set<string>> requiredGroups() override;

    /**
     *
     */
    bool senseAgents;
    bool resetOutputs;
    bool hasPenalty;
    bool phero;
    bool resetPositions;
    double nAgents;
    vector<int> senseSides;

    std::string simulationMode;
    int repeats;
    double invalidMovePenalty;

    int gridX;
    int gridY;
    int worldUpdates;
    double penalty;
    int waitForGoalInterval;

    vector<pair<int, int>> startSlots;


    explicit SwarmWorld(shared_ptr<ParametersTable> _PT = nullptr);

    ~SwarmWorld() override;

    void evaluate(map<string, shared_ptr<Group>> &groups, int analyse = 0, int visualize = 0, int debug = 0) override;

    void evaluateGroup(const std::vector<std::shared_ptr<Organism>> population, int visualize);

    void evaluateSolo(shared_ptr<Organism> org, int analyse, int visualize, int debug) override;


    /**
     * Creates a vector of size <gridX,gridY> containing pairs of increasing values
     * @return
     */
    virtual vector<pair<int, int>> buildGrid();

    unsigned long long int requiredInputs();

    int requiredOutputs = 2;

    vector<vector<double>> decay(vector<vector<double>> &pheroMap);

    int distance(pair<int, int> a, pair<int, int> b);


    void addToDataMap(vector<shared_ptr<Agent>> agents, const std::vector<std::shared_ptr<Organism>> population);

    void
    serializeResult(const vector<shared_ptr<Organism>> &organisms, const WorldLog &worldLog,
                    vector<OrganismStateContainer> &organismStates,
                    double globalScore);

    void simulateOnce(const shared_ptr<Agent> &agent,
                      std::unordered_map<int, std::vector<double>>& previousStates,
                      vector<vector<double>> &pheroMap);

    void
    serializeWorldUpdate(const shared_ptr<Organism> &org, WorldLog &worldLog, const vector<shared_ptr<Agent>> &agents,
                         vector<OrganismState> &organismStates, int copies, int orgIndex, int t) const;
};

#endif /* defined(__BasicMarkovBrainTemplate__WorldSwarm__) */
